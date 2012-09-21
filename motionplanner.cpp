#include "motionplanner.h"
#include "helpers.h"
#include "settings.h"
#include <qmath.h>
#include <QDateTime>

#define GRID_SCALE 0.1 // each cell is 0.1m long
#define TURN_ANGLE_REQUEST_THRESHOLD (5 * M_PI / 180.0)

MotionPlanner::MotionPlanner(QObject *parent) :
    QObject(parent),
    m_goalX(0),
    m_goalY(0)
{
    m_deltas << QPair<int, int>(0, -1)
             << QPair<int, int>(-1, 0)
             << QPair<int, int>(1, 0)
             << QPair<int, int>(0, 1)
             << QPair<int, int>(-1, -1)
             << QPair<int, int>(1, -1)
             << QPair<int, int>(-1, 1)
             << QPair<int, int>(1, 1);
}

bool MotionPlanner::loadGridMap(const QString &filePath)
{
    if (m_grid.load(filePath)) {
        m_map.resize(m_grid.width(), m_grid.height());
        return true;
    }
    return false;
}

void MotionPlanner::setGoal(int x, int y)
{
    m_goalX = x;
    m_goalY = y;
}

void MotionPlanner::requestNextUpdate(const Robot &robot)
{
    // Search for the best path and calculate the next required motion update.
    // Initial number of moves required to reach each position is set to infinity
    m_map.fill(INT_MAX);
    // Set the number of moves required to reach current position to 0.
    int rx = (int)(robot.position().first / GRID_SCALE + 0.5);
    int ry = (int)(robot.position().second / GRID_SCALE + 0.5);
    qDebug() << "Looking for path from" << rx << ry << "to" << m_goalX << m_goalY;
    m_map.setPoint(rx, ry, 0);

    bool isUpdated;
    do {
        isUpdated = false;

        for (int y = 0; y < m_map.height(); y++) {
            for (int x = 0; x < m_map.width(); x++) {
                int value = m_map.point(x, y);
                if (value < INT_MAX) {
                    if (expand(x, y, value))
                        isUpdated = true;
                }
            }
        }
    } while (isUpdated);

    if (m_map.point(m_goalX, m_goalY) < INT_MAX) {
        qDebug() << "Reached goal in" << m_map.point(m_goalX, m_goalY) << "steps";
        // Backtrack from the goal position to the starting position to calculate
        // the next movement update.
        QList<QIntPair> path = buildPath(robot);
        if (!path.isEmpty()) {
            requestMotionUpdate(robot, path);
        }
    } else {
        qDebug() << "Unable to reach the goal from the current position.";
        // TODO: emit an error and force robot into explore mode?
    }
}

bool MotionPlanner::expand(int x, int y, int cost)
{
    bool isUpdated = false;
    foreach (QIntPair delta, m_deltas) {
        int newX = x + delta.first;
        int newY = y + delta.second;
        int newCost = cost + 1; // TODO: Cost should be equal to sqrt(2) for 45 degrees movement
        if (newX < 0 || newX >= m_map.width() ||
            newY < 0 || newY >= m_map.height() ||
            m_map.point(newX, newY) <= newCost ||
            qGray(m_grid.pixel(newX, newY)) > 0)
            continue;
        m_map.setPoint(newX, newY, newCost);
        isUpdated = true;
    }

    return isUpdated;
}

QList<QIntPair> MotionPlanner::buildPath(const Robot &robot)
{
    int rx = (int)(robot.position().first / GRID_SCALE + 0.5);
    int ry = (int)(robot.position().second / GRID_SCALE + 0.5);

    QImage pathImage(m_grid.width(), m_grid.height(), QImage::Format_RGB888);
    for (int y = 0; y < m_map.height(); y++) {
        for (int x = 0; x < m_map.width(); x++) {
            if (m_map.point(x, y) < INT_MAX) {
                int value = qMin(m_map.point(x, y), 255);
                pathImage.setPixel(x, y, qRgb(value, 0, 0));
            } else {
                pathImage.setPixel(x, y, m_grid.pixel(x, y));
            }
        }
    }

    QList<QIntPair> path;
    int x = m_goalX, y = m_goalY;
    while (x != rx || y != ry) {
        int best = INT_MAX;
        QIntPair bestDelta(0, 0);
        foreach (QIntPair delta, m_deltas) {
            int curValue = m_map.point(x + delta.first, y + delta.second, INT_MAX);
            if (curValue < best) {
                best = curValue;
                bestDelta = delta;
            }
        }
        if (bestDelta.first == 0 && bestDelta.second == 0) {
            qDebug() << "Unable to build a path";
            return QList<QIntPair>();
        }
        x += bestDelta.first;
        y += bestDelta.second;

        pathImage.setPixel(x, y, qRgb(0, 255, 0));

        // Inverse path, because we're moving backwards from goal point
        bestDelta.first = -bestDelta.first;
        bestDelta.second = -bestDelta.second;
        path.push_front(bestDelta);
    }

    pathImage.setPixel(rx, ry, qRgb(0, 255, 255)); // initial position
    pathImage.setPixel(m_goalX, m_goalY, qRgb(255, 127, 0)); // destination
    QString filePath = PLAN_PATH + QDateTime::currentDateTime().toString("yyyy.MM.dd - hh_mm_ss_zzz") + ".png";
    pathImage.save(filePath);
    qDebug() << "Saved motion plan image to:" << filePath;

    return path;
}

void MotionPlanner::requestMotionUpdate(const Robot &robot, const QList<QIntPair> path)
{
    int distance = 0;
    QIntPair direction = path.first();
    QListIterator<QIntPair> it(path);
    while (it.hasNext()) {
        if (it.next() == direction) {
            distance++;
        }
    }
    // Now we have the direction and the total distance robot has to travel this time.

    // Note: There's a problem with qAtan2. It should be declared as qAtan2(y, x)
    qreal targetAngle = qAtan2(direction.second, direction.first);
    qDebug() << "Direction:" << targetAngle * 180.0 / M_PI << "Distance:" << distance;
    qDebug() << "Current Robot:" << robot.angle() * 180.0 / M_PI << robot.position();

    if (qAbs(targetAngle - robot.angle()) <= TURN_ANGLE_REQUEST_THRESHOLD) {
        qDebug() << "Angle is within the threshold" << targetAngle - robot.angle();
    } else {
        qreal angle = normalizeAngle(targetAngle - robot.angle());
        qDebug() << "Emitting turn request" << angle * 180.0 / M_PI;
        emit turnRequest(angle);
        return;
    }

    qDebug() << "Emitting move request" << distance * GRID_SCALE;
    emit moveRequest(distance * GRID_SCALE);
}

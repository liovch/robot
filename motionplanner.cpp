#include "motionplanner.h"
#include <qmath.h>

#define GRID_SCALE 0.1 // each cell is 0.1m long

MotionPlanner::MotionPlanner(QObject *parent) :
    QObject(parent),
    m_goalX(0),
    m_goalY(0)
{
    m_deltas << QPair<int, int>(-1, -1)
             << QPair<int, int>(0, -1)
             << QPair<int, int>(1, -1)
             << QPair<int, int>(-1, 0)
             << QPair<int, int>(1, 0)
             << QPair<int, int>(-1, 1)
             << QPair<int, int>(0, 1)
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
    Q_UNUSED(robot);
    // Search for the best path and calculate the next required motion update.
    // Initial number of moves required to reach each position is set to infinity
    m_map.fill(INT_MAX);
    // Set the number of moves required to reach current position to 0.
    int rx = (int)(robot.position().first + 0.5);
    int ry = (int)(robot.position().second + 0.5);
    if (search(rx, ry, 0)) {
        // Backtrack from the goal position to the starting position to calculate
        // the next movement update.
        QList<QIntPair> path = buildPath(robot);
        if (!path.isEmpty()) {
            Movement m = calculateMotionUpdate(robot, path);
            emit motionUpdate(m);
        }
    } else {
        qDebug() << "Unable to reach the goal from the current position.";
        // TODO: emit an error and force robot into explore mode?
    }
}

bool MotionPlanner::search(int x, int y, int cost)
{
    if (x < 0 || y < 0 || x >= m_map.width() || y >= m_map.height())
        return false;

    // TODO: Maybe change this to iterate throught the map until no improvements to
    //       the path can be made.
    if (m_map.point(x, y) <= cost) {
        return false;
    }

    if (x == m_goalX && y == m_goalY) {
        m_map.setPoint(x, y, cost);
        return true;
    }

    if (m_grid.pixel(x, y) > 0) {
        return false;
    }

    m_map.setPoint(x, y, cost);

    bool bSuccess = false;
    foreach (QIntPair delta, m_deltas) {
        if (search(x + delta.first, y + delta.second, cost + 1))
            bSuccess = true;
    }

    return bSuccess;
}

QList<QIntPair> MotionPlanner::buildPath(const Robot &robot)
{
    int rx = (int)(robot.position().first + 0.5);
    int ry = (int)(robot.position().second + 0.5);

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
        path.push_front(bestDelta);
    }

    return path;
}

Movement MotionPlanner::calculateMotionUpdate(const Robot &robot, const QList<QIntPair> path)
{
    Movement m;
    if (path.isEmpty())
        return m;

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
    m.setTurn(targetAngle - robot.angle());
    m.setForward(distance * GRID_SCALE);
    return m;
}

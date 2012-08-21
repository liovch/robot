#ifndef MOTIONPLANNER_H
#define MOTIONPLANNER_H

#include <QObject>
#include <QImage>
#include "vector2d.h"
#include "robot.h"

typedef QPair<int, int> QIntPair; // required by foreach macro

// ** This class uses predefined occupancy grid map,
//    current robot position estimated by the particle filter,
//    and the desired destination to generate next
//    motion update parameters for the robot:
//    rotation angle and distance to travel.
class MotionPlanner : public QObject
{
    Q_OBJECT
public:
    explicit MotionPlanner(QObject *parent = 0);
    
    bool loadGridMap(const QString& filePath);
    void setGoal(int x, int y);

signals:
    void motionUpdate(const Movement& m);

public slots:
    void requestNextUpdate(const Robot& robot); // estimated position of the robot

private:
    //bool search(int x, int y, int cost);
    bool expand(int x, int y, int cost);
    // builds path from goal position to the robot position specified as
    // a parameter
    QList<QIntPair> buildPath(const Robot& robot);
    Movement calculateMotionUpdate(const Robot &robot, const QList<QIntPair> path);

    QImage m_grid; // predefined occupancy map
    int m_goalX, m_goalY;
    Vector2D<int> m_map; // map where the path from the current robot position to the goal is laid out
    QList<QIntPair> m_deltas; // list of allowed movement deltas
};

#endif // MOTIONPLANNER_H

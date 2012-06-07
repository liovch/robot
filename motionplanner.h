#ifndef MOTIONPLANNER_H
#define MOTIONPLANNER_H

#include <QObject>
#include <QImage>
#include "robot.h"

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

signals:
    void motionUpdate(const Movement& m);

public slots:
    void requestNextUpdate(const Robot& robot); // estimated position of the robot

private:
    QImage m_grid;
};

#endif // MOTIONPLANNER_H

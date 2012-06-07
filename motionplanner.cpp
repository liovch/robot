#include "motionplanner.h"

MotionPlanner::MotionPlanner(QObject *parent) :
    QObject(parent)
{
}

bool MotionPlanner::loadGridMap(const QString &filePath)
{
    return m_grid.load(filePath);
}

void MotionPlanner::requestNextUpdate(const Robot &robot)
{
    Q_UNUSED(robot);
    // TODO:
    // Search for the best path and calculate the next required motion update.
    // emit motionUpdate
}

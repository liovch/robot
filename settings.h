#ifndef SETTINGS_H
#define SETTINGS_H

#include <qmath.h>
#include "qplatformdefs.h"

#define NUM_PARTICLES 1000

// TODO: How do we measure real noise?
// TODO: Should we move this out and make them global?
#define NOISE_TURN    (M_PI / 12.0) // 45 degrees
#define NOISE_FORWARD 0.1
// TODO: This is probably proportional to the distance to the object
#define NOISE_SENSE_DISTANCE 0.4 // TODO: This has to be >= 0.4,
                                 //       otherwise measurement probability can be > 1.0
#define NOISE_SENSE_ANGLE (M_PI / 36.0) // 5 degrees

#define GRID_MAP_WIDTH  20.0
#define GRID_MAP_HEIGHT 10.0
#ifndef MEEGO_EDITION_HARMATTAN
#define GRID_MAP_PATH "../robot/data/grid.png"
#define PLAN_PATH "../../data/robot/path/"
#else
#define GRID_MAP_PATH "/home/user/grid.png"
#define PLAN_PATH "/home/user/"
#endif

// TODO: Check these
#define ENCODER_WHEEL_READINGS_PER_CYCLE 20
#define WHEEL_DIAMETER 0.065
#define ROBOT_PLATFORM_RADIUS 0.085

#endif // SETTINGS_H

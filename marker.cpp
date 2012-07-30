#include "marker.h"
#include "camera.h"

Marker::Marker(QObject *parent) :
    QObject(parent),
    id(-1)
{
}

Marker::Marker(const Marker &marker):
    QObject(marker.parent())
{
    id = marker.id;
    confidence = marker.confidence;
    memcpy(pos, marker.pos, sizeof(pos));
    memcpy(line, marker.line, sizeof(line));
    memcpy(vertex, marker.vertex, sizeof(vertex));
    memcpy(modelView, marker.modelView, sizeof(modelView));
}

bool Marker::operator==(const Marker& marker)
{
    return id == marker.id && qFuzzyCompare(confidence, marker.confidence) &&
            qFuzzyCompare(pos[0], marker.pos[0]) && qFuzzyCompare(pos[1], marker.pos[1]) &&
            qFuzzyCompare(line[0][0], marker.line[0][0]) && qFuzzyCompare(line[1][0], marker.line[1][0]) && qFuzzyCompare(line[2][0], marker.line[2][0]) && qFuzzyCompare(line[3][0], marker.line[3][0]) &&
            qFuzzyCompare(line[0][1], marker.line[0][1]) && qFuzzyCompare(line[1][1], marker.line[1][1]) && qFuzzyCompare(line[2][1], marker.line[2][1]) && qFuzzyCompare(line[3][1], marker.line[3][1]) &&
            qFuzzyCompare(line[0][2], marker.line[0][2]) && qFuzzyCompare(line[1][2], marker.line[1][2]) && qFuzzyCompare(line[2][2], marker.line[2][2]) && qFuzzyCompare(line[3][2], marker.line[3][2]) &&
            qFuzzyCompare(vertex[0][0], marker.vertex[0][0]) && qFuzzyCompare(vertex[1][0], marker.vertex[1][0]) && qFuzzyCompare(vertex[2][0], marker.vertex[2][0]) && qFuzzyCompare(vertex[3][0], marker.vertex[3][0]) &&
            qFuzzyCompare(vertex[0][1], marker.vertex[0][1]) && qFuzzyCompare(vertex[1][1], marker.vertex[1][1]) && qFuzzyCompare(vertex[2][1], marker.vertex[2][1]) && qFuzzyCompare(vertex[3][1], marker.vertex[3][1]);
}

qreal Marker::distance() const
{
    return sqrt(modelView[12]*modelView[12] + modelView[13]*modelView[13] + modelView[14]*modelView[14]);
}

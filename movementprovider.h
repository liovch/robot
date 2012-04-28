#ifndef MOVEMENTPROVIDER_H
#define MOVEMENTPROVIDER_H

#include <QObject>
#include "movement.h"

class MovementProvider : public QObject
{
    Q_OBJECT
public:
    explicit MovementProvider(QObject *parent = 0);

signals:
    void nextMovement(const Movement& m);

public slots:
    void requestNextMovement();

private:
    QList<Movement> m_movements; // TODO: For now just use predefined list of movements
};

#endif // MOVEMENTPROVIDER_H

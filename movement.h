#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <QObject>

// TODO: We will probably need timestamps for movement and measurement objects.
class Movement : public QObject
{
    Q_OBJECT
public:
    explicit Movement(QObject *parent = 0);
    Movement(const Movement& m);
    Movement(qreal turn, qreal forward);
    
    qreal turn() const { return m_turn; }
    qreal forward() const { return m_forward; }

    void setTurn(qreal turn);
    void setForward(qreal forward) { m_forward = forward; }

    Movement& operator=(const Movement& m);

signals:
    
public slots:
    
private:
    qreal m_turn;
    qreal m_forward;
};

#endif // MOVEMENT_H

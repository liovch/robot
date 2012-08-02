#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <QImage>
#include <QDebug>

template <class Type>
class Vector2D : public QObject
{
public:
    Vector2D(QObject *parent = 0);
    Vector2D(int width, int height, QObject *parent = 0);
    Vector2D(const QImage& image, QObject *parent = 0);
    virtual ~Vector2D();

    void resize(int width, int height);
    Type line(int y) const;
    Type point(int x, int y) const;
    Type point(int x, int y, Type def) const; // returns def if out of range
    void setPoint(int x, int y, Type value);
    void fill(Type value);

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    int m_width, m_height;
    Type *m_data;
};

template <class Type>
Vector2D<Type>::Vector2D(QObject *parent):
    QObject(parent),
    m_width(0),
    m_height(0),
    m_data(0)
{
}

template <class Type>
Vector2D<Type>::Vector2D(int width, int height, QObject *parent):
    QObject(parent),
    m_data(0)
{
    resize(width, height);
}

template <class Type>
Vector2D<Type>::Vector2D(const QImage &image, QObject *parent):
    QObject(parent),
    m_width(image.width()),
    m_height(image.height())
{
    m_data = new int[m_width * m_height];
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            m_data[x + y * m_width] = (Type)qRed(image.pixel(x, y)); // use 1st channel of grayscale image
        }
    }
}

template <class Type>
Vector2D<Type>::~Vector2D()
{
    delete [] m_data;
}

template <class Type>
void Vector2D<Type>::resize(int width, int height)
{
    delete [] m_data;
    m_width = width;
    m_height = height;
    m_data = new Type[m_width * m_height];
    memset(m_data, 0, m_width * sizeof(m_data[0]) * m_height);
}

template <class Type>
Type Vector2D<Type>::line(int y) const
{
#ifdef DEBUG
    if (y >= m_height) {
        qDebug() << Q_FUNC_INFO << "index out of range";
        return 0;
    }
    if (!m_data) {
        qDebug() << Q_FUNC_INFO << "data is empty";
        return 0;
    }
#endif
    return m_data[y * m_width];
}

template <class Type>
Type Vector2D<Type>::point(int x, int y) const
{
#ifdef DEBUG
    if (x >= m_width || y >= m_height) {
        qDebug() << Q_FUNC_INFO << "index out of range";
        return 0;
    }
    if (!m_data) {
        qDebug() << Q_FUNC_INFO << "data is empty";
        return 0;
    }
#endif
    return m_data[x + y * m_width];
}

template <class Type>
Type Vector2D<Type>::point(int x, int y, Type def) const
{
    if (x < 0 || y < 0 || x >= m_width || y >= m_height)
        return def;

    return m_data[x + y * m_width];
}

template <class Type>
void Vector2D<Type>::setPoint(int x, int y, Type value)
{
#ifdef DEBUG
    if (x >= m_width || y >= m_height) {
        qDebug() << Q_FUNC_INFO << "index out of range";
        return;
    }
    if (!m_data) {
        qDebug() << Q_FUNC_INFO << "data is empty";
        return;
    }
#endif
    m_data[x + y * m_width] = value;
}

template <class Type>
void Vector2D<Type>::fill(Type value)
{
    for (int i = 0; i < m_width * m_height; i++)
        m_data[i] = value;
}

#endif // VECTOR2D_H

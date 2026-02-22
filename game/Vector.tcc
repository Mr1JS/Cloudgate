/*
 *  Vector<T>.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include <ostream>

namespace jumper
{


template<typename T>
Vector<T> Vector<T>::UnitX = Vector<T>(1, 0);

template<typename T>
Vector<T> Vector<T>::UnitY = Vector<T>(0, 1);

template<typename T>
std::ostream& operator<< (std::ostream& stream, const jumper::Vector<T> & vec)
{
    return stream << "(" << vec.x() << "," << vec.y() << ")" << std::endl;
}

template<typename T>
Vector<T> operator* (T s, const Vector<T>& v)
{
      return Vector<T>(v * s);
}

template<typename T>
Vector<T>::Vector(T x, T y)
        :	m_x(x), m_y(y)
{
}

template<typename T>
Vector<T>::Vector(const Vector<T> & other)
{
    m_x = other.m_x;
    m_y = other.m_y;
}

template<typename T>
Vector<T>::~Vector<T>()
{
    // Auto-generated destructor stub
}

template<typename T>
Vector<T> Vector<T>::operator =(const Vector<T> & other)
{
    if (this != &other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
    }
    return *this;
}

template<typename T>
Vector<T> Vector<T>::operator +(const Vector<T> & other) const
{
    Vector<T> p(*this);
    p += other;
    return p;
}

template<typename T>
void Vector<T>::operator +=(const Vector<T> & other)
{
    m_x += other.m_x;
    m_y += other.m_y;
}

template<typename T>
Vector<T> Vector<T>::operator *(const T& scalar) const
{
    Vector<T> p(this->m_x * scalar, this->m_y * scalar);
    return p;
}

template<typename T>
void Vector<T>::operator *=(const T& scalar)
{
    m_x *= scalar;
    m_y *= scalar;
}

template<typename T>
T Vector<T>::x() const
{
    return m_x;
}

template<typename T>
T Vector<T>::y() const
{
    return m_y;
}

template<typename T>
void Vector<T>::setX(T x)
{
    m_x = x;
}

template<typename T>
void Vector<T>::setY(T y)
{
    m_y = y;
}

template<typename T>
Vector<T> Vector<T>::operator-(const Vector<T> &other) const
{
    Vector<T> p(*this);
    p -= other;
    return p;
}

template<typename T>
void Vector<T>::operator-=(const Vector<T> &other)
{
    m_x -= other.m_x;
    m_y -= other.m_y;
}
} /* namespace jumper */

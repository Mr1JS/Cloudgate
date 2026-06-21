/**
 * @file Vector.hpp
 * @brief Defines the Vector template class for 2D/3D vector operations
 */

/*
 *  Vector.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef JUMPER_Vector_H
#define JUMPER_Vector_H

#include <iostream>

namespace jumper
{

/**
 * @brief A class to represent a Vector
 */
template<typename T>
class Vector
{
public:

    /**
     * Constructor
     * @param x		x coordinate of the Vector
     * @param y		y coordinate of the Vector
     */
    Vector(T x = 0, T y = 0);

    /**
     * Copy constructor
     *
     * @param other	The Vector that is copied
     */
    Vector(const Vector<T> & other);

    /**
     * Destructor
     */
    virtual ~Vector();

    /**
     * Assignment operator
     *
     * @param other	The Vector that is copied into this Vector
     * @return		The modified instance.
     */
    Vector<T> operator=(const Vector<T> & other);

    /**
     * Adds an offset encoded in the other Vector
     *
     * @param other	A Vector offset added to the current position
     * @return		A modified Vector
     */
    Vector<T> operator+(const Vector<T> & other) const;

    /**
     * Adds an offset encoded in the other Vector
     *
     * @param other	A Vector offset added to the current position
     */
    void operator+=(const Vector<T> & other);

    /**
     * Adds an offset encoded in the other Vector
     *
     * @param other	A Vector offset added to the current position
     * @return		A modified Vector
     */
    Vector operator-(const Vector<T> & other) const;

    /**
     * Adds an offset encoded in the other Vector
     *
     * @param other	A Vector offset added to the current position
     */
    void operator-=(const Vector<T> & other);


    /**
     * Scales the Vector
     *
     * @param other	A T with which the Vector should be scaled
     * @return		A modified Vector
     */
    Vector<T> operator*(const T& other) const;

    /**
     *
     * @param Scalar with which to multiply
     */
    void operator*=(const T& other);

    /**
     * Scales the Vector
     *
     * @param other	Dot product between two vectors
     * @return		A modified Vector
     */
    Vector<T> operator*(const Vector<T> & other) const;


    /// Returns the current x value
    T x() const;

    /// Returns the current y value
    T y() const;

    /// Sets a new x value
    void setX(T x);

    /// Sets a new y value
    void setY(T y);

    /// Stream output operator
    template<typename A>
    friend std::ostream& operator<< (std::ostream& stream, const jumper::Vector<A> & vec);

    /// Unit vector in x direction
    static Vector UnitX;

    /// Unit vector in y direction
    static Vector UnitY;
private:

    /// x coordinate of the Vector
    T m_x;

    /// y coordinate of the Vector
    T m_y;
};

/// Operator to allow left sided scaling
template<typename T>
Vector<T> operator* (T s, const Vector<T>& v);

/// Rename of Vector<double>
typedef Vector<double>  Vector2f;

/// Representation of a pixel as Vector<int>
typedef Vector<int>     Pixel;

} /* namespace jumper */

#include "game/Vector.tcc"

#endif /* SRC_Vector_HPP_ */

#pragma once

typedef struct
{
    float x;
    float y;
} Vec2;

/**
 * Creates a new 2D vector with the specified x and y components.
 *
 * @param x The x coordinate of the vector.
 * @param y The y coordinate of the vector.
 * @return A Vec2 instance initialized to (x, y).
 */
Vec2 vec2_create(float x, float y);

/**
 * Returns the sum of two 2D vectors component-wise.
 *
 * @param a The first addend vector.
 * @param b The second addend vector.
 * @return A Vec2 representing a + b.
 */
Vec2 vec2_add_vec(Vec2 a, Vec2 b);

/**
 * Adds a scalar to both components of the given 2D vector.
 *
 * @param v The vector to add the scalar to.
 * @param scalar The value to add to both x and y of v.
 * @return A Vec2 where each component is v.component + scalar.
 */
Vec2 vec2_add_scalar(Vec2 vec, float scalar);

/**
 * Subtracts the second 2D vector from the first component-wise.
 *
 * @param a The vector to subtract from.
 * @param b The vector to subtract.
 * @return A Vec2 representing a - b.
 */
Vec2 vec2_subtract_vec(Vec2 a, Vec2 b);

/**
 * Subtracts a scalar from both components of the given 2D vector.
 *
 * @param v The vector to subtract the scalar from.
 * @param scalar The value to subtract from both x and y of v.
 * @return A Vec2 where each component is v.component - scalar.
 */
Vec2 vec2_subtract_scalar(Vec2 vec, float scalar);

/**
 * Multiplies two 2D vectors component-wise.
 *
 * @param a The first operand vector.
 * @param b The second operand vector.
 * @return A Vec2 representing the component-wise product a * b.
 */
Vec2 vec2_multiply_vec(Vec2 a, Vec2 b);

/**
 * Multiplies both components of the given 2D vector by a scalar.
 *
 * @param v The vector to scale.
 * @param scalar The scale factor.
 * @return A Vec2 where each component is v.component * scalar.
 */
Vec2 vec2_multiply_scalar(Vec2 v, float scalar);

/**
 * Divides both components of the given 2D vector by a scalar.
 *
 * @param v The vector to scale.
 * @param scalar The divisor.
 * @return A Vec2 where each component is v.component / scalar.
 * @note Behavior is undefined if scalar is zero.
 */
Vec2 vec2_divide_scalar(Vec2 a, float scalar);

/**
 * Computes the Euclidean length (magnitude) of a 2D vector.
 *
 * @param v The vector whose length to compute.
 * @return The length √(x² + y²).
 */
float vec2_length(Vec2 v);

/**
 * Returns the unit vector (normalized form) of the given 2D vector.
 *
 * @param v The vector to normalize.
 * @return A Vec2 in the same direction as v with length 1.
 *         If v is the zero vector, returns Vec2{0, 0}.
 */
Vec2 vec2_normalize(Vec2 v);

/**
 * Computes the dot product of two 2D vectors.
 *
 * @param a The first operand vector.
 * @param b The second operand vector.
 * @return The scalar dot product a · b = ax*bx + ay*by.
 */
float vec2_dot(Vec2 a, Vec2 b);

/**
 * Computes the scalar cross product (perpendicular magnitude) of two 2D vectors.
 *
 * @param a The first operand vector.
 * @param b The second operand vector.
 * @return The scalar cross product a × b = ax*by - ay*bx.
 */
float vec2_cross(Vec2 a, Vec2 b);

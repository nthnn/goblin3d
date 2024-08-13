/*
 * This file is part of the Goblin3D.
 * Copyright (c) 2024 Nathanne Isip
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file goblin3d.h
 * @author [Nathanne Isip](https://github.com/nthnn)
 * @brief Header file for 3D object rendering using the Goblin3D library.
 * 
 * This file contains the function declarations and data structures necessary for
 * initializing, manipulating, and rendering 3D objects in a 2D space using the Goblin3D library.
 * The library supports basic 3D transformations, including translation, rotation, and scaling.
 * It is designed to work with microcontrollers and low-level graphical libraries.
 * 
 * The Goblin3D library provides functionalities to:
 * - Initialize and free 3D objects.
 * - Pre-calculate the 3D coordinates based on rotation angles.
 * - Render the 3D objects onto a 2D surface.
 * 
 * ## Usage Example
 * @code
 * goblin3d_obj_t cube;
 * goblin3d_init(&cube, 8, 12);
 * // Set up cube vertices and edges...

 * goblin3d_precalculate(&cube);
 * goblin3d_render(&cube, draw_function);
 * goblin3d_free(&cube);
 * @endcode
 * 
 * The library supports simple perspective projection by translating the 3D coordinates into 2D screen space.
 * It is lightweight and suitable for embedded systems with limited resources.
 * 
 * The library assumes that the Z-axis is pointing out of the screen, with positive values moving towards the viewer.
 * The perspective transformation is performed by dividing the X and Y coordinates by the Z-coordinate.
 * 
 * @note The library does not handle memory management for the content of the arrays used for points and edges. 
 * The user is responsible for providing valid data and ensuring that resources are correctly freed using `goblin3d_free`.
 */
#ifndef GOBLIN3D_H
#define GOBLIN3D_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Structure representing a 3D object for rendering using the Goblin3D library.
 * 
 * This structure contains the necessary data for representing a 3D object, including
 * the points in 3D space, edges that connect these points, and various transformation
 * parameters such as rotation angles and offsets.
 */
typedef struct {
    float** points;          /**< 2D array storing the projected 2D coordinates of each point after transformations. */
    uint32_t** edges;         /**< 2D array storing pairs of indices that represent the edges connecting the points. */
    float** orig_points;     /**< 2D array storing the original 3D coordinates of each point before any transformations. */
    float** rotated_points;  /**< 2D array storing the 3D coordinates of each point after rotation but before projection. */

    float x_offset;          /**< Horizontal offset applied to the projected points. */
    float y_offset;          /**< Vertical offset applied to the projected points. */
    float z_offset;          /**< Depth offset applied to the projected points. */
    float x_angle_deg;       /**< Rotation angle around the X-axis, in degrees. */
    float y_angle_deg;       /**< Rotation angle around the Y-axis, in degrees. */
    float z_angle_deg;       /**< Rotation angle around the Z-axis, in degrees. */

    uint32_t point_count;     /**< The number of points (vertices) in the 3D object. */
    uint32_t edge_count;      /**< The number of edges connecting the points in the 3D object. */
    float scale_size;        /**< Scaling factor applied to the projected points. */
} goblin3d_obj_t;

/**
 * @brief Type definition for a callback function used to draw lines between points.
 * 
 * This function type is used in the `goblin3d_render` function to draw lines between
 * the projected 2D points of the 3D object.
 * 
 * @param x1 The X-coordinate of the starting point of the line.
 * @param y1 The Y-coordinate of the starting point of the line.
 * @param x2 The X-coordinate of the ending point of the line.
 * @param y2 The Y-coordinate of the ending point of the line.
 */
typedef void (*goblin3d_obj_draw_fn)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * @brief Initializes a 3D object structure.
 * 
 * This function allocates memory for the points and edges of the 3D object and sets
 * up initial values for the rotation angles and offsets. If any memory allocation
 * fails, the function will free any previously allocated memory and return `false`.
 * 
 * @param obj Pointer to the `goblin3d_obj_t` structure to initialize.
 * @param point_count The number of points (vertices) in the 3D object.
 * @param edge_count The number of edges connecting the points in the 3D object.
 * @return `true` if initialization is successful, `false` otherwise.
 */
bool goblin3d_init(goblin3d_obj_t* obj, uint32_t point_count, uint32_t edge_count);

/**
 * @brief Initializes an empty Goblin3D object.
 * 
 * This function sets up an empty Goblin3D object by initializing its point and
 * edge counts to zero and setting all point and edge pointers to `NULL`. This 
 * function is useful for setting up a new object before adding points and edges 
 * to it.
 * 
 * @param obj A pointer to the Goblin3D object to initialize.
 */
void goblin3d_init_empty(goblin3d_obj_t* obj);

/**
 * @brief Frees the memory associated with a 3D object structure.
 * 
 * This function deallocates the memory used for storing the points, edges, and other
 * data in the `goblin3d_obj_t` structure.
 * 
 * @param obj Pointer to the `goblin3d_obj_t` structure to free.
 */
void goblin3d_free(goblin3d_obj_t* obj);

/**
 * @brief Precalculates the rotated and projected points for a 3D object.
 * 
 * This function applies rotation transformations to the original 3D points and
 * projects them onto a 2D plane. The rotation is applied in the order: X-axis, Y-axis,
 * and Z-axis. The projected points are then scaled and offset based on the object's
 * parameters.
 * 
 * ### Mathematical Representation
 * 
 * The rotation matrices for the X, Y, and Z axes are as follows:
 * 
 * - **Rotation around the X-axis:**
 * \f[
 * R_x(\theta) = \begin{pmatrix}
 * 1 & 0 & 0 \\
 * 0 & \cos\theta & -\sin\theta \\
 * 0 & \sin\theta & \cos\theta
 * \end{pmatrix}
 * \f]
 * 
 * - **Rotation around the Y-axis:**
 * \f[
 * R_y(\theta) = \begin{pmatrix}
 * \cos\theta & 0 & \sin\theta \\
 * 0 & 1 & 0 \\
 * -\sin\theta & 0 & \cos\theta
 * \end{pmatrix}
 * \f]
 * 
 * - **Rotation around the Z-axis:**
 * \f[
 * R_z(\theta) = \begin{pmatrix}
 * \cos\theta & -\sin\theta & 0 \\
 * \sin\theta & \cos\theta & 0 \\
 * 0 & 0 & 1
 * \end{pmatrix}
 * \f]
 * 
 * Given a point \f$(x, y, z)\f$, the rotated point is calculated as:
 * 
 * \f[
 * \begin{pmatrix}
 * x' \\
 * y' \\
 * z'
 * \end{pmatrix}
 * = R_z(\theta_z) \times R_y(\theta_y) \times R_x(\theta_x) \times \begin{pmatrix}
 * x \\
 * y \\
 * z
 * \end{pmatrix}
 * \f]
 * 
 * After applying the rotations, the 3D point is projected onto a 2D plane using the
 * formula:
 * 
 * \f[
 * x_{\text{proj}} = \frac{x'}{z'} \times scale_\text{size} + x_\text{offset}
 * \f]
 * 
 * \f[
 * y_{\text{proj}} = \frac{y'}{z'} \times scale_\text{size} + y_\text{offset}
 * \f]
 * 
 * The z-coordinate is clamped to a minimum value to avoid division by zero or very small
 * values, which could cause large distortions.
 * 
 * @param obj Pointer to the `goblin3d_obj_t` structure containing the 3D object data.
 */
void goblin3d_precalculate(goblin3d_obj_t* obj);

/**
 * @brief Renders the 3D object by drawing its edges on a 2D plane.
 * 
 * This function uses the pre-calculated 2D points to draw lines representing the
 * edges of the 3D object. A callback function is used to perform the actual drawing,
 * allowing for flexibility in the rendering method.
 * 
 * @param obj Pointer to the `goblin3d_obj_t` structure containing the 3D object data.
 * @param draw A callback function used to draw lines between the points on the 2D plane.
 */
void goblin3d_render(goblin3d_obj_t* obj, goblin3d_obj_draw_fn draw);

/**
 * @brief Adds a 3D point to a Goblin3D object.
 * 
 * This function adds a new 3D point to the Goblin3D object by reallocating memory 
 * for the points and storing the coordinates. The new point is added to both the 
 * original and rotated points arrays.
 * 
 * @param obj A pointer to the Goblin3D object.
 * @param x The x-coordinate of the point.
 * @param y The y-coordinate of the point.
 * @param z The z-coordinate of the point.
 * @return `true` if the point was added successfully, `false` if a memory allocation
 *         error occurred.
 */
bool goblin3d_add_point(goblin3d_obj_t* obj, float x, float y, float z);

/**
 * @brief Checks if an edge exists between two vertices in a Goblin3D object.
 * 
 * This function checks whether an edge exists between two specified vertices 
 * (identified by their indices) in the Goblin3D object's edges array. An edge 
 * is considered to exist if there is an edge in the array that connects the 
 * two vertices in either order.
 * 
 * @param obj A pointer to the Goblin3D object.
 * @param v1 The index of the first vertex.
 * @param v2 The index of the second vertex.
 * @return `true` if the edge exists, `false` otherwise.
 */
bool goblin3d_edge_exists(goblin3d_obj_t* obj, uint32_t v1, uint32_t v2);

/**
 * @brief Adds an edge between two vertices in a Goblin3D object.
 * 
 * This function adds a new edge between two vertices (identified by their indices)
 * in the Goblin3D object. If the edge already exists, the function returns `true` 
 * without adding a duplicate edge. If the edge does not exist, it is added to the 
 * edges array.
 * 
 * @param obj A pointer to the Goblin3D object.
 * @param v1 The index of the first vertex.
 * @param v2 The index of the second vertex.
 * @return `true` if the edge was added successfully, `false` if a memory allocation 
 *         error occurred.
 */
bool goblin3d_add_edge(goblin3d_obj_t* obj, uint32_t v1, uint32_t v2);

/**
 * @brief Parses an OBJ file to construct a Goblin3D object.
 * 
 * This function reads a Wavefront OBJ file and constructs a Goblin3D object from it.
 * The function initializes the object, reads vertices and faces from the file, and 
 * adds the corresponding points and edges to the Goblin3D object. The OBJ file should 
 * be formatted according to the standard OBJ file format specification.
 * 
 * @param filename The path to the OBJ file to parse.
 * @param obj A pointer to the Goblin3D object to populate.
 * @return `true` if the OBJ file was successfully parsed and the object constructed, 
 *         `false` if an error occurred (e.g., file not found, memory allocation failure).
 */
bool goblin3d_parse_obj_file(const char* filename, goblin3d_obj_t* obj);

#endif /* GOBLIN3D_H */
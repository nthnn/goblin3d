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

#include <goblin3d.h>
#include <math.h>

bool goblin3d_init(goblin3d_obj_t* obj, uint8_t point_count, uint8_t edge_count) {
    size_t fsize = sizeof(float);
    size_t fpsize = sizeof(float*);

    obj->point_count = point_count;
    obj->edge_count = edge_count;

    obj->points = (float**) malloc(fpsize * point_count);
    if(!obj->points) {
        goblin3d_free(obj);
        return false;
    }

    obj->edges = (uint8_t**) malloc(sizeof(uint8_t**) * edge_count);
    if(!obj->edges) {
        goblin3d_free(obj);
        return false;
    }

    obj->orig_points = (float**) malloc(fpsize * point_count);
    if(!obj->orig_points) {
        goblin3d_free(obj);
        return false;
    }

    obj->rotated_points = (float**) malloc(fpsize * point_count);
    if(!obj->rotated_points) {
        goblin3d_free(obj);
        return false;
    }

    for(uint8_t i = 0; i < point_count; i++) {
        obj->points[i] = (float*) malloc(fsize * 2);
        if(!obj->points[i]) {
            goblin3d_free(obj);
            return false;
        }

        obj->orig_points[i] = (float*) malloc(fsize * 3);
        if(!obj->orig_points[i]) {
            goblin3d_free(obj);
            return false;
        }

        obj->rotated_points[i] = (float*) malloc(fsize * 3);
        if(!obj->rotated_points[i]) {
            goblin3d_free(obj);
            return false;
        }
    }

    for(uint8_t i = 0; i < edge_count; i++) {
        obj->edges[i] = (uint8_t*) malloc(sizeof(uint8_t) * 2);
        if(!obj->edges[i]) {
            goblin3d_free(obj);
            return false;
        }
    }

    obj->x_angle_deg = 0.0;
    obj->y_angle_deg = 0.0;
    obj->z_angle_deg = 0.0;

    obj->x_offset = 0.0;
    obj->y_offset = 0.0;
    obj->z_offset = 0.0;

    return true;
}

void goblin3d_free(goblin3d_obj_t* obj) {
    for(uint8_t i = 0; i < obj->point_count; i++) {
        if(obj->points[i])
            free(obj->points[i]);

        if(obj->orig_points[i])
            free(obj->orig_points[i]);

        if(obj->rotated_points[i])
            free(obj->rotated_points[i]);
    }

    for(uint8_t i = 0; i < obj->edge_count; i++)
        if(obj->edges[i])
            free(obj->edges[i]);

    if(obj->points)
        free(obj->points);

    if(obj->edges)
        free(obj->edges);

    if(obj->orig_points)
        free(obj->orig_points);

    if(obj->rotated_points)
        free(obj->rotated_points);
}

void goblin3d_precalculate(goblin3d_obj_t* obj) {
    float radX = obj->x_angle_deg * 0.01745329251;
    float radY = obj->y_angle_deg * 0.01745329251;
    float radZ = obj->z_angle_deg * 0.01745329251;

    float cosX = cos(radX);
    float cosY = cos(radY);
    float cosZ = cos(radZ);

    float sinX = sin(radX);
    float sinY = sin(radY);
    float sinZ = sin(radZ);

    for(uint8_t i=0; i<obj->point_count; i++) {
        float x = obj->orig_points[i][0];
        float y = obj->orig_points[i][1];
        float z = obj->orig_points[i][2];

        float temp_y = y * cosX - z * sinX;
        z = y * sinX + z * cosX;
        y = temp_y;

        float temp_x = x * cosY + z * sinY;
        z = -x * sinY + z * cosY;
        x = temp_x;

        temp_x = x * cosZ - y * sinZ;
        y = x * sinZ + y * cosZ;
        x = temp_x;

        obj->rotated_points[i][0] = x;
        obj->rotated_points[i][1] = y;
        obj->rotated_points[i][2] = z + obj->z_offset;

        float z_clamped = z < -3.0 ? z : -3.0;
        obj->points[i][0] = round(obj->rotated_points[i][0] / z_clamped * obj->scale_size) + obj->x_offset;
        obj->points[i][1] = round(obj->rotated_points[i][1] / z_clamped * obj->scale_size) + obj->y_offset;
    }
}

void goblin3d_render(goblin3d_obj_t* obj, goblin3d_obj_draw_fn draw) {
    for(uint8_t i = 0; i < obj->edge_count; i++) {
        uint8_t start = obj->edges[i][0], end = obj->edges[i][1];
        draw(
            obj->points[start][0],
            obj->points[start][1],
            obj->points[end][0],
            obj->points[end][1]
        );
    }
}
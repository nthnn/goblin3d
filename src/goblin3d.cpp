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
#include <SD.h>

bool goblin3d_init(goblin3d_obj_t* obj, uint32_t point_count, uint32_t edge_count) {
    size_t fsize = sizeof(float);
    size_t fpsize = sizeof(float*);

    obj->point_count = point_count;
    obj->edge_count = edge_count;

    obj->points = (float**) malloc(fpsize * point_count);
    if(!obj->points) {
        goblin3d_free(obj);
        return false;
    }

    obj->edges = (uint32_t**) malloc(sizeof(uint32_t**) * edge_count);
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

    for(uint32_t i = 0; i < point_count; i++) {
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

    for(uint32_t i = 0; i < edge_count; i++) {
        obj->edges[i] = (uint32_t*) malloc(sizeof(uint32_t) * 2);
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

void goblin3d_init_empty(goblin3d_obj_t* obj) {
    obj->point_count = 0;
    obj->edge_count = 0;

    obj->points = NULL;
    obj->orig_points = NULL;
    obj->rotated_points = NULL;
    obj->edges = NULL;
}

void goblin3d_free(goblin3d_obj_t* obj) {
    for(uint32_t i = 0; i < obj->point_count; i++) {
        if(obj->points[i])
            free(obj->points[i]);

        if(obj->orig_points[i])
            free(obj->orig_points[i]);

        if(obj->rotated_points[i])
            free(obj->rotated_points[i]);
    }

    for(uint32_t i = 0; i < obj->edge_count; i++)
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

    for(uint32_t i = 0; i < obj->point_count; i++) {
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
    for(uint32_t i = 0; i < obj->edge_count; i++) {
        uint32_t start = obj->edges[i][0], end = obj->edges[i][1];
        draw(
            obj->points[start][0],
            obj->points[start][1],
            obj->points[end][0],
            obj->points[end][1]
        );
    }
}

bool goblin3d_add_point(goblin3d_obj_t* obj, float x, float y, float z) {
    obj->point_count++;

    obj->orig_points = (float**) realloc(obj->orig_points, obj->point_count * sizeof(float*));
    if(obj->orig_points == NULL)
        return false;

    obj->rotated_points = (float**) realloc(obj->rotated_points, obj->point_count * sizeof(float*));
    if(obj->rotated_points == NULL)
        return false;

    obj->points = (float**) realloc(obj->points, obj->point_count * sizeof(float*));
    if(obj->points == NULL)
        return false;

    obj->orig_points[obj->point_count - 1] = (float*) malloc(3 * sizeof(float));
    if(obj->orig_points == NULL)
        return false;

    obj->rotated_points[obj->point_count - 1] = (float*) malloc(3 * sizeof(float));
    if(obj->rotated_points == NULL)
        return false;

    obj->points[obj->point_count - 1] = (float*) malloc(3 * sizeof(float));
    if(obj->points == NULL)
        return false;

    obj->orig_points[obj->point_count - 1][0] = x;
    obj->orig_points[obj->point_count - 1][1] = y;
    obj->orig_points[obj->point_count - 1][2] = z;

    return true;
}

bool goblin3d_edge_exists(goblin3d_obj_t* obj, uint32_t v1, uint32_t v2) {
    for(int i = 0; i < obj->edge_count; ++i) {
        uint32_t existing_v1 = obj->edges[i][0],
            existing_v2 = obj->edges[i][1];
        if((existing_v1 == v1 && existing_v2 == v2) ||
            (existing_v1 == v2 && existing_v2 == v1))
            return true;
    }

    return false;
}

bool goblin3d_add_edge(goblin3d_obj_t* obj, uint32_t v1, uint32_t v2) {
    if(goblin3d_edge_exists(obj, v1, v2))
        return true;

    obj->edge_count++;
    obj->edges = (uint32_t**) realloc(obj->edges, obj->edge_count * sizeof(uint32_t*));
    if(obj->edges == NULL)
        return false;

    obj->edges[obj->edge_count - 1] = (uint32_t*)malloc(2 * sizeof(uint32_t));
    obj->edges[obj->edge_count - 1][0] = v1;
    obj->edges[obj->edge_count - 1][1] = v2;

    return true;
}

bool goblin3d_parse_obj_file(const char* filename, goblin3d_obj_t* obj) {
    File file = SD.open(filename);
    if(!file)
        return false;
    goblin3d_init_empty(obj);

    String line = "";
    while(file.available()) {
        char c = file.read();

        if(c == '\n' || c == '\r') {
            if(line.startsWith("v ")) {
                float x, y, z;

                sscanf(line.c_str() + 2, "%f %f %f", &x, &y, &z);
                if (!goblin3d_add_point(obj, x, y, z)) {
                    file.close();
                    return false;
                }
            }
            else if(line.startsWith("f ")) {
                uint32_t vertex_indices[4];
                int count = sscanf(
                    line.c_str() + 2, 
                    "%u %u %u %u", 
                    &vertex_indices[0], 
                    &vertex_indices[1], 
                    &vertex_indices[2], 
                    &vertex_indices[3]
                );

                if(count == 3) {
                    goblin3d_add_edge(obj, vertex_indices[0] - 1, vertex_indices[1] - 1);
                    goblin3d_add_edge(obj, vertex_indices[1] - 1, vertex_indices[2] - 1);
                    goblin3d_add_edge(obj, vertex_indices[2] - 1, vertex_indices[0] - 1);
                }
                else if(count == 4) {
                    goblin3d_add_edge(obj, vertex_indices[0] - 1, vertex_indices[1] - 1);
                    goblin3d_add_edge(obj, vertex_indices[1] - 1, vertex_indices[2] - 1);
                    goblin3d_add_edge(obj, vertex_indices[2] - 1, vertex_indices[3] - 1);
                    goblin3d_add_edge(obj, vertex_indices[3] - 1, vertex_indices[0] - 1);
                }
            }
            else if(line[0] == 'm' || line[0] == 'o' ||
                line[0] == '#' || line[0] == 'g' ||
                line[0] == 's' || line[0] == 'u' ||
                line[0] == 'n');

            line = "";
        }
        else line += c;
    }

    file.close();
    return true;
}
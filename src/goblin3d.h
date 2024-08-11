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

#ifndef GOBLIN3D_H
#define GOBLIN3D_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float** points;
  uint8_t** edges;
  float** orig_points;
  float** rotated_points;

  float x_offset, y_offset, z_offset;
  float x_angle_deg, y_angle_deg, z_angle_deg;

  uint8_t point_count, edge_count;
  float scale_size;
} goblin3d_obj_t;

typedef void (*goblin3d_obj_draw_fn)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

bool goblin3d_init(goblin3d_obj_t* obj, uint8_t point_count, uint8_t edge_count);
void goblin3d_free(goblin3d_obj_t* obj);
void goblin3d_precalculate(goblin3d_obj_t* obj);
void goblin3d_render(goblin3d_obj_t* obj, goblin3d_obj_draw_fn draw);

#endif
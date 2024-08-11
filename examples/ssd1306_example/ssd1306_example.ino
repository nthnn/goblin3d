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

#include <Adafruit_SSD1306.h>   // Include the library for SSD1306 OLED display
#include <Wire.h>               // Include the Wire library for I2C communication
#include <goblin3d.h>           // Include the Goblin3D library for 3D rendering

Adafruit_SSD1306 display(128, 64, &Wire, -1);   // Initialize the SSD1306 display with 128x64 resolution
goblin3d_obj_t cube;                            // Declare a 3D object using the Goblin3D structure

/*
 * This function will be passed to Goblin3D's
 * render function to draw edges between 3D
 * points onto the 2D display.
 */
void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    display.drawLine(x1, y1, x2, y2, WHITE);  // Draw a white line between the given coordinates
}

void setup() {
    // Define the 3D coordinates of the cube's vertices
    float cube_points[9][3] = {
        { -1.0,  -1.0,   1.0 },
        {  1.0,  -1.0,   1.0 },
        {  1.0,   1.0,   1.0 },
        { -1.0,   1.0,   1.0 },
        { -1.0,  -1.0,  -1.0 },
        {  1.0,  -1.0,  -1.0 },
        {  1.0,   1.0,  -1.0 },
        { -1.0,   1.0,  -1.0 },
        {  0.0,   3.0,   0.0 }
    };

    // Define the edges of the cube, connecting pairs of vertices
    uint8_t cube_edges[16][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Base edges
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Top edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7},  // Vertical edges
        {2, 8}, {3, 8}, {6, 8}, {7, 8}   // Pyramid edges
    };

    // Initialize the Goblin3D object (cube) with 9 points and 16 edges
    if(!goblin3d_init(&cube, 9, 16)) {
        Serial.begin(115200);                           // Start serial communication for debugging
        Serial.println("Failed to initialize object."); // Print error message if initialization fails

        while(true); // Halt execution if initialization fails
    }

    // Set the scaling factor for the 3D object
    cube.scale_size = 30.0;
    // Set the initial rotation angle around the X-axis
    cube.x_angle_deg = 20.0;
    // Set the initial X and Y offset to center the object on the display
    cube.x_offset = 64;
    cube.y_offset = 32;

    // Copy the predefined cube points to the Goblin3D object's original points array
    for(uint8_t i = 0; i < 9; i++)
        for(uint8_t j = 0; j < 3; j++)
            cube.orig_points[i][j] = cube_points[i][j];

    // Copy the predefined cube edges to the Goblin3D object's edges array
    for(uint8_t i = 0; i < 16; i++)
        for(uint8_t j = 0; j < 2; j++)
            cube.edges[i][j] = cube_edges[i][j];

    // Initialize the OLED display with I2C address 0x3C
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
    // Continuously update the rotation angles for a rotating effect
    cube.x_angle_deg = fmod(cube.x_angle_deg + 1.0, 360.0);  // Increment X rotation
    cube.y_angle_deg = fmod(cube.y_angle_deg + 1.0, 360.0);  // Increment Y rotation
    cube.z_angle_deg = fmod(cube.z_angle_deg + 1.0, 360.0);  // Increment Z rotation

    display.clearDisplay();             // Clear the display before drawing the new frame

    goblin3d_precalculate(&cube);       // Recalculate the 3D coordinates based on the current rotation angles
    goblin3d_render(&cube, &drawLine);  // Render the cube by drawing the edges on the display

    display.display();                  // Update the display with the newly drawn frame
}

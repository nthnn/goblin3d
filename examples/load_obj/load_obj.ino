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

#include <SPI.h>                // Include the SPI library for SPI communication
#include <SD.h>                 // Include the SD card library for loading files
#include <TFT_eSPI.h>           // Include the TFT_eSPI for display driver
#include <goblin3d.h>           // Include the Goblin3D library for 3D rendering

#define SD_CS      2            // SD card chip select pin
#define SD_SCK     14           // SD card SPI clock pin
#define SD_MOSI    13           // SD card SPI MOSI pin
#define SD_MISO    12           // SD card SPI MISO pin

TFT_eSPI tft = TFT_eSPI(320, 240);   // Initialize the TFT display with 320x240 resolution
SPIClass sdSpi(HSPI);                // SPI instance for SD card
goblin3d_obj_t obj;                  // Declare a 3D object using the Goblin3D structure

/*
 * This function will be passed to Goblin3D's
 * render function to draw edges between 3D
 * points onto the 2D display.
 */
void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    tft.drawLine(x1, y1, x2, y2, TFT_WHITE);  // Draw a white line between the given coordinates
}

void setup() {
    // Initialize SD card
    sdSpi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if(!SD.begin(SD_CS, sdSpi, 80000000)) {
        Serial.begin(115200);                           // Start serial communication for debugging
        Serial.println("Card mount failed.");           // Print error message if initialization fails

        while(true); // Halt execution if initialization fails
    }

    // Load the /sphere.obj file as Goblin3D object
    if(!goblin3d_parse_obj_file("/sphere.obj", &obj)) {
        Serial.begin(115200);                           // Start serial communication for debugging
        Serial.println("Failed to initialize object."); // Print error message if initialization fails

        while(true); // Halt execution if initialization fails
    }

    // Set the scaling factor for the 3D object
    obj.scale_size = 230.0;
    // Set the initial X and Y offset to center the object on the display
    obj.x_offset = 160;
    obj.y_offset = 120;

    // Initialize the TFT display with black background color
    tft.init();
    tft.fillScreen(ILI9341_BLACK);
}

void loop() {
    // Continuously update the rotation angles for a rotating effect
    obj.x_angle_deg = fmod(obj.x_angle_deg + 1.0, 360.0);  // Increment X rotation
    obj.y_angle_deg = fmod(obj.y_angle_deg + 1.0, 360.0);  // Increment Y rotation
    obj.z_angle_deg = fmod(obj.z_angle_deg + 1.0, 360.0);  // Increment Z rotation

    goblin3d_precalculate(&obj);       // Perform rendition pre-calculations

    tft.startWrite();                   // Start the rendition SPI transaction
    tft.fillScreen(ILI9341_BLACK);      // Clear the display before drawing the new frame
    goblin3d_render(&obj, &drawLine);  // Render the object on the TFT display
    tft.endWrite();                     // End the SPI transaction

    delay(10);                          // Sleep after ending the transaction
}

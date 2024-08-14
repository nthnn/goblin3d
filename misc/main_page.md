@mainpage Goblin3D
@tableofcontents

![Arduino CI](https://github.com/nthnn/goblin3d/actions/workflows/arduino_ci.yml/badge.svg)
![Arduino Lint](https://github.com/nthnn/goblin3d/actions/workflows/arduino_lint.yml/badge.svg)
![SDL2 Port CI](https://github.com/nthnn/goblin3d/actions/workflows/sdl2_ci.yml/badge.svg)
![Arduino Release](https://img.shields.io/badge/Library%20Manager-v0.0.2-red?logo=Arduino)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/nthnn/goblin3d/blob/main/LICENSE)

**Goblin3D** is a lightweight, dependency-free graphics engine designed for rendering 3D wireframes on monochromatic displays and TFT LCDs using the Arduino platform. Goblin3D is perfect for embedded systems where resources are limited but 3D rendering capabilities are desired.

<p align="center">
    <img src="https://raw.githubusercontent.com/nthnn/goblin3d/main/assets/goblin3d_demo.jpg" width="480" alt="Goblin3D demo"/>
    <br/>
    Goblin3D demo with ESP32 on TFT and OLED.
</p>

> This library is also compatible with desktop environments. You can simply integrate it with SLD2, OpenGL, et cetera.

## Features

- **3D Wireframe Rendering**: Render basic 3D shapes as wireframes on displays.
- **Flexible Display Support**: Compatible with various display types, including OLEDs and TFT LCDs.
- **No External Dependencies**: Goblin3D is a standalone library that does not require any additional libraries, making it easy to integrate into any Arduino project.
- **Customizable Objects**: Easily define and manipulate custom 3D objects with your own vertices and edges.
- **Rotation and Scaling**: Support for rotating and scaling objects in 3D space.
- **Directly `*.obj` Rendering**: Goblin3D can render `*.obj` files made with Blender directly from SD card.

<p align="center">
    <img src="https://raw.githubusercontent.com/nthnn/goblin3d/main/assets/goblin3d_sdl2.png" alt="Goblin3D port with SDL2"/>
    <br/>
    Goblin3D port with SDL2
</p>

## Installation

### Arduino Library Manager

1. Open the Arduino IDE.
2. Go to **Sketch** > **Include Library** > **Manage Libraries...**
3. Search for **Goblin3D**.
4. Click **Install**.

### Manual Installation

1. Download the latest release from the [Releases](https://github.com/nthnn/goblin3d/releases) page.
2. Unzip the downloaded file.
3. Move the `Goblin3D` folder to your Arduino libraries directory (typically `~/Documents/Arduino/libraries/` on Windows and Linux, `~/Documents/Arduino/libraries/` on macOS).
4. Restart the Arduino IDE.

## Getting Started

### Quick Start

Goblin3D SDL2 port example is availble on Docker to try it out.

```bash
docker pull nthnn/goblin3d
docker run -it -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix goblin3d
```

### Basic Usage

Here’s a simple example of how to use Goblin3D to load an `*.obj` file from SD card to TFT ILI9341 display.

```cpp
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

    // Load the /monkey.obj file as Goblin3D object
    if(!goblin3d_parse_obj_file("/monkey.obj", &obj)) {
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
```

## Exporting from Blender

Follow these steps to export a 3D scene from Blender in a format compatible with Goblin3D:

1. **Open the Export Menu**

    In Blender, navigate to the top menu bar and select `File > Export > Wavefront (.obj)`. This will allow you to export your 3D scene in the OBJ format, which Goblin3D can read and render as a wireframe.

    <p align="center">
        <img alt="Exporting to Blender 1" src="https://raw.githubusercontent.com/nthnn/goblin3d/main/assets/screenshot_1.png" />
    </p>

2. **Configure the Export Settings**

    Before saving the file, ensure that the `Vertex Groups` option is selected. This will export the vertex information needed for wireframe rendering.

    Optionally, you can check the `Triangulate Faces` option to convert all faces into triangles, which is commonly used in wireframe rendering to ensure consistent results across different models.

    <p align="center">
        <img alt="Exporting to Blender 1" src="https://raw.githubusercontent.com/nthnn/goblin3d/main/assets/screenshot_2.png" />
    </p>

3. **Save the OBJ File**

    When saving, you do not need to include the Material File (*.mtl) since Goblin3D only renders the wireframe, and materials are not required for this purpose. Simply save the OBJ file, and it's ready for use with Goblin3D.

## Contribution and Feedback

Contributions and feedback are all welcome to enhance this library. If you encounter any issues, have suggestions for improvements, or would like to contribute code, please do so.

## License

Copyright 2024 - Nathanne Isip

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
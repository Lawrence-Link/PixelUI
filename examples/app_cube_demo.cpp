/**
 * @file app_cube_demo.cpp
 * @brief Simple application demonstrating a 3D rotating cube projection on a 2D display.
 *
 * This application uses basic trigonometry and perspective projection to render a
 * wireframe cube that continuously rotates around the X and Y axes.
 *
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include "core/app/IApplication.h"
#include "core/app/app_system.h"
#include <etl/memory.h>
#include <math.h>

/** @brief Bitmap data for the application icon (24x24). */
static const unsigned char image_sans2_bits[] = {
0xf0,0xff,0x0f,0xfc,0xff,0x3f,0xfe,0xff,0x7f,0xfe,0xff,0x7f,0xff,0xff,0xff,0x3f,0x00,0xff,0x3f,0x7f,0xfe,0xbf,0x7e,0xfd,0xbf,0x00,0xfc,0xbf,0x7e,0xfd,0xbf,0x72,0xfd,0xbf,0x6a,0xfd,0xbf,0x72,0xfd,0xbf,0x6a,0xfd,0x3f,0x00,0xfd,0x7f,0xfe,0xec,0xf3,0x00,0xc4,0xe7,0xff,0xef,0xcf,0xff,0xe3,0x1f,0xff,0xf8,0x7e,0x3c,0x7e,0xfe,0x81,0x7f,0xfc,0xff,0x3f,0xf0,0xff,0x0f
};

/** @brief Coordinates of the eight vertices of the cube in 3D space. */
float cube_vertices[8][3] = {
    {-0.3, -0.3, -0.3},
    { 0.3, -0.3, -0.3},
    { 0.3,  0.3, -0.3},
    {-0.3,  0.3, -0.3},
    {-0.3, -0.3,  0.3},
    { 0.3, -0.3,  0.3},
    { 0.3,  0.3,  0.3},
    {-0.3,  0.3,  0.3}
};

/** @brief Indices defining the 12 edges connecting the cube's vertices. */
uint8_t cube_edges[12][2] = {
    {0,1},{1,2},{2,3},{3,0}, // Bottom face edges
    {4,5},{5,6},{6,7},{7,4}, // Top face edges
    {0,4},{1,5},{2,6},{3,7}  // Side edges
};

/**
 * @brief Rotates a 3D vector (vertex) around the X and Y axes.
 *
 * Applies sequential rotation transformations based on provided angles.
 *
 * @param v The 3D vertex array (x, y, z) to be rotated (modified in place).
 * @param angleX Rotation angle around the X-axis (in radians).
 * @param angleY Rotation angle around the Y-axis (in radians).
 */
void rotate3D(float v[3], float angleX, float angleY) {
    float x = v[0], y = v[1], z = v[2];
    float sinX = sin(angleX), cosX = cos(angleX);
    float sinY = sin(angleY), cosY = cos(angleY);

    // Rotate around the X axis
    float y1 = y * cosX - z * sinX;
    float z1 = y * sinX + z * cosX;
    y = y1; z = z1;

    // Rotate around the Y axis
    float x1 = x * cosY + z * sinY;
    float z2 = -x * sinY + z * cosY;

    // Update the vertex components
    v[0] = x1;
    v[1] = y;
    v[2] = z2;
}

/**
 * @brief Projects a 3D vertex onto a 2D plane (screen) using perspective projection.
 *
 * @param v The 3D vertex array (x, y, z).
 * @param x2d Reference to store the resulting 2D x-coordinate.
 * @param y2d Reference to store the resulting 2D y-coordinate.
 * @param screenW The width of the screen in pixels.
 * @param screenH The height of the screen in pixels.
 */
void project2D(float v[3], int16_t &x2d, int16_t &y2d, int16_t screenW, int16_t screenH) {
    // Define field of view (FOV) and distance from viewer (camera)
    float fov = 64;
    float distance = 10.0;

    // Calculate the scaling factor based on distance (Z-depth) for perspective effect
    // Objects further away (larger v[2]) have a smaller factor.
    float factor = fov / (v[2] + distance);

    // Map 3D X coordinate to 2D screen X coordinate
    // The projection center is screenW/2. Scaling is applied by factor.
    x2d = static_cast<int16_t>(screenW / 2 + v[0] * factor * screenW / 16);

    // Map 3D Y coordinate to 2D screen Y coordinate (Y-axis is inverted on screen, thus the subtraction)
    // Add an offset (0.12 * screenH) to center the cube vertically on the low-resolution display
    y2d = static_cast<int16_t>(screenH / 2 - v[1] * factor * screenH / 16) + 0.12 * screenH;
}

/**
 * @brief Draws the 3D cube onto the 2D display by transforming and projecting its vertices.
 *
 * @param display Reference to the U8G2 display object.
 * @param angleX Current rotation angle around the X-axis.
 * @param angleY Current rotation angle around the Y-axis.
 */
void drawCube(U8G2 &display, float angleX, float angleY) {
    // Arrays to hold the transformed 3D coordinates and the final 2D coordinates
    float transformed[8][3];
    int16_t projected[8][2];

    // --- Vertex Transformation Loop ---
    for (int i = 0; i < 8; i++) {
        // 1. Copy the original vertex coordinates
        transformed[i][0] = cube_vertices[i][0];
        transformed[i][1] = cube_vertices[i][1];
        transformed[i][2] = cube_vertices[i][2];

        // 2. Rotate the vertex in 3D space
        rotate3D(transformed[i], angleX, angleY);

        // 3. Project the 3D vertex to 2D screen coordinates (128x64 pixels)
        project2D(transformed[i],
                  projected[i][0], projected[i][1],
                  128,
                  64);
    }

    // --- Edge Drawing Loop ---
    for (int i = 0; i < 12; i++) {
        // Get the indices of the two vertices that form the current edge
        int a = cube_edges[i][0];
        int b = cube_edges[i][1];

        // Draw a line connecting the two projected 2D points
        display.drawLine(projected[a][0], projected[a][1],
                         projected[b][0], projected[b][1]);
    }
}

/** @brief Current rotation angle around the X-axis. */
static float angleX = 0;
/** @brief Current rotation angle around the Y-axis. */
static float angleY = 0;
// Static variables below are unused but kept for reference/future use:
static float height = 28;
static bool state = 0;

/**
 * @brief Application class for demonstrating a rotating 3D cube.
 */
class CubeDemo : public IApplication {
private:
    /** @brief Reference to the main UI context. */
    PixelUI& m_ui;
public:
    /**
     * @brief Constructor for CubeDemo.
     * @param ui Reference to the PixelUI instance.
     */
    CubeDemo(PixelUI& ui, void* parameter):m_ui(ui) {};

    /**
     * @brief Main drawing function, called continuously due to `setContinousDraw(true)`.
     */
    void draw() override {
        // Signal the UI to redraw the screen
        m_ui.markDirty();
        U8G2& display = m_ui.getU8G2();

        // Draw a static title header
        display.setFont(u8g2_font_ncenB10_tr);
        display.drawStr(20, 20, "Cube Demo");

        // Render the cube using the current rotation angles
        drawCube(display, angleX, angleY);

        // Increment rotation angles for the next frame to create continuous rotation
        angleX += 0.05f;
        angleY += 0.03f;
    }

    /**
     * @brief Handles input events.
     * @param event The input event received.
     * @return true if the event was consumed, false otherwise.
     */
    bool handleInput(InputEvent event) override {
        // Check for the BACK button press to exit the application
        if (event == InputEvent::BACK) {
            requestExit();
            return true;
        }
        return false;
    }

    /**
     * @brief Setup function called when the application is entered.
     * @param cb Callback function to be executed upon application exit.
     */
    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        // Enable continuous drawing mode for smooth animation
        m_ui.setContinousDraw(true);
        // Force an initial draw
        m_ui.markDirty();
    }

    /**
     * @brief Cleanup function called when the application exits.
     */
    void onExit() override {
        // Disable continuous drawing to save power when the app is not active
        m_ui.setContinousDraw(false);
    }
};

AppItem cube_demo_app = AppItem::make<CubeDemo>("Cube Demo", image_sans2_bits);

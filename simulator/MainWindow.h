/*
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

#pragma once

#include <QMainWindow>
#include <QKeyEvent>
#include <etl/mutex.h>
#include <etl/optional.h>
#include <etl/queue.h>
#include "PixelUI.h"
#include "u8g2_wrapper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /**
     * @brief Creates a fixed-size window for the emulated display.
     * @param parent Optional parent widget.
     * @param _width Emulated display width in pixels.
     * @param _height Emulated display height in pixels.
     * @param _scale Window scale applied to each emulated pixel.
     */
    explicit MainWindow(QWidget *parent = nullptr, int _width = 128, int _height=64, int _scale = 10);

    /** @brief Destroys the simulator window. */
    ~MainWindow();

    /**
     * @brief Replaces the displayed framebuffer and updates pixel dimensions.
     * @param pixels Framebuffer to display.
     */
    void setPixels(const Framebuffer& pixels);

    /**
     * @brief Adds an input event to the bounded simulator queue.
     * @param event Event to enqueue; it is dropped when the queue is full.
     */
    void pushInputEvent(InputEvent event);

    /**
     * @brief Removes the oldest queued input event.
     * @return The event, or an empty optional when the queue is empty.
     */
    etl::optional<InputEvent> popInputEvent();
    
    // void setDisplaySize(int _width, int _height, int _scale) { // setter to display size
    //     dSize_w = _width;
    //     dSize_h = _height;
    //     dScale = _scale;
    // }
protected:
    /**
     * @brief Paints the scaled framebuffer and optional cursor coordinates.
     * @param event Qt paint event associated with the redraw.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Tracks the emulated pixel under the mouse and requests a redraw.
     * @param event Mouse movement event containing the window coordinates.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Converts a non-repeated navigation key press into an input event.
     * @param event Qt key press event.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Clears the pressed state for a released navigation key.
     * @param event Qt key release event.
     */
    void keyReleaseEvent(QKeyEvent *event) override;
    etl::queue<InputEvent, 16> inputQueue;
    etl::mutex queueMutex;
private:
    Framebuffer pixels{};

    // Height and width for each pixel block. (initial)
    int dSize_w = 128;
    int dSize_h = 64;
    int dScale = 10; // scale factor for pixel blocks

    int pixelWidth = 10; 
    int pixelHeight = 10; 

    bool m_upPressed = false;
    bool m_downPressed = false;
    bool m_leftPressed = false;
    bool m_rightPressed = false;
    bool m_enterPressed = false;
    bool m_escPressed = false;

    
};

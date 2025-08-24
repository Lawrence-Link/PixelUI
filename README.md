# U8G2 Emulator: Companion for U8g2lib Embedded Display Development 💻

**U8G2 Emulator** is a personal practice project designed to simulate the powerful [u8g2 graphics library](https://github.com/olikraus/u8g2). Built on Qt6, it was designed to be a wrapper layer for the u8g2lib, enabling you to preview the UI you wanted to construct on without having to repeatedly upload heavy non-UI related code on your embedded devices. It also allows you to debug embedded display code without the need for physical hardware. Additionally, it includes a cursor function to help you quickly determine the coordinates you want to use.

## ✨ Key Features
.
-   **Comprehensive API Coverage**: The wrapper layer was publicly derived from the U8G2 class, you can indeed use most of its API functions.
-   **Efficient Debugging & Visualization**: Offers convenient debugging and display output visualization, allowing you to quickly grasp every detail of your graphics rendering.
-   **Easy Integration**: Can be integrated into your projects as a u8g2 display driver, smoothly fitting into your existing development pipeline and reducing the learning curve.
-   **Pixel-Perfect Display**: Renders pixels from the U8G2 framebuffer using QPainter
-   **Interactive Mouse Support**: The emulator displays U8G2 coordinates when the mouse moves, aiding in debugging and precise positioning.

## 🚀 Quick Start

Get the `u8g2_emulator` example up and running on your system in just a few steps!

1.  **Clone the repository and initialize submodules:**
    ```bash
    git clone https://github.com/Lawrence-Link/u8g2-emulator.git
    cd u8g2_emulator
    git submodule update --init --recursive
    ```

2.  **Build the project (Install Qt6 support first):**
    ```bash
    mkdir build && cd build
    cmake .. -DUSE_QT6=ON
    make
    ```

3.  **Run this or..**
    ```bash
    ./u8g2_emulator.exe
    ```
    Upon running, you will see a simulated U8G2 display window showcasing various graphics drawing effects (the GraphicTest demo running).

## 💡 Usage Guide

Integrate U8G2 Emulator into your project as a pseudo display driver. You can use all the familiar u8g2 API functions by creating instance of the `U8G2Wrapper`, and the emulator will display your graphics output in a desktop window in real-time.

The `u8g2_emulator` encapsulates the u8g2 library through the `U8G2Wrapper` class, providing a `getFramebufferPixels()` method to retrieve pixel data, which is then rendered by the Qt window. An `EmulatorThread` class, inheriting from `EmuWorker`, runs the u8g2 drawing loop in a background thread and triggers Qt window updates via signals. This is crucial since you can only run Qt on the main thread, which app.exec() with block the function. You have to use the pseudo main loop (run on a different thread), and later on call `u8g2.sendBuffer();` along with `emit updateRequested();` to refresh the window.

## ❤️ Contributing

This project is a result of personal learning and practice. Issues and Pull Requests are warmly welcomed to help improve the project or discuss new ideas.

## 📄 License

This project is licensed under the GPL-3.0 License.
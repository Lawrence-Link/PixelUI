#pragma once

#include <QMainWindow>
#include <QKeyEvent>
#include <vector>
#include <queue>
#include <mutex>
#include <optional>
#include "PixelUI/pixelui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr, int _width = 128, int _height=64, int _scale = 10);
    ~MainWindow();
    void setPixels(const std::vector<std::vector<bool>>& pixels);

    void pushInputEvent(InputEvent event);
    std::optional<InputEvent> popInputEvent();
    
    // void setDisplaySize(int _width, int _height, int _scale) { // setter to display size
    //     dSize_w = _width;
    //     dSize_h = _height;
    //     dScale = _scale;
    // }
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    std::queue<InputEvent> inputQueue;
    std::mutex queueMutex;
private:
    std::vector<std::vector<bool>> pixels;

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

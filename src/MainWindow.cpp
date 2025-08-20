#include "MainWindow.h"
#include <QPainter>
#include "u8g2_wrapper.h"
#include <QMouseEvent>
#include <iostream>

QPoint mousePosU8g2 = QPoint(-1, -1);

MainWindow::MainWindow(QWidget *parent, int _width, int _height, int _scale) : QMainWindow(parent), dSize_w(_width), dSize_h(_height), dScale(_scale)
{
    setMouseTracking(true);
    setFixedSize(dSize_w * dScale, dSize_h * dScale);
    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow() {}

void MainWindow::setPixels(const std::vector<std::vector<bool>> &newPixels)
{
    pixels = newPixels;
    // 计算像素块大小，保持和窗口宽高对应
    pixelWidth = width() / (pixels.empty() ? 1 : static_cast<int>(pixels[0].size()));
    pixelHeight = height() / (pixels.empty() ? 1 : static_cast<int>(pixels.size()));
}

void MainWindow::pushInputEvent(InputEvent event)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    inputQueue.push(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return; // 忽略键盘按住后的持续重复事件
    }

    switch (event->key())
    {
    case Qt::Key_Up:
        if (!m_upPressed) {
            m_upPressed = true;
            pushInputEvent(InputEvent::UP);
        }
        break;
    case Qt::Key_Down:
        if (!m_downPressed) {
            m_downPressed = true;
            pushInputEvent(InputEvent::DOWN);
        }
        break;
    case Qt::Key_Left:
        if (!m_leftPressed) {
            m_leftPressed = true;
            pushInputEvent(InputEvent::LEFT);
            std::cout << "fuck fuck" << std::endl;
        }
        break;
    case Qt::Key_Right:
        if (!m_rightPressed) {
            m_rightPressed = true;
            pushInputEvent(InputEvent::RIGHT);
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (!m_enterPressed) {
            m_enterPressed = true;
            pushInputEvent(InputEvent::SELECT);
        }
        break;
    case Qt::Key_Escape: // 新增ESC键处理
        if (!m_escPressed) {
            m_escPressed = true;
            pushInputEvent(InputEvent::BACK);
        }
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

std::optional<InputEvent> MainWindow::popInputEvent()
{
    std::lock_guard<std::mutex> lock(queueMutex);
    if (inputQueue.empty()) {
        return std::nullopt; // 没有事件
    }
    InputEvent event = inputQueue.front();
    inputQueue.pop();
    return event;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(rect(), Qt::black);

    if (pixels.empty())
        return;

    for (int y = 0; y < static_cast<int>(pixels.size()); ++y)
    {
        for (int x = 0; x < static_cast<int>(pixels[y].size()); ++x)
        {
            if (pixels[y][x])
            {
                painter.fillRect(x * pixelWidth, y * pixelHeight, pixelWidth, pixelHeight, Qt::white);
            }
        }
    }

    if (mousePosU8g2.x() != -1)
    {
        int px = mousePosU8g2.x();
        int py = mousePosU8g2.y();

        painter.setBrush(QColor(255, 128, 0, 100));
        painter.setPen(Qt::NoPen);
        painter.drawRect(px * dScale, py * dScale, dScale, dScale);

        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 15, QFont::Bold));
        QString coordText = QString("(%1, %2)").arg(mousePosU8g2.x()).arg(mousePosU8g2.y());
        painter.drawText(10, height() - 10, coordText); // Show coordinates at the bottom left
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int u8_x = event->position().x() / dScale;
    int u8_y = event->position().y() / dScale;

    if (u8_x >= 0 && u8_x < dSize_w &&
        u8_y >= 0 && u8_y < dSize_h)
    {
        mousePosU8g2 = QPoint(u8_x, u8_y);
    }
    else
    {
        mousePosU8g2 = QPoint(-1, -1); // 鼠标不在屏幕区域
    }
    update(); // 触发重绘
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // 不处理自动重复的释放事件
    if (event->isAutoRepeat()) {
        return;
    }

    switch (event->key())
    {
    case Qt::Key_Up:
        m_upPressed = false;
        break;
    case Qt::Key_Down:
        m_downPressed = false;
        break;
    case Qt::Key_Left:
        m_leftPressed = false;
        break;
    case Qt::Key_Right:
        m_rightPressed = false;
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        m_enterPressed = false;
        break;
    case Qt::Key_Escape:
        m_escPressed = false;
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
        break;
    }
}
#include "transparentwidget.h"
#include <QEnterEvent>
#include <QApplication>

TransparentWidget::TransparentWidget(QWidget *parent)
    : QWidget(parent), drawing(false), mouseInside(false), m_isAdjustingBrushSize(false),
      m_currentTool(Tool::Pen), m_currentPenWidth(5), currentColor(255, 255, 255, 128)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true); // Needed for hover events

    m_rightClickTimer = new QTimer(this);
    m_rightClickTimer->setSingleShot(true);
    // Use the system's double-click interval to be more adaptive
    m_rightClickTimer->setInterval(QApplication::doubleClickInterval());
    connect(m_rightClickTimer, &QTimer::timeout, this, &TransparentWidget::onRightClickTimeout);
}

TransparentWidget::~TransparentWidget() {}

void TransparentWidget::setPenColor(const QColor &color)
{
    currentColor = color;
    // If the current tool is the eraser, switch back to the pen tool
    if (m_currentTool == Tool::Eraser) {
        m_currentTool = Tool::Pen;
    }
    update();
}

void TransparentWidget::setTool(Tool newTool)
{
    m_currentTool = newTool;
    update(); // Update cursor
}

void TransparentWidget::undo()
{
    if (!paths.isEmpty()) {
        undonePaths.append(paths.takeLast());
        update();
    }
}

void TransparentWidget::redo()
{
    if (!undonePaths.isEmpty()) {
        paths.append(undonePaths.takeLast());
        update();
    }
}

void TransparentWidget::clearCanvas()
{
    paths.clear();
    undonePaths.clear();
    update();
}

void TransparentWidget::onRightClickTimeout()
{
    emit rightButtonClicked();
}

void TransparentWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    mouseInside = true;
    cursorPos = event->position().toPoint();
    setCursor(Qt::BlankCursor);
    update();
}

void TransparentWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    mouseInside = false;
    unsetCursor();
    update();
}

void TransparentWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        drawing = true;
        undonePaths.clear();
        currentPath.clear();
        currentPath.append(event->position().toPoint());
        // For shapes, we need a second point to start
        if (m_currentTool != Tool::Pen && m_currentTool != Tool::Eraser) {
            currentPath.append(event->position().toPoint());
        }
        update();
    } else if (event->button() == Qt::MiddleButton) {
        m_isAdjustingBrushSize = true;
    }
}

void TransparentWidget::mouseMoveEvent(QMouseEvent *event)
{
    cursorPos = event->position().toPoint();
    if (drawing) {
        if (m_currentTool == Tool::Pen || m_currentTool == Tool::Eraser) {
            currentPath.append(event->position().toPoint());
        } else {
            // For shapes, just update the second point for live preview
            currentPath[1] = event->position().toPoint();
        }
    }
    update();
}

void TransparentWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && drawing) {
        drawing = false;
        if (!currentPath.isEmpty()) {
            QColor pathColor = (m_currentTool == Tool::Eraser) ? QColor(0, 0, 0, 0) : currentColor;
            paths.append({currentPath, pathColor, m_currentPenWidth, m_currentTool});
            currentPath.clear();
        }
        update();
    } else if (event->button() == Qt::MiddleButton) {
        m_isAdjustingBrushSize = false;
    } else if (event->button() == Qt::RightButton) {
        m_rightClickTimer->start();
    }
}

void TransparentWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Cycle through tools
        m_currentTool = static_cast<Tool>((static_cast<int>(m_currentTool) + 1) % 5);
        update(); // Update cursor to reflect new mode
    } else if (event->button() == Qt::RightButton) {
        m_rightClickTimer->stop(); // CRITICAL: Stop the timer before emitting the signal
        emit rightButtonDoubleClicked();
    } else if (event->button() == Qt::MiddleButton) {
        emit middleButtonDoubleClicked();
    }
}

void TransparentWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw all saved paths
    for (const auto &pathData : paths) {
        QPen pen(pathData.color, pathData.penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        if (pathData.tool == Tool::Eraser) {
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
        } else {
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }

        if (pathData.points.size() > 1) {
            switch (pathData.tool) {
                case Tool::Pen:
                case Tool::Eraser:
                    painter.drawPolyline(pathData.points.constData(), pathData.points.size());
                    break;
                case Tool::Line:
                    painter.drawLine(pathData.points.first(), pathData.points.last());
                    break;
                case Tool::Rectangle:
                    painter.drawRect(QRect(pathData.points.first(), pathData.points.last()));
                    break;
                case Tool::Circle:
                    painter.drawEllipse(QRect(pathData.points.first(), pathData.points.last()));
                    break;
            }
        }
    }
    
    // Restore default composition mode for drawing the current path and cursor
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Draw the current path being drawn (live preview)
    if (drawing && currentPath.size() > 1) {
        QColor pathColor = (m_currentTool == Tool::Eraser) ? QColor(0, 0, 0, 0) : currentColor;
        QPen pen(pathColor, m_currentPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        if (m_currentTool == Tool::Eraser) {
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
        }

        switch (m_currentTool) {
            case Tool::Pen:
            case Tool::Eraser:
                painter.drawPolyline(currentPath.constData(), currentPath.size());
                break;
            case Tool::Line:
                painter.drawLine(currentPath.first(), currentPath.last());
                break;
            case Tool::Rectangle:
                painter.drawRect(QRect(currentPath.first(), currentPath.last()));
                break;
            case Tool::Circle:
                painter.drawEllipse(QRect(currentPath.first(), currentPath.last()));
                break;
        }
        
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    // Draw custom cursor
    if (mouseInside) {
        if (m_currentTool == Tool::Eraser) {
            painter.setPen(Qt::white);
            painter.setBrush(Qt::transparent);
            painter.drawEllipse(cursorPos, m_currentPenWidth / 2, m_currentPenWidth / 2);
        } else {
            QPen cursorPen(currentColor);
            cursorPen.setWidth(1);
            QBrush cursorBrush(currentColor);
            painter.setPen(cursorPen);
            painter.setBrush(cursorBrush);
            painter.drawEllipse(cursorPos, m_currentPenWidth / 2, m_currentPenWidth / 2);
        }
    }
}

void TransparentWidget::wheelEvent(QWheelEvent *event)
{
    if (m_isAdjustingBrushSize) {
        int delta = (event->angleDelta().y() > 0) ? -1 : 1;
        m_currentPenWidth = std::clamp(m_currentPenWidth + delta, 1, 100);
        update();
    } else {
        if (event->angleDelta().y() > 0) {
            undo();
        } else {
            redo();
        }
    }
}

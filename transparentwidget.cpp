#include "transparentwidget.h"
#include <QEnterEvent>
#include <QApplication>

TransparentWidget::TransparentWidget(QWidget *parent)
    : QWidget(parent), drawing(false), mouseInside(false), m_isAdjustingBrushSize(false),
      m_isErasing(false), m_currentPenWidth(5), currentColor(255, 255, 255, 128)
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
    m_isErasing = false; // Always switch back to pen tool when a color is chosen
    update();
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
        update();
    } else if (event->button() == Qt::MiddleButton) {
        m_isAdjustingBrushSize = true;
    }
}

void TransparentWidget::mouseMoveEvent(QMouseEvent *event)
{
    cursorPos = event->position().toPoint();
    if (drawing) {
        currentPath.append(event->position().toPoint());
    }
    update();
}

void TransparentWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && drawing) {
        drawing = false;
        if (!currentPath.isEmpty()) {
            QColor pathColor = m_isErasing ? QColor(0, 0, 0, 0) : currentColor;
            paths.append({currentPath, pathColor, m_currentPenWidth});
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
        m_isErasing = !m_isErasing;
        update(); // Update cursor to reflect new mode
    } else if (event->button() == Qt::RightButton) {
        m_rightClickTimer->stop(); // CRITICAL: Stop the timer before emitting the signal
        emit rightButtonDoubleClicked();
    }
}

void TransparentWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw all saved paths with their specific colors and widths
    for (const auto &pathData : paths) {
        // Check if this is an eraser path
        if (pathData.color.alpha() == 0) {
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
        } else {
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }

        QPen pen(pathData.color, pathData.penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        if (pathData.points.size() > 1) {
            painter.drawPolyline(pathData.points.constData(), pathData.points.size());
        }
    }
    
    // Restore default composition mode for drawing the current path and cursor
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Draw the current path being drawn
    if (drawing && currentPath.size() > 1) {
        QColor pathColor = m_isErasing ? QColor(0, 0, 0, 0) : currentColor;
        
        if (m_isErasing) {
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
        }

        QPen pen(pathColor, m_currentPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.drawPolyline(currentPath.constData(), currentPath.size());
        
        // Restore default mode after drawing
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    // Draw custom cursor
    if (mouseInside) {
        if (m_isErasing) {
            // Eraser cursor: white circle with a black outline
            painter.setPen(Qt::white);
            painter.setBrush(Qt::transparent);
            painter.drawEllipse(cursorPos, m_currentPenWidth / 2, m_currentPenWidth / 2);
        } else {
            // Pen cursor: solid color circle
            QPen cursorPen(currentColor);
            cursorPen.setWidth(1); // Outline width
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
        int delta = (event->angleDelta().y() > 0) ? 1 : -1;
        m_currentPenWidth = std::clamp(m_currentPenWidth + delta, 1, 100);
        update(); // Redraw cursor
    } else {
        if (event->angleDelta().y() > 0) {
            undo();
        } else {
            redo();
        }
    }
}

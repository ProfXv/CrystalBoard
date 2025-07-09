#include "transparentwidget.h"
#include <QEnterEvent>

TransparentWidget::TransparentWidget(QWidget *parent)
    : QWidget(parent), drawing(false), mouseInside(false), m_isAdjustingBrushSize(false),
      m_currentPenWidth(5), currentColor(255, 255, 255, 128)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true); // Needed for hover events
}

TransparentWidget::~TransparentWidget() {}

void TransparentWidget::setPenColor(const QColor &color)
{
    currentColor = color;
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
            paths.append({currentPath, currentColor, m_currentPenWidth});
            currentPath.clear();
        }
        update();
    } else if (event->button() == Qt::MiddleButton) {
        m_isAdjustingBrushSize = false;
    }
}

void TransparentWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw all saved paths with their specific colors and widths
    for (const auto &pathData : paths) {
        QPen pen(pathData.color, pathData.penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        if (pathData.points.size() > 1) {
            painter.drawPolyline(pathData.points.constData(), pathData.points.size());
        }
    }
    
    // Draw the current path being drawn
    if (drawing && currentPath.size() > 1) {
        QPen pen(currentColor, m_currentPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.drawPolyline(currentPath.constData(), currentPath.size());
    }

    // Draw custom cursor
    if (mouseInside) {
        QPen cursorPen(currentColor);
        cursorPen.setWidth(1); // Outline width
        QBrush cursorBrush(currentColor);
        painter.setPen(cursorPen);
        painter.setBrush(cursorBrush);
        painter.drawEllipse(cursorPos, m_currentPenWidth / 2, m_currentPenWidth / 2);
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

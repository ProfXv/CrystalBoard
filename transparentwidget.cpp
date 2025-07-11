#include "transparentwidget.h"
#include <QEnterEvent>
#include <QApplication>
#include <QPainterPath>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

TransparentWidget::TransparentWidget(QWidget *parent)
    : QWidget(parent), drawing(false), mouseInside(false),
      m_currentTool(Tool::Pen), m_scrollMode(ScrollMode::History),
      m_currentPenWidth(5), currentColor(255, 255, 255, 128),
      m_showIndicator(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    m_rightClickTimer = new QTimer(this);
    m_rightClickTimer->setSingleShot(true);
    m_rightClickTimer->setInterval(QApplication::doubleClickInterval());
    connect(m_rightClickTimer, &QTimer::timeout, this, &TransparentWidget::onRightClickTimeout);

    m_indicatorTimer = new QTimer(this);
    m_indicatorTimer->setSingleShot(true);
    m_indicatorTimer->setInterval(1000); // 1 second
    connect(m_indicatorTimer, &QTimer::timeout, this, &TransparentWidget::hideModeIndicator);
}

TransparentWidget::~TransparentWidget() {}

void TransparentWidget::setPenColor(const QColor &color)
{
    currentColor = color;
    if (m_currentTool == Tool::Eraser) {
        setTool(Tool::Pen);
    }
    update();
}

void TransparentWidget::setTool(Tool newTool)
{
    m_currentTool = newTool;
    showIndicator(toolToString(m_currentTool));
    update();
}

void TransparentWidget::undo()
{
    if (!paths.isEmpty()) {
        undonePaths.append(paths.takeLast());
        showIndicator("undo");
        update();
    }
}

void TransparentWidget::redo()
{
    if (!undonePaths.isEmpty()) {
        paths.append(undonePaths.takeLast());
        showIndicator("redo");
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

void TransparentWidget::hideModeIndicator()
{
    m_showIndicator = false;
    update();
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
        if (m_currentTool != Tool::Pen && m_currentTool != Tool::Eraser) {
            currentPath.append(event->position().toPoint());
        }
        update();
    }
}

void TransparentWidget::mouseMoveEvent(QMouseEvent *event)
{
    cursorPos = event->position().toPoint();
    if (drawing) {
        if (m_currentTool == Tool::Pen || m_currentTool == Tool::Eraser) {
            currentPath.append(event->position().toPoint());
        } else {
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
        cycleScrollMode();
    } else if (event->button() == Qt::RightButton) {
        m_rightClickTimer->start();
    }
}

void TransparentWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        m_rightClickTimer->stop();
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
    
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Draw the current path being drawn
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

    // Draw custom cursor and mode indicator
    if (mouseInside) {
        // Draw cursor
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

        // Draw mode indicator text if active
        if (m_showIndicator) {
            QString text = scrollModeToString();
            if (!m_indicatorSubText.isEmpty()) {
                text += ": " + m_indicatorSubText;
            }
            
            QPoint textPos = cursorPos + QPoint(m_currentPenWidth / 2 + 15, m_currentPenWidth / 2 + 15);
            
            // Draw outline
            painter.setPen(Qt::black);
            painter.drawText(textPos + QPoint(1, 1), text);
            painter.drawText(textPos + QPoint(-1, -1), text);
            painter.drawText(textPos + QPoint(1, -1), text);
            painter.drawText(textPos + QPoint(-1, 1), text);

            // Draw text
            painter.setPen(Qt::white);
            painter.drawText(textPos, text);
        }
    }
}

void TransparentWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();
    if (delta == 0) return;

    QString subText;
    switch (m_scrollMode) {
        case ScrollMode::Color: {
            int hue, saturation, value;
            currentColor.getHsv(&hue, &saturation, &value);
            // Adjust hue, wrapping around at 0 and 359
            hue = (hue + (delta > 0 ? -5 : 5) + 360) % 360;
            currentColor.setHsv(hue, 255, 255);
            subText = QString("H: %1").arg(hue);
            emit penColorChanged(currentColor);
            break;
        }
        case ScrollMode::BrushSize:
            // Inverted: scroll up decreases, scroll down increases
            m_currentPenWidth = std::clamp(m_currentPenWidth + (delta > 0 ? -1 : 1), 1, 100);
            subText = QString("%1").arg(m_currentPenWidth);
            break;
        case ScrollMode::History:
            // Not inverted: scroll up is undo, scroll down is redo
            (delta > 0) ? undo() : redo();
            return; 
        case ScrollMode::ToolSwitch:
            // Inverted: scroll up goes to previous tool, scroll down to next
            int currentToolIndex = static_cast<int>(m_currentTool);
            int nextToolIndex = (currentToolIndex + (delta > 0 ? -1 : 1) + 5) % 5;
            setTool(static_cast<Tool>(nextToolIndex));
            return;
    }
    showIndicator(subText);
    update();
}

void TransparentWidget::cycleScrollMode()
{
    m_scrollMode = static_cast<ScrollMode>((static_cast<int>(m_scrollMode) + 1) % 4);
    showIndicator();
}

void TransparentWidget::showIndicator(const QString &subText)
{
    m_indicatorSubText = subText;
    m_showIndicator = true;
    m_indicatorTimer->start(); // Restart the timer
    update();
}

QString TransparentWidget::scrollModeToString() const
{
    switch (m_scrollMode) {
        case ScrollMode::History:   return "History";
        case ScrollMode::Color:     return "Color";
        case ScrollMode::BrushSize: return "Size";
        case ScrollMode::ToolSwitch:return "Tool";
    }
    return "";
}

QString TransparentWidget::toolToString(Tool tool) const
{
    switch (tool) {
        case Tool::Pen:       return "pen";
        case Tool::Eraser:    return "eraser";
        case Tool::Line:      return "line";
        case Tool::Rectangle: return "rectangle";
        case Tool::Circle:    return "circle";
    }
    return "";
}
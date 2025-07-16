#include "canvas.h"
#include <QEnterEvent>
#include <QApplication>
#include <QPainterPath>
#include <QTextDocument>
#include <QLineEdit>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent), drawing(false), mouseInside(false), isMiddleButtonPressed(false),
      m_currentTool(Tool::Pen), m_scrollMode(ScrollMode::History),
      m_currentPenWidth(1), m_currentTextSize(16), // Default fallback sizes
      currentColor(255, 255, 255, 255),
      m_showIndicator(false), m_textInput(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    m_rightClickTimer = new QTimer(this);
    m_rightClickTimer->setSingleShot(true);
    m_rightClickTimer->setInterval(QApplication::doubleClickInterval());
    connect(m_rightClickTimer, &QTimer::timeout, this, &Canvas::onRightClickTimeout);

    m_indicatorTimer = new QTimer(this);
    m_indicatorTimer->setSingleShot(true);
    m_indicatorTimer->setInterval(Constants::INDICATOR_TIMEOUT_MS);
    connect(m_indicatorTimer, &QTimer::timeout, this, &Canvas::hideModeIndicator);
}

Canvas::~Canvas() {}

void Canvas::setInitialPenWidth(int width)
{
    m_currentPenWidth = width;
}

void Canvas::setInitialTextSize(int size)
{
    m_currentTextSize = size;
}

void Canvas::setPenColor(const QColor &color)
{
    currentColor = color;
    if (m_currentTool == Tool::Eraser) {
        setTool(Tool::Pen);
    }
    update();
}

void Canvas::setTool(Tool newTool)
{
    m_currentTool = newTool;
    showIndicator(toolToString(m_currentTool));
    update();
}

void Canvas::undo()
{
    if (!paths.isEmpty()) {
        undonePaths.append(paths.takeLast());
        showIndicator("undo");
        update();
    }
}

void Canvas::redo()
{
    if (!undonePaths.isEmpty()) {
        paths.append(undonePaths.takeLast());
        showIndicator("redo");
        update();
    }
}

void Canvas::clearCanvas()
{
    paths.clear();
    undonePaths.clear();
    if (m_textInput) {
        m_textInput->deleteLater();
        m_textInput = nullptr;
    }
    update();
}

void Canvas::handleTextEditingFinished()
{
    if (!m_textInput) return;

    QString text = m_textInput->text();
    
    // Use the initial click position as the center reference
    QPoint centerPos = m_textClickPos;
    
    m_textInput->deleteLater();
    m_textInput = nullptr;

    if (!text.isEmpty()) {
        // Prepare to calculate text size for centering
        QFont font;
        font.setPointSize(m_currentTextSize);
        QFontMetrics fm(font);
        
        // Calculate the bounding box of the text
        QRect textRect = fm.boundingRect(text);
        
        // Calculate the top-left position to make the text's center align with centerPos
        QPoint topLeftPos = centerPos - textRect.center();

        undonePaths.clear();
        paths.append({ {topLeftPos}, currentColor, 0, Tool::Text, text, m_currentTextSize });
        update();
    }
}

void Canvas::onRightClickTimeout()
{
    emit rightButtonClicked();
}

void Canvas::hideModeIndicator()
{
    m_showIndicator = false;
    update();
}

void Canvas::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    mouseInside = true;
    cursorPos = event->position().toPoint();
    setCursor(Qt::BlankCursor);
    update();
}

void Canvas::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    mouseInside = false;
    unsetCursor();
    update();
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // If an input box already exists, finalize it before doing anything else.
        if (m_textInput) {
            handleTextEditingFinished();
        }

        if (m_currentTool == Tool::Text) {
            m_textClickPos = event->position().toPoint();
            m_textInput = new QLineEdit(this);
            connect(m_textInput, &QLineEdit::editingFinished, this, &Canvas::handleTextEditingFinished);
            
            QFont font;
            font.setPointSize(m_currentTextSize);
            m_textInput->setFont(font);
            
            m_textInput->setStyleSheet(
                "background-color: rgba(0, 0, 0, 0.5);"
                "color: white;"
                "border: 1px solid white;"
                "border-radius: 3px;"
                "padding: 2px;"
            );
            
            m_textInput->move(m_textClickPos);
            m_textInput->show();
            m_textInput->setFocus();
        } else {
            drawing = true;
            undonePaths.clear();
            currentPath.clear();
            currentPath.append(event->position().toPoint());
            // For shape tools, add a second point to be modified during mouse move.
            if (m_currentTool == Tool::Line || m_currentTool == Tool::Arrow || m_currentTool == Tool::Rectangle || m_currentTool == Tool::Circle) {
                currentPath.append(event->position().toPoint());
            }
            update();
        }
    } else if (event->button() == Qt::MiddleButton) {
        isMiddleButtonPressed = true;
        event->accept();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
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

void Canvas::mouseReleaseEvent(QMouseEvent *event)
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
        isMiddleButtonPressed = false;
        event->accept();
    } else if (event->button() == Qt::RightButton) {
        m_rightClickTimer->start();
    }
}

void Canvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_currentTool == Tool::Text) return;
        
        // A double-click is preceded by a press and release, which creates a "dot".
        // We remove that dot before toggling the view.
        if (!paths.isEmpty()) {
            paths.removeLast();
            update();
        }
        
        emit leftButtonDoubleClicked();
    } else if (event->button() == Qt::RightButton) {
        m_rightClickTimer->stop();
        emit rightButtonDoubleClicked();
    }
}

void Canvas::paintEvent(QPaintEvent *event)
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

        if (pathData.points.size() > 0) {
            switch (pathData.tool) {
                case Tool::Pen:
                case Tool::Eraser:
                    if (pathData.points.size() > 1)
                        painter.drawPolyline(pathData.points.constData(), pathData.points.size());
                    break;
                case Tool::Line:
                    if (pathData.points.size() > 1)
                        painter.drawLine(pathData.points.first(), pathData.points.last());
                    break;
                case Tool::Arrow:
                    if (pathData.points.size() > 1) {
                        QLineF line(pathData.points.first(), pathData.points.last());
                        painter.drawLine(line);
                        // Draw arrowhead
                        double angle = std::atan2(-line.dy(), line.dx());
                        qreal arrowSize = pathData.penWidth * 3;
                        QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
                        QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);
                        painter.drawPolyline(QPolygonF() << arrowP1 << line.p2() << arrowP2);
                    }
                    break;
                case Tool::Rectangle:
                     if (pathData.points.size() > 1)
                        painter.drawRect(QRect(pathData.points.first(), pathData.points.last()));
                    break;
                case Tool::Circle:
                     if (pathData.points.size() > 1)
                        painter.drawEllipse(QRect(pathData.points.first(), pathData.points.last()));
                    break;
                case Tool::Text:
                    {
                        painter.save();
                        QFont font = painter.font();
                        font.setPointSize(pathData.textSize);
                        painter.setFont(font);
                        painter.drawText(pathData.points.first(), pathData.text);
                        painter.restore();
                    }
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
            case Tool::Arrow:
                {
                    QLineF line(currentPath.first(), currentPath.last());
                    painter.drawLine(line);
                    // Draw arrowhead
                    double angle = std::atan2(-line.dy(), line.dx());
                    qreal arrowSize = m_currentPenWidth * 3;
                    QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
                    QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);
                    painter.drawPolyline(QPolygonF() << arrowP1 << line.p2() << arrowP2);
                }
                break;
            case Tool::Rectangle:
                painter.drawRect(QRect(currentPath.first(), currentPath.last()));
                break;
            case Tool::Circle:
                painter.drawEllipse(QRect(currentPath.first(), currentPath.last()));
                break;
            case Tool::Text:
                // Text is drawn directly, not interactively
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
            QString mainText = scrollModeToString();
            
            QPoint textPos = cursorPos + QPoint(m_currentPenWidth / 2 + 15, m_currentPenWidth / 2 + 15);
            
            // Draw outline
            painter.setPen(Qt::black);
            painter.drawText(textPos + QPoint(1, 1), mainText);
            painter.drawText(textPos + QPoint(-1, -1), mainText);
            painter.drawText(textPos + QPoint(1, -1), mainText);
            painter.drawText(textPos + QPoint(-1, 1), mainText);
            if (!m_indicatorSubText.isEmpty()) {
                QPoint subTextPos = textPos + QPoint(0, 18);
                painter.drawText(subTextPos + QPoint(1, 1), m_indicatorSubText);
                painter.drawText(subTextPos + QPoint(-1, -1), m_indicatorSubText);
                painter.drawText(subTextPos + QPoint(1, -1), m_indicatorSubText);
                painter.drawText(subTextPos + QPoint(-1, 1), m_indicatorSubText);
            }

            // Draw text
            painter.setPen(Qt::white);
            painter.drawText(textPos, mainText);
            if (!m_indicatorSubText.isEmpty()) {
                painter.drawText(textPos + QPoint(0, 18), m_indicatorSubText);
            }
        }
    }
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();
    if (delta == 0) return;

    if (isMiddleButtonPressed) {
        if (delta > 0) {
            cycleScrollModeBackward();
        } else {
            cycleScrollMode();
        }
        event->accept();
        return;
    }

    int h, s, v, a;
    currentColor.getHsv(&h, &s, &v, &a);
    
    switch (m_scrollMode) {
        case ScrollMode::Hue:
            h = (h + (delta > 0 ? -Constants::HUE_SENSITIVITY : Constants::HUE_SENSITIVITY) + 360) % 360;
            break;
        case ScrollMode::Saturation:
            s = std::clamp(s + (delta > 0 ? -Constants::SATURATION_SENSITIVITY : Constants::SATURATION_SENSITIVITY), 0, 255);
            break;
        case ScrollMode::Brightness:
            v = std::clamp(v + (delta > 0 ? -Constants::BRIGHTNESS_SENSITIVITY : Constants::BRIGHTNESS_SENSITIVITY), 0, 255);
            break;
        case ScrollMode::Opacity:
            a = std::clamp(a + (delta > 0 ? -Constants::OPACITY_SENSITIVITY : Constants::OPACITY_SENSITIVITY), 0, 255);
            break;
        case ScrollMode::BrushSize:
            if (m_currentTool == Tool::Text) {
                m_currentTextSize = std::max(1, m_currentTextSize + (delta > 0 ? -Constants::SIZE_SENSITIVITY : Constants::SIZE_SENSITIVITY));
                showIndicator(QString("%1pt").arg(m_currentTextSize));
            } else {
                m_currentPenWidth = std::max(1, m_currentPenWidth + (delta > 0 ? -Constants::SIZE_SENSITIVITY : Constants::SIZE_SENSITIVITY));
                showIndicator(QString("%1px").arg(m_currentPenWidth));
            }
            update();
            return; 
        case ScrollMode::History:
            (delta > 0) ? undo() : redo();
            return; 
        case ScrollMode::ToolSwitch:
            int currentToolIndex = static_cast<int>(m_currentTool);
            int nextToolIndex = (currentToolIndex + (delta > 0 ? -1 : 1) + 7) % 7;
            setTool(static_cast<Tool>(nextToolIndex));
            return;
    }

    currentColor.setHsv(h, s, v, a);
    emit penColorChanged(currentColor);
    showIndicator();
    update();
}

void Canvas::cycleScrollMode()
{
    m_scrollMode = static_cast<ScrollMode>((static_cast<int>(m_scrollMode) + 1) % 7);
    showIndicator();
}

void Canvas::cycleScrollModeBackward()
{
    int totalModes = 7;
    int currentModeIndex = static_cast<int>(m_scrollMode);
    m_scrollMode = static_cast<ScrollMode>((currentModeIndex - 1 + totalModes) % totalModes);
    showIndicator();
}

void Canvas::showIndicator(const QString &subText)
{
    if (m_scrollMode == ScrollMode::Hue || m_scrollMode == ScrollMode::Saturation || m_scrollMode == ScrollMode::Brightness || m_scrollMode == ScrollMode::Opacity) {
        int h, s, v;
        currentColor.getHsv(&h, &s, &v);
        m_indicatorSubText = QString::asprintf("H:%.2f S:%.2f B:%.2f A:%.2f", h / 359.0, s / 255.0, v / 255.0, currentColor.alphaF());
    } else {
        m_indicatorSubText = subText;
    }

    m_showIndicator = true;
    m_indicatorTimer->start(); // Restart the timer
    update();
}

QString Canvas::scrollModeToString() const
{
    switch (m_scrollMode) {
        case ScrollMode::History:    return "History";
        case ScrollMode::Hue:        return "Hue";
        case ScrollMode::Saturation: return "Saturation";
        case ScrollMode::Brightness: return "Brightness";
        case ScrollMode::Opacity:    return "Opacity";
        case ScrollMode::BrushSize:  return "Size";
        case ScrollMode::ToolSwitch: return "Tool";
    }
    return "";
}

QString Canvas::toolToString(Tool tool) const
{
    switch (tool) {
        case Tool::Pen:       return "pen";
        case Tool::Eraser:    return "eraser";
        case Tool::Text:      return "text";
        case Tool::Line:      return "line";
        case Tool::Arrow:     return "arrow";
        case Tool::Rectangle: return "rectangle";
        case Tool::Circle:    return "circle";
    }
    return "";
}

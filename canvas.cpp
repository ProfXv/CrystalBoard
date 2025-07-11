#include "canvas.h"
#include <QEnterEvent>
#include <QApplication>
#include <QPainterPath>
#include <QTextDocument>
#include <QLineEdit>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent), drawing(false), mouseInside(false),
      m_currentTool(Tool::Pen), m_scrollMode(ScrollMode::History),
      m_currentPenWidth(1), currentColor(255, 255, 255, 128),
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
    m_indicatorTimer->setInterval(1000); // 1 second
    connect(m_indicatorTimer, &QTimer::timeout, this, &Canvas::hideModeIndicator);
}

Canvas::~Canvas() {}

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
        font.setPointSize(m_currentPenWidth + 10);
        QFontMetrics fm(font);
        
        // Calculate the bounding box of the text
        QRect textRect = fm.boundingRect(text);
        
        // Calculate the top-left position to make the text's center align with centerPos
        QPoint topLeftPos = centerPos - textRect.center();

        undonePaths.clear();
        paths.append({ {topLeftPos}, currentColor, m_currentPenWidth, Tool::Text, text });
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
            font.setPointSize(m_currentPenWidth + 10);
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
            if (m_currentTool == Tool::Line || m_currentTool == Tool::Rectangle || m_currentTool == Tool::Circle) {
                currentPath.append(event->position().toPoint());
            }
            update();
        }
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
        cycleScrollMode();
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
    } else if (event->button() == Qt::MiddleButton) {
        emit middleButtonDoubleClicked();
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
                        font.setPointSize(pathData.penWidth + 10); // Use penWidth to control font size
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

    int h, s, v;
    
    switch (m_scrollMode) {
        case ScrollMode::Hue:
        case ScrollMode::Saturation:
        case ScrollMode::Brightness:
            currentColor.getHsv(&h, &s, &v);
            if (m_scrollMode == ScrollMode::Hue) {
                h = (h + (delta > 0 ? -5 : 5) + 360) % 360;
            } else if (m_scrollMode == ScrollMode::Saturation) {
                s = std::clamp(s + (delta > 0 ? -5 : 5), 0, 255);
            } else { // Brightness
                v = std::clamp(v + (delta > 0 ? -5 : 5), 0, 255);
            }
            currentColor.setHsv(h, s, v);
            emit penColorChanged(currentColor);
            showIndicator();
            break;
        case ScrollMode::BrushSize:
            m_currentPenWidth = std::max(1, m_currentPenWidth + (delta > 0 ? -1 : 1));
            showIndicator(QString("%1px").arg(m_currentPenWidth));
            break;
        case ScrollMode::History:
            (delta > 0) ? undo() : redo();
            return; 
        case ScrollMode::ToolSwitch:
            int currentToolIndex = static_cast<int>(m_currentTool);
            int nextToolIndex = (currentToolIndex + (delta > 0 ? -1 : 1) + 6) % 6;
            setTool(static_cast<Tool>(nextToolIndex));
            return;
    }
    update();
}

void Canvas::cycleScrollMode()
{
    m_scrollMode = static_cast<ScrollMode>((static_cast<int>(m_scrollMode) + 1) % 6);
    showIndicator();
}

void Canvas::showIndicator(const QString &subText)
{
    if (m_scrollMode == ScrollMode::Hue || m_scrollMode == ScrollMode::Saturation || m_scrollMode == ScrollMode::Brightness) {
        int h, s, v;
        currentColor.getHsv(&h, &s, &v);
        m_indicatorSubText = QString::asprintf("H:%.2f S:%.2f B:%.2f", h / 359.0, s / 255.0, v / 255.0);
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
        case Tool::Rectangle: return "rectangle";
        case Tool::Circle:    return "circle";
    }
    return "";
}
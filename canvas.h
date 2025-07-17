#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QVector>
#include <QColor>
#include <QLineEdit>
#include <QTimer>
#include <algorithm> // For std::clamp
#include <cmath> // For std::atan2, std::cos, std::sin

namespace Constants {
    constexpr int INDICATOR_TIMEOUT_MS = 1000;
    constexpr int HUE_SENSITIVITY = 5;
    constexpr int SATURATION_SENSITIVITY = 5;
    constexpr int BRIGHTNESS_SENSITIVITY = 5;
    constexpr int OPACITY_SENSITIVITY = 5;
    constexpr int SIZE_SENSITIVITY = 1;
}

// Define Tool enum accessible by other classes
enum class Tool {
    Pen,
    Eraser,
    Text,
    Line,
    Arrow,
    Rectangle,
    Circle
};

// Define the modes for the scroll wheel in the desired order
enum class ScrollMode {
    History,
    Hue,
    Saturation,
    Brightness,
    Opacity,
    BrushSize,
    ToolSwitch
};

// Struct to hold both the points of a path, its color, its width, and the tool used
struct PathData {
    QVector<QPoint> points;
    QColor color;
    int penWidth;
    Tool tool;
    QString text;
    int textSize; // For text tool
};

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();

signals:
    void leftButtonDoubleClicked();
    void rightButtonClicked();
    void rightButtonDoubleClicked();

public: // Add getters for state saving
    ScrollMode getScrollMode() const { return m_scrollMode; }
    Tool getTool() const { return m_currentTool; }
    int getPenWidth() const { return m_currentPenWidth; }
    int getTextSize() const { return m_currentTextSize; }
    QColor getColor() const { return currentColor; }

    // String conversion helpers for settings
    QString toolToString(Tool tool) const;
    QString scrollModeToString() const;
    static Tool toolFromString(const QString &s);
    static ScrollMode scrollModeFromString(const QString &s);

public slots:
    void beginInitialization() { m_isInitializing = true; }
    void endInitialization() { m_isInitializing = false; }
    void setScrollMode(ScrollMode mode) { m_scrollMode = mode; }
    void setInitialPenWidth(int width);
    void setInitialTextSize(int size);
    void setPenColor(const QColor &color);
    void setTool(Tool newTool);
    void undo();
    void redo();
    void clearCanvas();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void handleTextEditingFinished();
    void onRightClickTimeout();
    void hideModeIndicator();

private:
    void cycleScrollMode();
    void cycleScrollModeBackward();
    void showIndicator(const QString &subText = "");

    bool m_isInitializing;
    bool drawing;
    bool mouseInside;
    bool isMiddleButtonPressed;
    bool m_ignoreNextRightRelease;
    Tool m_currentTool;
    ScrollMode m_scrollMode;
    int m_currentPenWidth;
    int m_currentTextSize;
    QPoint cursorPos;
    QColor currentColor;
    QVector<QPoint> currentPath;
    QVector<PathData> paths;
    QVector<PathData> undonePaths;
    
    QLineEdit *m_textInput;
    QPoint m_textClickPos;
    QTimer *m_rightClickTimer;
    QTimer *m_indicatorTimer;
    bool m_showIndicator;
    QString m_indicatorSubText;
};

#endif // CANVAS_H

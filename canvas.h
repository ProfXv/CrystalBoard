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
#include <QTimer>
#include <algorithm> // For std::clamp
#include <cmath> // For std::atan2, std::cos, std::sin

// Define Tool enum accessible by other classes
enum class Tool {
    Pen,
    Eraser,
    Line,
    Rectangle,
    Circle
};

// Define the modes for the scroll wheel in the desired order
enum class ScrollMode {
    History,
    Hue,
    Saturation,
    Brightness,
    BrushSize,
    ToolSwitch
};

// Struct to hold both the points of a path, its color, its width, and the tool used
struct PathData {
    QVector<QPoint> points;
    QColor color;
    int penWidth;
    Tool tool;
};

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();

signals:
    void penColorChanged(const QColor &color);
    void rightButtonClicked();
    void rightButtonDoubleClicked();
    void middleButtonDoubleClicked();

public slots:
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

private slots:
    void onRightClickTimeout();
    void hideModeIndicator();

private:
    void cycleScrollMode();
    QString scrollModeToString() const;
    QString toolToString(Tool tool) const;
    void showIndicator(const QString &subText = "");

    bool drawing;
    bool mouseInside;
    Tool m_currentTool;
    ScrollMode m_scrollMode;
    int m_currentPenWidth;
    QPoint cursorPos;
    QColor currentColor;
    QVector<QPoint> currentPath;
    QVector<PathData> paths;
    QVector<PathData> undonePaths;
    
    QTimer *m_rightClickTimer;
    QTimer *m_indicatorTimer;
    bool m_showIndicator;
    QString m_indicatorSubText;
};

#endif // CANVAS_H

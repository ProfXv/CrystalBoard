#ifndef TRANSPARENTWIDGET_H
#define TRANSPARENTWIDGET_H

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

// Struct to hold both the points of a path, its color, its width, and the tool used
struct PathData {
    QVector<QPoint> points;
    QColor color;
    int penWidth;
    Tool tool;
};

class TransparentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransparentWidget(QWidget *parent = nullptr);
    ~TransparentWidget();

signals:
    void rightButtonClicked();
    void rightButtonDoubleClicked();
    void middleButtonClicked();

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

private:
    bool drawing;
    bool mouseInside;
    bool m_isAdjustingBrushSize;
    bool m_brushWasAdjusted;
    Tool m_currentTool;
    int m_currentPenWidth;
    QPoint cursorPos;
    QColor currentColor;
    QVector<QPoint> currentPath;
    QVector<PathData> paths;
    QVector<PathData> undonePaths;
    QTimer *m_rightClickTimer;
};

#endif // TRANSPARENTWIDGET_H

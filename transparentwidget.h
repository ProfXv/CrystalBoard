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

// Struct to hold both the points of a path, its color, and its width
struct PathData {
    QVector<QPoint> points;
    QColor color;
    int penWidth;
};

class TransparentWidget : public QWidget
{
    Q_OBJECT

public:
    TransparentWidget(QWidget *parent = nullptr);
    ~TransparentWidget();

signals:
    void rightButtonClicked();
    void rightButtonDoubleClicked();

public slots:
    void setPenColor(const QColor &color);
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
    bool m_isErasing;
    int m_currentPenWidth;
    QPoint cursorPos;
    QColor currentColor;
    QVector<QPoint> currentPath;
    QVector<PathData> paths;
    QVector<PathData> undonePaths;
    QTimer *m_rightClickTimer;
};

#endif // TRANSPARENTWIDGET_H

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

// Struct to hold both the points of a path and its color
struct PathData {
    QVector<QPoint> points;
    QColor color;
};

class TransparentWidget : public QWidget
{
    Q_OBJECT

public:
    TransparentWidget(QWidget *parent = nullptr);
    ~TransparentWidget();

public slots:
    void setPenColor(const QColor &color);
    void undo();
    void redo();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    bool drawing;
    bool mouseInside;
    QPoint cursorPos;
    QColor currentColor;
    QVector<QPoint> currentPath;
    QVector<PathData> paths;
    QVector<PathData> undonePaths;
};

#endif // TRANSPARENTWIDGET_H

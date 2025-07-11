#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QColor>
#include <QPaintEvent>
#include "transparentwidget.h" // Include for the Tool enum

class ColorPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPickerWidget(QWidget *parent = nullptr);
    QColor getCurrentColor() const;

public slots:
    void onPenColorChanged(const QColor &color);

signals:
    void colorChanged(const QColor &color);
    void toolSelected(Tool tool);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *helpLabel;
    QColor currentColor;
};

#endif // COLORPICKERWIDGET_H

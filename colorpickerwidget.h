#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QColor>
#include <QPaintEvent>

class ColorPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPickerWidget(QWidget *parent = nullptr);
    QColor getCurrentColor() const;

signals:
    void colorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateColor();
    void incrementRed();
    void decrementRed();
    void incrementGreen();
    void decrementGreen();
    void incrementBlue();
    void decrementBlue();

private:
    void setupSlider(QSlider *slider);
    void updateRgbLabel();

    QSlider *redSlider;
    QSlider *greenSlider;
    QSlider *blueSlider;

    QPushButton *redMinusButton;
    QPushButton *redPlusButton;
    QPushButton *greenMinusButton;
    QPushButton *greenPlusButton;
    QPushButton *blueMinusButton;
    QPushButton *bluePlusButton;
    
    QLabel *rgbLabel;
    QColor currentColor;
};

#endif // COLORPICKERWIDGET_H

#include "colorpickerwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>
#include <QButtonGroup>

ColorPickerWidget::ColorPickerWidget(QWidget *parent) : QWidget(parent), currentColor(255, 255, 255, 128)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setAutoFillBackground(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(20);

    mainLayout->addStretch(1);

    // --- Help Panel ---
    helpLabel = new QLabel(this);
    const QString helpTextStyle =
        "background-color: black;"
        "color: white;"
        "padding: 15px;"
        "font-size: 22px;"
        "border-radius: 10px;";
    const QString helpText =
        "<div align='center'>"
        "<h1>🔮 CrystalBoard 🎨</h1>"
        "<p><i>A crystal-clear, transparent canvas that floats above your desktop.</i></p>"
        "</div>"
        "<div align='center'>"
        "<table style='width:100%; margin-top: 15px;'>"
        "  <thead>"
        "    <tr>"
        "      <th style='padding: 10px; text-align: center;'></th>"
        "      <th style='padding: 10px; text-align: center;'>Left Button</th>"
        "      <th style='padding: 10px; text-align: center;'>Middle Button</th>"
        "      <th style='padding: 10px; text-align: center;'>Right Button</th>"
        "    </tr>"
        "  </thead>"
        "  <tbody>"
        "    <tr>"
        "      <th style='padding: 10px; text-align: right;'>Click</th>"
        "      <td style='padding: 10px; text-align: center;'><u>Drag</u> to draw</td>"
        "      <td style='padding: 10px; text-align: center;'>cycle scroll mode</td>"
        "      <td style='padding: 10px; text-align: center;'>clear canvas</td>"
        "    </tr>"
        "    <tr>"
        "      <th style='padding: 10px; text-align: right;'>Double-click</th>"
        "      <td style='padding: 10px; text-align: center;'></td>"
        "      <td style='padding: 10px; text-align: center;'>exit app</td>"
        "      <td style='padding: 10px; text-align: center;'>toggle view</td>"
        "    </tr>"
        "  </tbody>"
        "</table>"
        "<p style='text-align: center; margin-top: 15px;'><b>Scroll Wheel:</b> use active mode</p>"
        "</div>";
    helpLabel->setText(helpText);
    helpLabel->setStyleSheet(helpTextStyle);
    helpLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(helpLabel);

    mainLayout->addStretch(1);

    setLayout(mainLayout);
    onPenColorChanged(QColor(0, 255, 255, 128)); // Set initial color (cyan)
}

void ColorPickerWidget::onPenColorChanged(const QColor &color)
{
    currentColor = color;
    update(); // Trigger paintEvent to update background
    emit colorChanged(currentColor);
}

void ColorPickerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
}

QColor ColorPickerWidget::getCurrentColor() const
{
    return currentColor;
}

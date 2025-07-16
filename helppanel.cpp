#include "helppanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>
#include <QButtonGroup>

HelpPanel::HelpPanel(QWidget *parent) : QWidget(parent), currentColor(255, 255, 255, 128)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);

    // Create a horizontal layout to center the vertical layout
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addStretch(1);

    // Vertical layout for the help label itself
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0); // No margins for the inner layout
    vLayout->setSpacing(20);
    vLayout->addStretch(1);

    // --- Help Panel ---
    helpLabel = new QLabel(this);
    const QString helpTextStyle =
        "background-color: black;"
        "color: white;"
        "padding: 25px;" // Increased padding for better aesthetics
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
        "      <th style='padding: 10px; text-align: center;'>Right Button</th>"
        "    </tr>"
        "  </thead>"
        "  <tbody>"
        "    <tr>"
        "      <th style='padding: 10px; text-align: right;'>Single-click</th>"
        "      <td style='padding: 10px; text-align: center;'><u>Drag</u> to draw</td>"
        "      <td style='padding: 10px; text-align: center;'>clear canvas</td>"
        "    </tr>"
        "    <tr>"
        "      <th style='padding: 10px; text-align: right;'>Double-click</th>"
        "      <td style='padding: 10px; text-align: center;'>toggle view</td>"
        "      <td style='padding: 10px; text-align: center;'>exit app</td>"
        "    </tr>"
        "  </tbody>"
        "</table>"
        "<p style='text-align: center; margin-top: 15px; margin-bottom: 0; padding: 0;'><b>Scroll:</b> use active mode</p>"
        "<p style='text-align: center; margin-top: 5px; margin-bottom: 0; padding: 0;'><b>Scroll when <u>Hold</u>:</b> cycle mode</p>"
        "</div>";
    helpLabel->setText(helpText);
    helpLabel->setStyleSheet(helpTextStyle);
    helpLabel->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(helpLabel);

    vLayout->addStretch(1);

    // Add the vertical layout to the horizontal layout
    hLayout->addLayout(vLayout);
    hLayout->addStretch(1);

    setLayout(hLayout);
    onPenColorChanged(QColor(0, 255, 255, 128)); // Set initial color (cyan)
}

void HelpPanel::onPenColorChanged(const QColor &color)
{
    currentColor = color;
    // No need to call update() anymore as the background is transparent
    emit colorChanged(currentColor);
}

void HelpPanel::paintEvent(QPaintEvent *event)
{
    // This is now intentionally left empty to keep the widget background transparent.
    // The helpLabel's stylesheet handles its own black background.
    Q_UNUSED(event);
}

QColor HelpPanel::getCurrentColor() const
{
    return currentColor;
}

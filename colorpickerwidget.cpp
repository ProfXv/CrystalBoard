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

    // --- Help Panel ---
    helpLabel = new QLabel(this);
    const QString helpTextStyle =
        "background-color: rgba(0, 0, 0, 0.5);"
        "color: white;"
        "padding: 15px;"
        "font-size: 16px;"
        "border-radius: 10px;";
    const QString helpText =
        "<b>Left-click + Drag:</b> Draw<br>"
        "<b>Double-click Left:</b> Cycle Tools<br>"
        "<b>Right-click:</b> Clear Canvas<br>"
        "<b>Double-click Right:</b> Toggle This View<br>"
        "<b>Middle-click + Scroll:</b> Adjust Size<br>"
        "<b>Scroll Wheel:</b> Undo/Redo<br>"
        "<b>Esc Key:</b> Exit";
    helpLabel->setText(helpText);
    helpLabel->setStyleSheet(helpTextStyle);
    helpLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(helpLabel);
    
    mainLayout->addStretch(1);

    const QString buttonStyle =
        "QPushButton {"
        "   background-color: #888;"
        "   color: white;"
        "   border: 1px solid #555;"
        "   min-width: 40px;"
        "   max-width: 40px;"
        "   min-height: 40px;"
        "   max-height: 40px;"
        "   border-radius: 20px;" // half of width/height
        "   font-size: 20px;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #999;"
        "}";

    // Red Slider
    QHBoxLayout *redLayout = new QHBoxLayout();
    redMinusButton = new QPushButton("-");
    redSlider = new QSlider(Qt::Horizontal);
    redPlusButton = new QPushButton("+");
    setupSlider(redSlider);
    redSlider->setValue(255);
    redMinusButton->setStyleSheet(buttonStyle);
    redPlusButton->setStyleSheet(buttonStyle);
    redLayout->addWidget(redMinusButton);
    redLayout->addWidget(redSlider);
    redLayout->addWidget(redPlusButton);
    mainLayout->addLayout(redLayout);

    // Green Slider
    QHBoxLayout *greenLayout = new QHBoxLayout();
    greenMinusButton = new QPushButton("-");
    greenSlider = new QSlider(Qt::Horizontal);
    greenPlusButton = new QPushButton("+");
    setupSlider(greenSlider);
    greenSlider->setValue(255);
    greenMinusButton->setStyleSheet(buttonStyle);
    greenPlusButton->setStyleSheet(buttonStyle);
    greenLayout->addWidget(greenMinusButton);
    greenLayout->addWidget(greenSlider);
    greenLayout->addWidget(greenPlusButton);
    mainLayout->addLayout(greenLayout);

    // Blue Slider
    QHBoxLayout *blueLayout = new QHBoxLayout();
    blueMinusButton = new QPushButton("-");
    blueSlider = new QSlider(Qt::Horizontal);
    bluePlusButton = new QPushButton("+");
    setupSlider(blueSlider);
    blueSlider->setValue(255);
    blueMinusButton->setStyleSheet(buttonStyle);
    bluePlusButton->setStyleSheet(buttonStyle);
    blueLayout->addWidget(blueMinusButton);
    blueLayout->addWidget(blueSlider);
    blueLayout->addWidget(bluePlusButton);
    mainLayout->addLayout(blueLayout);
    
    mainLayout->addStretch(1);

    // --- Tool Selection Buttons ---
    QHBoxLayout *toolLayout = new QHBoxLayout();
    toolLayout->setSpacing(10);
    
    penButton = new QPushButton("Pen");
    eraserButton = new QPushButton("Eraser");
    lineButton = new QPushButton("Line");
    rectButton = new QPushButton("Rect");
    circleButton = new QPushButton("Circle");

    const QString toolButtonStyle =
        "QPushButton {"
        "   background-color: #666;"
        "   color: white;"
        "   border: 1px solid #444;"
        "   padding: 10px;"
        "   font-size: 16px;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:checked {"
        "   background-color: #888;"
        "   border: 1px solid #999;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #777;"
        "}";

    QPushButton* buttons[] = {penButton, eraserButton, lineButton, rectButton, circleButton};
    for(auto* button : buttons) {
        button->setStyleSheet(toolButtonStyle);
        button->setCheckable(true);
    }
    penButton->setChecked(true); // Pen is the default tool

    QButtonGroup *toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);
    for(auto* button : buttons) {
        toolGroup->addButton(button);
        toolLayout->addWidget(button);
    }
    mainLayout->addLayout(toolLayout);

    // --- RGB Label ---
    rgbLabel = new QLabel(this);
    rgbLabel->setAlignment(Qt::AlignCenter);
    rgbLabel->setStyleSheet(
        "background-color: white;"
        "color: black;"
        "padding: 10px;"
        "font-size: 18px;"
        "border-radius: 5px;"
    );
    mainLayout->addWidget(rgbLabel, 0, Qt::AlignBottom);

    // --- Connections ---
    connect(redSlider, &QSlider::valueChanged, this, &ColorPickerWidget::updateColor);
    connect(greenSlider, &QSlider::valueChanged, this, &ColorPickerWidget::updateColor);
    connect(blueSlider, &QSlider::valueChanged, this, &ColorPickerWidget::updateColor);

    connect(redMinusButton, &QPushButton::clicked, this, &ColorPickerWidget::decrementRed);
    connect(redPlusButton, &QPushButton::clicked, this, &ColorPickerWidget::incrementRed);
    connect(greenMinusButton, &QPushButton::clicked, this, &ColorPickerWidget::decrementGreen);
    connect(greenPlusButton, &QPushButton::clicked, this, &ColorPickerWidget::incrementGreen);
    connect(blueMinusButton, &QPushButton::clicked, this, &ColorPickerWidget::decrementBlue);
    connect(bluePlusButton, &QPushButton::clicked, this, &ColorPickerWidget::incrementBlue);

    // --- Tool Connections ---
    connect(penButton, &QPushButton::clicked, this, [this](){ emit toolSelected(Tool::Pen); });
    connect(eraserButton, &QPushButton::clicked, this, [this](){ emit toolSelected(Tool::Eraser); });
    connect(lineButton, &QPushButton::clicked, this, [this](){ emit toolSelected(Tool::Line); });
    connect(rectButton, &QPushButton::clicked, this, [this](){ emit toolSelected(Tool::Rectangle); });
    connect(circleButton, &QPushButton::clicked, this, [this](){ emit toolSelected(Tool::Circle); });

    setLayout(mainLayout);
    updateColor();
}

void ColorPickerWidget::setupSlider(QSlider *slider)
{
    slider->setRange(0, 255);
}

void ColorPickerWidget::updateColor()
{
    int r = redSlider->value();
    int g = greenSlider->value();
    int b = blueSlider->value();
    currentColor.setRgb(r, g, b, 128);

    const QString baseStyle =
        "QSlider { min-height: 40px; }"
        "QSlider::handle:horizontal {"
        "    background: #888;"
        "    border: 1px solid #555;"
        "    width: 40px;"
        "    height: 40px;"
        "    margin: -15px 0;"
        "    border-radius: 20px;"
        "}";

    QString redGrooveColor = QString("rgb(%1, 0, 0)").arg(r);
    QString greenGrooveColor = QString("rgb(0, %1, 0)").arg(g);
    QString blueGrooveColor = QString("rgb(0, 0, %1)").arg(b);

    redSlider->setStyleSheet(QString("QSlider::groove:horizontal { height: 10px; border-radius: 5px; background-color: %1; } %2").arg(redGrooveColor, baseStyle));
    greenSlider->setStyleSheet(QString("QSlider::groove:horizontal { height: 10px; border-radius: 5px; background-color: %1; } %2").arg(greenGrooveColor, baseStyle));
    blueSlider->setStyleSheet(QString("QSlider::groove:horizontal { height: 10px; border-radius: 5px; background-color: %1; } %2").arg(blueGrooveColor, baseStyle));

    updateRgbLabel();
    update(); // Trigger paintEvent to update background
    emit colorChanged(currentColor);
}

void ColorPickerWidget::updateRgbLabel()
{
    rgbLabel->setText(QString("RGB: (%1, %2, %3)").arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue()));
}

void ColorPickerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    // Use the opaque version of the color for the background
    QColor backgroundColor = currentColor;
    backgroundColor.setAlpha(255);
    painter.fillRect(rect(), backgroundColor);
}

QColor ColorPickerWidget::getCurrentColor() const
{
    return currentColor;
}

void ColorPickerWidget::incrementRed() { redSlider->setValue(qMin(255, redSlider->value() + 1)); }
void ColorPickerWidget::decrementRed() { redSlider->setValue(qMax(0, redSlider->value() - 1)); }
void ColorPickerWidget::incrementGreen() { greenSlider->setValue(qMin(255, greenSlider->value() + 1)); }
void ColorPickerWidget::decrementGreen() { greenSlider->setValue(qMax(0, greenSlider->value() - 1)); }
void ColorPickerWidget::incrementBlue() { blueSlider->setValue(qMin(255, blueSlider->value() + 1)); }
void ColorPickerWidget::decrementBlue() { blueSlider->setValue(qMax(0, blueSlider->value() - 1)); }

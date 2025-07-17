#include "mainwindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QEvent>
#include <QSettings>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_isLeftButtonPressed(false), m_isRightButtonPressed(false)
{
    // Make the main window transparent and frameless
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    
    // Use setFixedSize, which is the key to floating behavior on some WMs
    setFixedSize(QGuiApplication::primaryScreen()->size());

    // Setup the stacked widget to switch between views
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Create the widgets
    canvas = new Canvas(this);
    helpPanel = new HelpPanel(this);

    // Add them to the stack
    stackedWidget->addWidget(canvas);
    stackedWidget->addWidget(helpPanel);

    // Install event filter on both widgets to catch global mouse events
    canvas->installEventFilter(this);
    helpPanel->installEventFilter(this);

    // --- Connect Signals and Slots ---

    // 1. Right-click on drawing widget clears the canvas
    connect(canvas, &Canvas::rightButtonClicked,
            canvas, &Canvas::clearCanvas);

    // 2. Left-double-click on drawing widget toggles the help panel view
    connect(canvas, &Canvas::leftButtonDoubleClicked,
            this, &MainWindow::toggleHelpPanel);

    // 3. Right-double-click on drawing widget resets the settings
    connect(canvas, &Canvas::rightButtonDoubleClicked,
            this, &MainWindow::resetSettings);

    // --- Load Settings or Set Defaults ---
    loadSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    QSettings settings;

    // Check for a complete set of keys. If any are missing, apply all defaults.
    bool settingsAreComplete = settings.contains("Tool/current") &&
                               settings.contains("Size/general") &&
                               settings.contains("Size/text") &&
                               settings.contains("Color/hue") &&
                               settings.contains("Color/saturation") &&
                               settings.contains("Color/value") &&
                               settings.contains("Color/opacity");

    if (!settingsAreComplete) {
        applyDefaultSettings();
        return;
    }

    canvas->beginInitialization();

    // --- Load All Settings from File ---
    int generalSize = settings.value("Size/general").toInt();
    int textSize = settings.value("Size/text").toInt();
    canvas->setInitialPenWidth(generalSize);
    canvas->setInitialTextSize(textSize);

    QColor color;
    color.setHsv(
        settings.value("Color/hue").toInt(),
        settings.value("Color/saturation").toInt(),
        settings.value("Color/value").toInt(),
        settings.value("Color/opacity").toInt()
    );
    canvas->setPenColor(color);

    QString toolName = settings.value("Tool/current").toString();
    canvas->setTool(Canvas::toolFromString(toolName));

    QString modeName = settings.value("Mode/current").toString();
    canvas->setScrollMode(Canvas::scrollModeFromString(modeName));
    
    canvas->endInitialization();
}

void MainWindow::resetSettings()
{
    QSettings settings;
    settings.clear(); // Clear the entire configuration file
    applyDefaultSettings();
}

void MainWindow::applyDefaultSettings()
{
    canvas->beginInitialization();

    // --- Apply Dynamic Sizing Logic ---
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenHeight = screenGeometry.height();
    int initialPenWidth = std::max(1, static_cast<int>(screenHeight * 0.005));
    int initialTextSize = std::max(12, static_cast<int>(screenHeight * 0.025));
    canvas->setInitialPenWidth(initialPenWidth);
    canvas->setInitialTextSize(initialTextSize);

    // --- Apply Default Color ---
    QColor color = QColor(255, 255, 255, 255);
    canvas->setPenColor(color);

    // --- Apply Default Tool and Mode ---
    canvas->setTool(Tool::Pen);
    canvas->setScrollMode(ScrollMode::History); // Set default mode to History

    canvas->endInitialization();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    // --- Save Color ---
    settings.beginGroup("Color");
    QColor color = canvas->getColor();
    settings.setValue("hue", color.hue());
    settings.setValue("saturation", color.saturation());
    settings.setValue("value", color.value());
    settings.setValue("opacity", color.alpha());
    settings.endGroup();

    // --- Save Size ---
    settings.beginGroup("Size");
    settings.setValue("general", canvas->getPenWidth());
    settings.setValue("text", canvas->getTextSize());
    settings.endGroup();

    // --- Save Tool and Mode ---
    settings.setValue("Tool/current", canvas->toolToString(canvas->getTool()));
    settings.setValue("Mode/current", canvas->scrollModeToString());
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Handle the Escape key exit
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::toggleHelpPanel()
{
    int currentIndex = stackedWidget->currentIndex();
    int nextIndex = (currentIndex + 1) % stackedWidget->count();
    stackedWidget->setCurrentIndex(nextIndex);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // This filter now handles events from both the canvas and the help panel
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_isLeftButtonPressed = true;
        } else if (mouseEvent->button() == Qt::RightButton) {
            m_isRightButtonPressed = true;
        }
        if (m_isLeftButtonPressed && m_isRightButtonPressed) {
            close();
            return true; // Event handled
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_isLeftButtonPressed = false;
        } else if (mouseEvent->button() == Qt::RightButton) {
            m_isRightButtonPressed = false;
        }
    }

    // Global event filter to handle events on the help panel
    if (obj == helpPanel && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            toggleHelpPanel();
            return true; // Event is handled
        }
        if (mouseEvent->button() == Qt::RightButton) {
            resetSettings(); // Also allow reset from help panel
            return true; // Event is handled
        }
    }
    // Standard event processing for other events
    return QMainWindow::eventFilter(obj, event);
}

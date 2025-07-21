#include "mainwindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QEvent>
#include <QSettings>
#include <QCloseEvent>
#include <QVariantMap>

MainWindow::MainWindow(const QVariantMap &cmdLineOptions, QWidget *parent)
    : QMainWindow(parent),
      m_cmdLineOptions(cmdLineOptions),
      m_isLeftButtonPressed(false),
      m_isRightButtonPressed(false)
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
    connect(canvas, &Canvas::rightButtonClicked, canvas, &Canvas::clearCanvas);
    connect(canvas, &Canvas::leftButtonDoubleClicked, this, &MainWindow::toggleHelpPanel);
    connect(canvas, &Canvas::rightButtonDoubleClicked, this, &MainWindow::resetSettings);

    // --- Load Settings or Set Defaults ---
    if (m_cmdLineOptions.contains("reset")) {
        QSettings settings;
        settings.clear();
    }
    loadSettings();

    // --- Set Initial View ---
    if (m_cmdLineOptions.contains("clean")) {
        stackedWidget->setCurrentWidget(canvas);
    } else {
        stackedWidget->setCurrentWidget(helpPanel);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_cmdLineOptions.contains("never-save")) {
        saveSettings();
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    canvas->beginInitialization();

    // --- Priority: Command Line > Settings File > Defaults ---

    // Get default values first
    applyDefaultSettings();

    // Load from settings file if it exists and is complete
    bool settingsAreComplete = settings.contains("Tool/current") && settings.contains("Size/general");
    if (settingsAreComplete) {
        canvas->setInitialPenWidth(settings.value("Size/general").toInt());
        canvas->setInitialTextSize(settings.value("Size/text").toInt());
        QColor color;
        color.setHsv(
            settings.value("Color/hue").toInt(),
            settings.value("Color/saturation").toInt(),
            settings.value("Color/value").toInt(),
            settings.value("Color/opacity").toInt()
        );
        canvas->setPenColor(color);
        canvas->setTool(Canvas::toolFromString(settings.value("Tool/current").toString()));
        canvas->setScrollMode(Canvas::scrollModeFromString(settings.value("Mode/current").toString()));
    }

    // Override with command line options if they exist
    if (m_cmdLineOptions.contains("size")) canvas->setInitialPenWidth(m_cmdLineOptions["size"].toInt());
    if (m_cmdLineOptions.contains("text-size")) canvas->setInitialTextSize(m_cmdLineOptions["text-size"].toInt());
    if (m_cmdLineOptions.contains("tool")) canvas->setTool(Canvas::toolFromString(m_cmdLineOptions["tool"].toString()));
    if (m_cmdLineOptions.contains("mode")) canvas->setScrollMode(Canvas::scrollModeFromString(m_cmdLineOptions["mode"].toString()));

    QColor finalColor = canvas->getColor();
    if (m_cmdLineOptions.contains("hue")) finalColor.setHsv(m_cmdLineOptions["hue"].toInt(), finalColor.saturation(), finalColor.value(), finalColor.alpha());
    if (m_cmdLineOptions.contains("saturation")) finalColor.setHsv(finalColor.hue(), m_cmdLineOptions["saturation"].toInt(), finalColor.value(), finalColor.alpha());
    if (m_cmdLineOptions.contains("value")) finalColor.setHsv(finalColor.hue(), finalColor.saturation(), m_cmdLineOptions["value"].toInt(), finalColor.alpha());
    if (m_cmdLineOptions.contains("opacity")) finalColor.setAlpha(m_cmdLineOptions["opacity"].toInt());
    canvas->setPenColor(finalColor);

    canvas->endInitialization();
}

void MainWindow::resetSettings()
{
    QSettings settings;
    settings.clear();
    applyDefaultSettings();
}

void MainWindow::applyDefaultSettings()
{
    // This function now only sets the base default values
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenHeight = screenGeometry.height();
    int initialPenWidth = std::max(1, static_cast<int>(screenHeight * 0.005));
    int initialTextSize = std::max(12, static_cast<int>(screenHeight * 0.025));
    canvas->setInitialPenWidth(initialPenWidth);
    canvas->setInitialTextSize(initialTextSize);

    QColor color = QColor(255, 255, 255, 255);
    canvas->setPenColor(color);

    canvas->setTool(Tool::Pen);
    canvas->setScrollMode(ScrollMode::History);
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Color");
    QColor color = canvas->getColor();
    settings.setValue("hue", color.hue());
    settings.setValue("saturation", color.saturation());
    settings.setValue("value", color.value());
    settings.setValue("opacity", color.alpha());
    settings.endGroup();

    settings.beginGroup("Size");
    settings.setValue("general", canvas->getPenWidth());
    settings.setValue("text", canvas->getTextSize());
    settings.endGroup();

    settings.setValue("Tool/current", canvas->toolToString(canvas->getTool()));
    settings.setValue("Mode/current", canvas->scrollModeToString());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
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
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_isLeftButtonPressed = true;
        } else if (mouseEvent->button() == Qt::RightButton) {
            m_isRightButtonPressed = true;
        }
        if (m_isLeftButtonPressed && m_isRightButtonPressed) {
            close();
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_isLeftButtonPressed = false;
        } else if (mouseEvent->button() == Qt::RightButton) {
            m_isRightButtonPressed = false;
        }
    }

    if (obj == helpPanel && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            toggleHelpPanel();
            return true;
        }
        if (mouseEvent->button() == Qt::RightButton) {
            resetSettings();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
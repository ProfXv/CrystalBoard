#include "mainwindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Make the main window transparent and frameless
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    
    // Setup the stacked widget to switch between views
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Create the widgets
    canvas = new Canvas(this);
    helpPanel = new HelpPanel(this);

    // Add them to the stack
    stackedWidget->addWidget(canvas);
    stackedWidget->addWidget(helpPanel);

    // Install event filter on the color picker to catch global mouse events
    helpPanel->installEventFilter(this);

    // --- Connect Signals and Slots ---

    // 1. When color is changed in picker, update drawing widget's pen
    connect(helpPanel, &HelpPanel::colorChanged,
            canvas, &Canvas::setPenColor);

    // 1b. When color is changed by wheel, update picker's display
    connect(canvas, &Canvas::penColorChanged,
            helpPanel, &HelpPanel::onPenColorChanged);
            
    // 2. Right-click on drawing widget clears the canvas
    connect(canvas, &Canvas::rightButtonClicked,
            canvas, &Canvas::clearCanvas);

    // 3. Left-double-click on drawing widget toggles the help panel view
    connect(canvas, &Canvas::leftButtonDoubleClicked,
            this, &MainWindow::toggleHelpPanel);

    // 4. Right-double-click on drawing widget closes the application
    connect(canvas, &Canvas::rightButtonDoubleClicked,
            this, &MainWindow::close);
            
    // 5. When a tool is selected in picker, update drawing widget's tool
    connect(helpPanel, &HelpPanel::toolSelected,
            canvas, &Canvas::setTool);

    // --- Dynamic Initial Sizing & Color ---
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenHeight = screenGeometry.height();

    // Set initial sizes based on screen height
    int initialPenWidth = std::max(1, static_cast<int>(screenHeight * 0.005));
    int initialTextSize = std::max(12, static_cast<int>(screenHeight * 0.025));
    canvas->setInitialPenWidth(initialPenWidth);
    canvas->setInitialTextSize(initialTextSize);

    // Set initial color to white (opaque)
    QColor initialColor(255, 255, 255, 255);
    canvas->setPenColor(initialColor);
    helpPanel->onPenColorChanged(initialColor);
}

MainWindow::~MainWindow()
{
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
    // Global event filter to handle events on the help panel
    if (obj == helpPanel && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            toggleHelpPanel();
            return true; // Event is handled
        }
        if (mouseEvent->button() == Qt::RightButton) {
            close();
            return true; // Event is handled
        }
    }
    // Standard event processing for other events
    return QMainWindow::eventFilter(obj, event);
}

#include "mainwindow.h"
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

    // 3. Middle-double-click on drawing widget toggles the color picker view
    connect(canvas, &Canvas::middleButtonDoubleClicked,
            this, &MainWindow::toggleHelpPanel);

    // 4. Right-double-click on drawing widget closes the application
    connect(canvas, &Canvas::rightButtonDoubleClicked,
            this, &MainWindow::close);
            
    // 5. When a tool is selected in picker, update drawing widget's tool
    connect(helpPanel, &HelpPanel::toolSelected,
            canvas, &Canvas::setTool);

    // Set initial color for both widgets
    QColor initialColor;
    initialColor.setHsv(0, 255, 255); // Start with Red
    initialColor.setAlpha(128);
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
    // Global event filter to handle closing from the color picker view
    if (obj == helpPanel && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::MiddleButton) {
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

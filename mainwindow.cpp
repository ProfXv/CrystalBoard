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
    drawingWidget = new TransparentWidget(this);
    colorPickerWidget = new ColorPickerWidget(this);

    // Add them to the stack
    stackedWidget->addWidget(drawingWidget);
    stackedWidget->addWidget(colorPickerWidget);

    // The color picker doesn't emit signals, so we must filter its events
    colorPickerWidget->installEventFilter(this);

    // --- Connect Signals and Slots ---

    // 1. When color is changed in picker, update drawing widget's pen
    connect(colorPickerWidget, &ColorPickerWidget::colorChanged,
            drawingWidget, &TransparentWidget::setPenColor);
            
    // 2. Right-click on drawing widget clears the canvas
    connect(drawingWidget, &TransparentWidget::rightButtonClicked,
            drawingWidget, &TransparentWidget::clearCanvas);

    // 3. Right-double-click on drawing widget toggles the color picker view
    connect(drawingWidget, &TransparentWidget::rightButtonDoubleClicked,
            this, &MainWindow::toggleColorPicker);
            
    // 4. When a tool is selected in picker, update drawing widget's tool
    connect(colorPickerWidget, &ColorPickerWidget::toolSelected,
            drawingWidget, &TransparentWidget::setTool);

    // 5. Middle-click on drawing widget closes the application
    connect(drawingWidget, &TransparentWidget::middleButtonClicked,
            this, &MainWindow::close);

    // Set initial color
    drawingWidget->setPenColor(colorPickerWidget->getCurrentColor());
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

void MainWindow::toggleColorPicker()
{
    int currentIndex = stackedWidget->currentIndex();
    int nextIndex = (currentIndex + 1) % stackedWidget->count();
    stackedWidget->setCurrentIndex(nextIndex);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // This filter now ONLY handles events for the color picker,
    // because the drawing widget handles its own clicks and emits signals.
    if (obj == colorPickerWidget && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            toggleColorPicker();
            return true; // Event is handled
        }
    }
    // Standard event processing for other events
    return QMainWindow::eventFilter(obj, event);
}

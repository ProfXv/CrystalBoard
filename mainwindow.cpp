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

    // Install event filters to catch right-clicks on child widgets
    drawingWidget->installEventFilter(this);
    colorPickerWidget->installEventFilter(this);

    // Connect color picker changes to the drawing widget's pen
    connect(colorPickerWidget, &ColorPickerWidget::colorChanged,
            drawingWidget, &TransparentWidget::setPenColor);
            
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

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            int currentIndex = stackedWidget->currentIndex();
            int nextIndex = (currentIndex + 1) % stackedWidget->count();
            stackedWidget->setCurrentIndex(nextIndex);
            return true; // Event is handled, don't pass it further
        }
    }
    // Standard event processing for other events
    return QMainWindow::eventFilter(obj, event);
}

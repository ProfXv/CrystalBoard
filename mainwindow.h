#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QKeyEvent>
#include "transparentwidget.h"
#include "colorpickerwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void toggleColorPicker();

    QStackedWidget *stackedWidget;
    TransparentWidget *drawingWidget;
    ColorPickerWidget *colorPickerWidget;
};

#endif // MAINWINDOW_H

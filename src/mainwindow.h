#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QKeyEvent>
#include "canvas.h"
#include "helppanel.h"
#include <QVariantMap>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QVariantMap &cmdLineOptions, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void toggleHelpPanel();
    void resetSettings();

private:
    void loadSettings();
    void saveSettings();
    void applyDefaultSettings();

    bool m_isLeftButtonPressed;
    bool m_isRightButtonPressed;

    QVariantMap m_cmdLineOptions;

    QStackedWidget *stackedWidget;
    Canvas *canvas;
    HelpPanel *helpPanel;
};

#endif // MAINWINDOW_H
#ifndef HELPPANEL_H
#define HELPPANEL_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QColor>
#include <QPaintEvent>
#include "canvas.h" // Include for the Tool enum

class HelpPanel : public QWidget
{
    Q_OBJECT

public:
    explicit HelpPanel(QWidget *parent = nullptr);
    QColor getCurrentColor() const;

public slots:
    void onPenColorChanged(const QColor &color);

signals:
    void colorChanged(const QColor &color);
    void toolSelected(Tool tool);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *helpLabel;
    QColor currentColor;
};

#endif // HELPPANEL_H

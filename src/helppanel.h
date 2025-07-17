#ifndef HELPPANEL_H
#define HELPPANEL_H

#include <QWidget>
#include <QLabel>

class HelpPanel : public QWidget
{
    Q_OBJECT

public:
    explicit HelpPanel(QWidget *parent = nullptr);

private:
    QLabel *helpLabel;
};

#endif // HELPPANEL_H

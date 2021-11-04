#ifndef TITLEBAR_HPP
#define TITLEBAR_HPP

#include <QWidget>

class Titlebar : public QWidget
{
    Q_OBJECT

public:
    explicit Titlebar(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent*) override;
};

#endif // TITLEBAR_HPP

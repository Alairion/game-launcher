#include "titlebar.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsBlurEffect>

Titlebar::Titlebar(QWidget* parent)
:QWidget{parent}
{

}

void Titlebar::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(palette().dark().color());
    painter.drawLine(0, height() - 1, width(), height() - 1);
}

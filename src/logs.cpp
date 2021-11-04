#include "logs.hpp"
#include "ui_logs.h"

#include <QPainter>

Logs::Logs(QWidget *parent)
:QWidget{parent}
,ui{new Ui::Logs}
{
    ui->setupUi(this);
}

Logs::~Logs()
{
    delete ui;
}

void Logs::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{"#20000000"});
    painter.drawRect(rect());
}

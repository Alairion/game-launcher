#include "log.hpp"
#include "ui_log.h"

Log::Log(QWidget *parent)
:QWidget{parent}
,ui{new Ui::Log}
{
    ui->setupUi(this);
}

Log::~Log()
{
    delete ui;
}

#include "credits.hpp"
#include "ui_credits.h"

#include <QPainter>

#include <fstream>

#include <origins/updater.hpp>

Credits::Credits(QWidget *parent)
:QWidget{parent}
,ui{new Ui::Credits}
{
    ui->setupUi(this);
    refresh();
}

Credits::~Credits()
{
    delete ui;
}

void Credits::setScrollBar(QScrollBar* scrollbar)
{
    ui->area->setVerticalScrollBar(scrollbar);
}

void Credits::refresh()
{
    std::ifstream ifs{std::string{origins::game_directory} + "/Credits.txt"};
    if(ifs)
    {
        QString credits{};
        credits += "<html><head/><body align=\"center\">";
        credits += QString::fromStdString(std::string{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{}});
        credits.replace("[", "<p><u><b>");
        credits.replace("]", "</b></u></p>");
        credits.replace("\n", "<br>");
        credits += "</body></html>";

        ui->area->setText(credits);
    }
    else
    {
        ui->area->setText("<html><body align=\"center\">Les crédits ne sont pas disponibles :(</body></html>");
    }
}

void Credits::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{"#20000000"});
    painter.drawRect(rect());
}

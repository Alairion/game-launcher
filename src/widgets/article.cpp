#include "article.hpp"
#include "ui_article.h"

#include <QPainter>
#include <QMouseEvent>

Article::Article(QWidget* parent)
:QWidget{parent}
,ui{new Ui::Article}
{
    ui->setupUi(this);
}

Article::~Article()
{
    delete ui;
}

void Article::setData(const QString& title, const QString& preview, const QString& details)
{
    ui->title->setText(title);
    ui->preview->setText(preview);
    ui->details->setText(details);
}

void Article::enterEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Hovered;
        update();
    }
}

void Article::leaveEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state &= ~State::Hovered;
        update();
    }
}

void Article::mousePressEvent(QMouseEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Pressed;
        update();
    }
}

void Article::mouseReleaseEvent(QMouseEvent* event)
{
    if(isEnabled())
    {
        m_state &= ~State::Pressed;
        if(rect().contains(event->pos()))
            emit pushed();

        update();
    }
}

void Article::paintEvent(QPaintEvent*)
{
    QPainter painter{this};

    painter.setPen(Qt::NoPen);
    if(m_state & State::Pressed)
        painter.fillRect(rect(), palette().mid());
    else if(m_state & State::Hovered)
        painter.fillRect(rect(), palette().midlight());
    else
        painter.fillRect(rect(), palette().light());

    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, width() - 1, height() - 1);
}

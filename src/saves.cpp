#include "saves.hpp"
#include "ui_saves.h"

#include <iostream>

#include <QPainter>
#include <QFile>

#include <origins/updater.hpp>

static QString savePath(int i)
{
    return QString{origins::game_directory} + "/save" + QString::number(i + 1) + ".rxdata";
}

void shiftSaves()
{
    const auto save1{savePath(0)};
    const auto save2{savePath(1)};
    const auto save3{savePath(2)};

    if(QFile::exists(save2) && !QFile::exists(save1))
    {
        QFile::rename(save2, save1);
    }

    if(QFile::exists(save3))
    {
        if(!QFile::exists(save1))
        {
            QFile::rename(save3, save1);
        }
        else if(!QFile::exists(save2))
        {
            QFile::rename(save3, save2);
        }
    }
}

Saves::Saves(QWidget* parent)
:QWidget{parent}
,ui{new Ui::Saves}
,m_timer{new QTimer{this}}
{
    ui->setupUi(this);

    shiftSaves();

    m_saves = {ui->first, ui->second, ui->third};

    for(int i{}; i < 3; ++i)
    {
        m_saves[i]->setFile(savePath(i));

        connect(m_saves[i], &Save::removed, [this]{slotRemoved();});
        connect(m_saves[i], &Save::filled, [this]{slotFilled();});
    }

    showSaves();

    connect(m_timer, &QTimer::timeout, [this]
    {
        shiftSaves();
        showSaves();
    });

    m_timer->start(2500);
}

Saves::~Saves()
{
    delete ui;
}

void Saves::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{"#20000000"});
    painter.drawRect(rect());
}

void Saves::slotRemoved()
{
    shiftSaves();
    showSaves();
}

void Saves::slotFilled()
{
    shiftSaves();
    showSaves();
}

void Saves::showSaves()
{
    for(int i{}; i < 3; ++i)
    {
        m_saves[i]->setFile(savePath(i));
    }

    for(int i{1}; i < 3; ++i)
    {
        if(m_saves[i]->isFilled() || m_saves[i - 1]->isFilled())
        {
            m_saves[i]->show();
        }

        if(!m_saves[i]->isFilled() && !m_saves[i - 1]->isFilled())
        {
            m_saves[i]->hide();
        }
    }
}

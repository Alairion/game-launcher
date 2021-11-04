#include "progressbar.hpp"

#include <QPainter>

ProgressBar::ProgressBar(QWidget* parent)
:QWidget{parent}
{

}

void ProgressBar::setText(QString text)
{
    m_text = std::move(text);
    repaint();
}

void ProgressBar::setPercent(double value)
{
    m_percent = value;
    repaint();
}

void ProgressBar::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setRenderHints(QPainter::TextAntialiasing |  QPainter::TextAntialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().light().color());
    painter.drawRect(0, 4, width() * m_percent, 24);

    painter.setPen(palette().buttonText().color());
    painter.setFont(font());
    painter.drawText(0, 0, width(), height(), Qt::AlignCenter, m_text);
}

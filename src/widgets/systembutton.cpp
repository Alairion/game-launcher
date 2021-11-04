#include "systembutton.hpp"

#include <QPainter>
#include <QMouseEvent>

SystemButton::SystemButton(QWidget *parent)
:QWidget{parent}
{
    setFixedSize(32, 32);
}

void SystemButton::setType(std::uint32_t type)
{
    m_type = type;
    repaint();
}

void SystemButton::enterEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Hovered;
        repaint();
    }
}

void SystemButton::leaveEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state &= ~State::Hovered;
        repaint();
    }
}

void SystemButton::mousePressEvent(QMouseEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Pressed;
        repaint();
    }
}

void SystemButton::mouseReleaseEvent(QMouseEvent* event)
{
    if(isEnabled())
    {
        m_state &= ~State::Pressed;
        if(rect().contains(event->pos()))
            emit pushed();

        repaint();
    }
}

void SystemButton::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setRenderHints(QPainter::Antialiasing);

    QColor color{palette().buttonText().color()};
    if((m_state & State::Hovered))
        color = QColor{"#00a8ff"};

    if((m_state & State::Pressed))
        color = color.darker(130);

    const auto scale{width() / 16};

    painter.setPen(QPen{color, 2.0});

    if(m_type == Type::Minimize)
    {
        painter.drawLine(4 * scale, 12 * scale, 12 * scale, 12 * scale);
    }
    else if(m_type == Type::Maximize)
    {
        painter.drawRect(4 * scale, 4 * scale, 8 * scale, 8 * scale);
    }
    else if(m_type == Type::Restore)
    {
        painter.drawRect(4 * scale, 4 * scale, 8 * scale, 8 * scale);
        painter.drawLine(6 * scale, 4 * scale, 6 * scale, 4);
        painter.drawLine(6 * scale, 4, 14 * scale, 2 * scale);
        painter.drawLine(14 * scale, 2 * scale, 14 * scale, 10 * scale);
        painter.drawLine(14 * scale, 10 * scale, 12 * scale, 10 * scale);
    }
    else if(m_type == Type::Close)
    {
        painter.drawLine(4 * scale, 4 * scale, 12 * scale, 12 * scale);
        painter.drawLine(12 * scale, 4 * scale, 4 * scale, 12 * scale);
    }
    else if(m_type == Type::LeftArrow)
    {
        painter.drawLine(8 * scale, 4 * scale, 4 * scale, 8 * scale);
        painter.drawLine(4 * scale, 8 * scale, 8 * scale, 12 * scale);
    }
    else if(m_type == Type::UpArrow)
    {
        painter.drawLine(4 * scale, 8 * scale, 8 * scale, 4 * scale);
        painter.drawLine(8 * scale, 4 * scale, 12 * scale, 8 * scale);
    }
    else if(m_type == Type::RightArrow)
    {
        painter.drawLine(8 * scale, 4 * scale, 12 * scale, 8 * scale);
        painter.drawLine(12 * scale, 8 * scale, 8 * scale, 12 * scale);
    }
    else if(m_type == Type::DownArrow)
    {
        painter.drawLine(4 * scale, 8 * scale, 8 * scale, 12 * scale);
        painter.drawLine(8 * scale, 12 * scale, 12 * scale, 8 * scale);
    }
}

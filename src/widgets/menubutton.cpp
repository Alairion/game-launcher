#include "menubutton.hpp"

#include <QMouseEvent>
#include <QPainter>

MenuButton::MenuButton(QWidget *parent)
:QWidget{parent}
{

}

void MenuButton::drawLine(std::uint32_t value)
{
    m_drawLine = value;
    update();
}

void MenuButton::setCheckable(bool value)
{
    m_checkable = value;
}

void MenuButton::setPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
    update();
}

void MenuButton::setText(const QString& text)
{
    m_text = text;
    update();
}

void MenuButton::bump()
{
    m_state &= ~State::Pushed;
    emit bumped();
    update();
}

void MenuButton::enterEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Hovered;
        update();
    }
}

void MenuButton::leaveEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state &= ~State::Hovered;
        update();
    }
}

void MenuButton::mousePressEvent(QMouseEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Pressed;
        update();
    }
}

void MenuButton::mouseReleaseEvent(QMouseEvent* event)
{
    if(isEnabled())
    {
        m_state &= ~State::Pressed;
        if(rect().contains(event->pos()))
        {
            if(m_checkable)
            {
                if((m_state & State::Pushed))
                {
                    m_state &= ~State::Pushed;
                    emit bumped();
                }
                else
                {
                    m_state |= State::Pushed;
                    emit pushed();
                }
            }
            else
            {
                emit pushed();
            }
        }

        update();
    }
}

void MenuButton::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    QRect area{0, 0, width(), height()};
    if(m_drawLine & DrawLine::Left)
        area.adjust(1, 0, 0, 0);
    if(m_drawLine & DrawLine::Top)
        area.adjust(0, 1, 0, 0);
    if(m_drawLine & DrawLine::Right)
        area.adjust(0, 0, -1, 0);
    if(m_drawLine & DrawLine::Bottom)
        area.adjust(0, 0, 0, -1);

    if((m_state & State::Pushed) && (m_state & State::Hovered))
        painter.fillRect(area, palette().dark());
    else if((m_state & State::Pressed) || (m_state & State::Pushed))
        painter.fillRect(area, palette().mid());
    else if(m_state & State::Hovered)
        painter.fillRect(area, palette().midlight());
    else
        painter.fillRect(area, palette().light());

    painter.setPen(palette().dark().color());
    if((m_drawLine & DrawLine::Left))
        painter.drawLine(0, 0, 0, height());
    if((m_drawLine & DrawLine::Top))
        painter.drawLine(0, 0, width(), 0);
    if((m_drawLine & DrawLine::Right))
        painter.drawLine(width() - 1, 0, width() - 1, height());
    if((m_drawLine & DrawLine::Bottom))
        painter.drawLine(0, height() - 1, width(), height() - 1);

    painter.setPen(palette().buttonText().color());

    if(m_pixmap.isNull())
    {
        painter.drawText(0, 0, width(), height(), Qt::AlignCenter, m_text);
    }
    else
    {
        painter.drawText(QRect{width() / 4, 0, width() - (width() / 4), height()}, Qt::AlignVCenter, m_text);
        painter.drawPixmap((64 - m_pixmap.width()) / 2, (64 - m_pixmap.height()) / 2, m_pixmap);
    }
}

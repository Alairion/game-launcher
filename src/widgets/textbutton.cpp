#include "textbutton.hpp"

#include <QPainter>
#include <QMouseEvent>

TextButton::TextButton(QWidget* parent)
:QWidget{parent}
{

}

void TextButton::setText(QString text)
{
    m_text = std::move(text);

    QFontMetrics metrics{font()};
    const auto textSize{metrics.size(Qt::TextSingleLine, m_text)};
    setFixedSize(textSize.width() + m_icon.width() + 8, std::max(textSize.height(), m_icon.height()));

    repaint();
}

void TextButton::setIcon(QPixmap icon)
{
    m_icon = std::move(icon);

    QFontMetrics metrics{font()};
    const auto textSize{metrics.size(Qt::TextSingleLine, m_text)};
    setFixedSize(textSize.width() + m_icon.width() + 8, std::max(textSize.height(), m_icon.height()));

    repaint();
}

void TextButton::enterEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Hovered;
        repaint();
    }
}

void TextButton::leaveEvent(QEvent*)
{
    if(isEnabled())
    {
        m_state &= ~State::Hovered;
        repaint();
    }
}

void TextButton::mousePressEvent(QMouseEvent*)
{
    if(isEnabled())
    {
        m_state |= State::Pressed;
        repaint();
    }
}

void TextButton::mouseReleaseEvent(QMouseEvent* event)
{
    if(isEnabled())
    {
        m_state &= ~State::Pressed;
        if(rect().contains(event->pos()))
            emit pushed();

        repaint();
    }
}

void TextButton::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.drawPixmap(0, 0, m_icon);

    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.setFont(font());

    QColor color{palette().buttonText().color()};
    if((m_state & State::Hovered))
        color = QColor{"#00a8ff"};

    if((m_state & State::Pressed))
        color = color.darker(130);

    painter.setPen(QPen{color});
    painter.drawText(m_icon.width() + 8, 0, width() - m_icon.width() + 8, height(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

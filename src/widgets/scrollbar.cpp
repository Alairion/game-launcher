#include "scrollbar.hpp"

#include <QPainter>

#include "../mainwindow.hpp"

ScrollBar::ScrollBar(MainWindow* mainWindow, QWidget* parent)
:QScrollBar{Qt::Vertical, parent}
,m_mainWindow{mainWindow}
,m_upButton{new SystemButton{this}}
,m_downButton{new SystemButton{this}}
,m_layout{new QVBoxLayout{this}}
{
    setAutoFillBackground(false);
    setFixedWidth(16);
    setPalette(m_mainWindow->palette());
    setAttribute(Qt::WA_SetPalette);

    m_upButton->setFixedSize(16, 16);
    m_upButton->setType(SystemButton::UpArrow);
    m_upButton->setPalette(m_mainWindow->palette());
    m_upButton->setAttribute(Qt::WA_SetPalette);

    m_downButton->setFixedSize(16, 16);
    m_downButton->setType(SystemButton::DownArrow);
    m_downButton->setPalette(m_mainWindow->palette());
    m_downButton->setAttribute(Qt::WA_SetPalette);

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_layout->addWidget(m_upButton);
    m_layout->addSpacerItem(new QSpacerItem{40, 16, QSizePolicy::Minimum, QSizePolicy::Expanding});
    m_layout->addWidget(m_downButton);

    connect(m_mainWindow, &MainWindow::sizeChanged, this, &ScrollBar::refreshBackground);
    connect(m_upButton, &SystemButton::pushed, this, [this]{triggerAction(QAbstractSlider::SliderSingleStepSub);});
    connect(m_downButton, &SystemButton::pushed, this, [this]{triggerAction(QAbstractSlider::SliderSingleStepAdd);});
}

void ScrollBar::refreshBackground()
{
    m_background = m_mainWindow->bluredBackground().copy(QRect{mapTo(m_mainWindow, QPoint{}) + QPoint{MainWindow::blurRadius, MainWindow::blurRadius}, size()});
    update();
}

void ScrollBar::showEvent(QShowEvent*)
{
    refreshBackground();
}

void ScrollBar::paintEvent(QPaintEvent*)
{
    QPainter painter{this};

    painter.drawPixmap(rect(), m_background);
    painter.fillRect(rect(), QColor{"#80000000"});
    painter.fillRect(rect(), QColor{"#20000000"});

    const auto length{static_cast<double>(maximum() + pageStep() - minimum())};
    QRect barRect{0, static_cast<int>(16 + (height() - 32) * (value() / length)), width(), static_cast<int>((height() - 32) * (pageStep() / length))};
    painter.fillRect(barRect, palette().dark());
    painter.fillRect(barRect, palette().dark());

    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    barRect.adjust(0, 0, -1, -1);
    painter.drawRect(barRect);
    painter.drawRect(barRect);
}

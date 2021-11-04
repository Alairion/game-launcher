#ifndef SCROLLBAR_HPP
#define SCROLLBAR_HPP

#include <QScrollBar>
#include <QVBoxLayout>

#include "systembutton.hpp"

class MainWindow;

class ScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    ScrollBar(MainWindow* mainWindow, QWidget* parent = nullptr);

protected:
    virtual void showEvent(QShowEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

private slots:
    void refreshBackground();

private:
    MainWindow* m_mainWindow{};
    SystemButton* m_upButton{};
    SystemButton* m_downButton{};
    QVBoxLayout* m_layout{};
    QPixmap m_background{};
};

#endif // SCROLLBAR_HPP

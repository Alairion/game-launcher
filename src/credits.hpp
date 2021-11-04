#ifndef CREDITS_HPP
#define CREDITS_HPP

#include <QWidget>
#include <QScrollBar>

namespace Ui
{
class Credits;
}

class Credits : public QWidget
{
    Q_OBJECT

public:
    explicit Credits(QWidget* parent = nullptr);
    ~Credits();

    void setScrollBar(QScrollBar* scrollbar);
    void refresh();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    Ui::Credits *ui;
};

#endif // CREDITS_HPP

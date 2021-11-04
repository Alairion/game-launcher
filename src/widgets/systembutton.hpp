#ifndef SYSTEMBUTTON_HPP
#define SYSTEMBUTTON_HPP

#include <QWidget>

class SystemButton : public QWidget
{
    Q_OBJECT

private:
    enum State : std::uint32_t
    {
        Pressed = 0x01,
        Hovered = 0x02
    };

public:
    enum Type : std::uint32_t
    {
        Minimize = 0,
        Maximize = 1,
        Restore = 2,
        Close = 3,
        LeftArrow = 4,
        UpArrow = 5,
        RightArrow = 6,
        DownArrow = 7
    };

public:
    explicit SystemButton(QWidget *parent = nullptr);

signals:
    void pushed();

public slots:
    void setType(std::uint32_t type);

protected:
    virtual void enterEvent(QEvent*) override;
    virtual void leaveEvent(QEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

private:
    std::uint32_t m_type{};
    std::uint32_t m_state{};
};

#endif // SYSTEMBUTTON_HPP

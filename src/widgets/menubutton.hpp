#ifndef ICONBUTTON_HPP
#define ICONBUTTON_HPP

#include <QWidget>
#include <QPixmap>
#include <QIcon>

class MenuButton : public QWidget
{
    Q_OBJECT

private:
    enum State : std::uint32_t
    {
        Pressed = 0x01,
        Hovered = 0x02,
        Pushed = 0x04
    };

public:
    enum DrawLine : std::uint32_t
    {
        None = 0x00,
        Left = 0x01,
        Top = 0x02,
        Right = 0x04,
        Bottom = 0x08
    };

public:
    explicit MenuButton(QWidget *parent = nullptr);

    void drawLine(std::uint32_t value);
    void setCheckable(bool value);

signals:
    void pushed();
    void bumped();

public slots:
    void bump();
    void setPixmap(const QPixmap& pixmap);
    void setText(const QString& text);

protected:
    virtual void enterEvent(QEvent*) override;
    virtual void leaveEvent(QEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

private:
    QPixmap m_pixmap{};
    QString m_text{};
    std::uint32_t m_state{};
    std::uint32_t m_drawLine{};
    bool m_checkable{};
};

#endif // ICONBUTTON_HPP

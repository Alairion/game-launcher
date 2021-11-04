#ifndef TEXTBUTTON_HPP
#define TEXTBUTTON_HPP

#include <QWidget>

class TextButton : public QWidget
{
    Q_OBJECT

private:
    enum State : std::uint32_t
    {
        Pressed = 0x01,
        Hovered = 0x02
    };

public:
    explicit TextButton(QWidget *parent = nullptr);


signals:
    void pushed();

public slots:
    void setText(QString text);
    void setIcon(QPixmap icon);

protected:
    virtual void enterEvent(QEvent*) override;
    virtual void leaveEvent(QEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

private:
    QPixmap m_icon{};
    QString m_text{};
    std::uint32_t m_state{};
};

#endif // TEXTBUTTON_HPP

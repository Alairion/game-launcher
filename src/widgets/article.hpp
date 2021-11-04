#ifndef ARTICLE_HPP
#define ARTICLE_HPP

#include <QWidget>

namespace Ui
{
class Article;
}

class Article : public QWidget
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
    explicit Article(QWidget *parent = nullptr);
    ~Article();

    void setData(const QString& title, const QString& preview, const QString& details);

signals:
    void pushed();

protected:
    virtual void enterEvent(QEvent*) override;
    virtual void leaveEvent(QEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

private:
    Ui::Article *ui;
    std::uint32_t m_state{};
};

#endif // ARTICLE_HPP

#ifndef NEWS_HPP
#define NEWS_HPP

#include <vector>

#include <QWidget>
#include <QScrollBar>
#include <QDate>

#include "widgets/article.hpp"

namespace Ui
{
class News;
}

class News : public QWidget
{
    Q_OBJECT

    struct Entry
    {
        int id{};
        QString author{};
        QDate date{};
        QString title{};
        QString content{};
        QString preview{};
        QString link{};
        Article* article{};
    };

public:
    explicit News(QWidget* parent = nullptr);
    ~News();

    void setScrollBar(QScrollBar* listScrollbar, QScrollBar* areaScrollbar);

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void resizeEvent(QResizeEvent*) override;

private:
    QString format(const Entry& entry);

private:
    Ui::News *ui;
    std::vector<Entry> m_entries{};
    Entry* m_entry{};
};

#endif // NEWS_HPP

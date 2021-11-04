#include "news.hpp"
#include "ui_news.h"

#include <QPainter>
#include <QtSql>

#include <iostream>
#include <algorithm>

News::News(QWidget* parent)
:QWidget{parent}
,ui{new Ui::News}
{
    ui->setupUi(this);
    ui->close_button->setType(SystemButton::Close);

    QSqlDatabase db{QSqlDatabase::addDatabase("QMYSQL", "news_db")};
    db.setHostName("originsproject.eu");
    db.setPort(3306);
    db.setDatabaseName("wordpress");
    db.setUserName("news");
    db.setPassword("news");

    if(!db.open())
    {
        ui->area->setHtml("<html><head/><body><center><span style=\" font-size:20pt; color:#ffffff;\">Impossible de se connecter :(</span></center></body></html>");
        ui->list_scroll_area->setVisible(false);
        return;
    }

    QSqlQuery query{db};
    if(!query.exec("SELECT * FROM wp_news"))
    {
        ui->area->setHtml("<html><head/><body><center><span style=\" font-size:20pt; color:#ffffff;\">Impossible de récupérer les actualités :(</span></center></body></html>");
        ui->list_scroll_area->setVisible(false);
        return;
    }

    ui->article_area->setVisible(false);

    while(query.next())
    {
        Entry entry{};

        entry.id = query.value("ID").toInt();
        entry.author = query.value("author").toString();
        entry.date = QDate::fromString(query.value("post_date").toString().mid(0, 10), "yyyy-MM-dd");
        entry.title = query.value("post_title").toString();
        entry.content = query.value("post_content").toString();
        entry.link = query.value("guid").toString();

        const int begin{entry.content.indexOf("<p>") + 3};
        const int mid{entry.content.indexOf('<', begin)};
        const int end{entry.content.indexOf("</p>")};

        if(end - begin > 80)
            entry.preview = "<html><body>" + entry.content.mid(begin, std::min(mid - begin, 80)) + "...</body></html>";
        else
            entry.preview = "<html><body>" + entry.content.mid(begin, end - begin) + "</body></html>";

        m_entries.push_back(std::move(entry));
    }

    std::sort(std::begin(m_entries), std::end(m_entries), [](auto&& left, auto&& right){return left.date < right.date;});

    for(auto&& entry : m_entries)
    {
        entry.article = new Article;
        entry.article->setData(entry.title, entry.preview, "Par <font color=\"#00a8ff\">" + entry.author + "</font>, le " + entry.date.toString("dddd d MMMM yyyy"));
        ui->list_layout->insertWidget(0, entry.article);

        connect(entry.article, &Article::pushed, [this, &entry]
        {
            ui->list_scroll_area->setVisible(false);
            ui->article_area->setVisible(true);
            ui->title->setText(entry.title);
            ui->area->setHtml(format(entry));
            m_entry = &entry;
        });
    }

    ui->list_layout->addSpacerItem(new QSpacerItem{40, 16, QSizePolicy::Minimum, QSizePolicy::Expanding});

    connect(ui->close_button, &SystemButton::pushed, [this]
    {
        ui->article_area->setVisible(false);
        ui->list_scroll_area->setVisible(true);
        ui->area->clearCache();
        m_entry = nullptr;
    });
}

News::~News()
{
    delete ui;
}

void News::setScrollBar(QScrollBar* listScrollbar, QScrollBar* areaScrollbar)
{
    ui->list_scroll_area->setVerticalScrollBar(listScrollbar);
    ui->area->setVerticalScrollBar(areaScrollbar);
}

void News::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{"#20000000"});
    painter.drawRect(rect());
}

void News::resizeEvent(QResizeEvent*)
{
    if(m_entry)
    {
        ui->area->setHtml(format(*m_entry));
    }
}

QString News::format(const Entry& entry)
{
    const QString margin{QString::number((width() - 680) / 2)};

    QString text{"<html><head/><body style=\" margin-top:6px; margin-bottom:6px; margin-left:" + margin + "px; margin-right:" + margin + "px;\">"};
    text += entry.content;

    auto index{text.indexOf("<!--")};
    while(index != -1)
    {
        text.remove(index, text.indexOf("-->", index) - index + 3);
        index = text.indexOf("<!--");
    }

    index = text.indexOf("<img");
    while(index != -1)
    {
        text.insert(index, "<center>");
        index = text.indexOf("/>", index);
        text.insert(index + 2, "</center>");
        index = text.indexOf("<img", index);
    }

    index = text.indexOf("<figure");
    while(index != -1)
    {
        text.remove(index, text.indexOf('>', index) - index + 1);
        index = text.indexOf("</figure>", index);
        text.remove(index, 9);
        index = text.indexOf("<figure", index);
    }

    index = text.indexOf("<h2>");
    while(index != -1)
    {
        text.insert(index + 4, "<u>");
        index = text.indexOf("</h2>", index);
        text.insert(index, "</u>");
        index = text.indexOf("<h2>", index);
    }

    index = text.indexOf("https://www.youtube.com/watch?v=");
    while(index != -1)
    {
        const auto endIndex{index + 32};
        const auto videoId{text.mid(endIndex, text.indexOf('\n', endIndex) - endIndex)};

        text.remove(index, 32 + videoId.size());

        QString video{};
        video += "<br/><center><a href=\"https://www.youtube.com/watch?v=" + videoId;
        video += "\"><img src=\"https://img.youtube.com/vi/" + videoId + "/mqdefault.jpg\"/></a></center>";
        text.insert(index, video);

        index = text.indexOf("https://www.youtube.com/watch?v=", index + video.size());
    }

    text.replace("<figcaption>", "<p><center><i><font color=\"#EFEFEF\">");
    text.replace("</figcaption>", "</font></i></center></p>");

    text += "<hr/><p><i>Par <font color=\"#00a8ff\">" + entry.author + "</font>, le " + entry.date.toString("dddd d MMMM yyyy") + "</i>";
    text += " | <a href=\"" + entry.link + "\">Consulter l'article sur le site</a></p>";
    text += "</body></html>";

    return text;
}

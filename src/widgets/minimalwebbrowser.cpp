#include "minimalwebbrowser.hpp"

#include <QPainter>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>

MinimalWebBrowser::MinimalWebBrowser(QWidget* parent)
:QTextBrowser{parent}
{
    m_manager.connectToHostEncrypted("originsproject.eu");

    document()->setDefaultStyleSheet("a{color: #00a8ff;}");
}

QVariant MinimalWebBrowser::loadResource(int type, const QUrl& name)
{
    if(type == QTextDocument::ImageResource && name.scheme().contains("http"))
    {
        if(auto it{m_cache.find(name.toString().toStdString())}; it != std::end(m_cache))
        {
            if(it->second.width() > 660)
            {
                return it->second.scaledToWidth(660, Qt::SmoothTransformation);
            }

            return it->second;
        }

        QNetworkReply* reply{m_manager.get(QNetworkRequest{name})};
        connect(reply, &QNetworkReply::finished, [this, reply, name]
        {
            if(reply->error() == QNetworkReply::NoError)
            {
                QPixmap pixmap{};
                pixmap.loadFromData(reply->readAll());

                if(name.host() == "img.youtube.com")
                {
                    QPainter painter{&pixmap};
                    painter.drawPixmap(0, 0, QPixmap{":/assets/youtube.png"});
                }

                m_cache.emplace(name.toString().toStdString(), std::move(pixmap));

                reply->deleteLater();
                setDocument(document());
            }
        });
    }

    return QTextBrowser::loadResource(type, name);
}

void MinimalWebBrowser::resizeEvent(QResizeEvent* event)
{
    setDocument(document());

    QTextBrowser::resizeEvent(event);
}

void MinimalWebBrowser::clearCache()
{
    m_cache.clear();
}

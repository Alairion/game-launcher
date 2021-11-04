#ifndef MINIMALWEBBROWSER_HPP
#define MINIMALWEBBROWSER_HPP

#include <unordered_map>
#include <string>

#include <QNetworkAccessManager>
#include <QTextBrowser>
#include <QPixmap>

class MinimalWebBrowser : public QTextBrowser
{
public:
    MinimalWebBrowser(QWidget* parent = nullptr);

    virtual QVariant loadResource(int type, const QUrl &name) override;

public slots:
    void clearCache();

protected:
    virtual void resizeEvent(QResizeEvent*) override;

private:
    QNetworkAccessManager m_manager{};
    std::unordered_map<std::string, QPixmap> m_cache{};
};

#endif // MINIMALWEBBROWSER_HPP

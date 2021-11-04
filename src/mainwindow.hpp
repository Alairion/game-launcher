#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <vector>

#include <QMainWindow>
#include <QStackedLayout>

#include "widgets/scrollbar.hpp"
#include "updater.hpp"
#include "gamewatcher.hpp"

#include "news.hpp"
#include "credits.hpp"
#include "saves.hpp"
#include "logs.hpp"
#include "options.hpp"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    static constexpr long borderWidth{5};
    static constexpr int blurRadius{16};
    static constexpr int titlebarHeight{64};
    static constexpr int menuWidth{224};

private:
    enum : int
    {
        ContentNone = 0,
        ContentNews = 1,
        ContentCredits = 2,
        ContentSaves = 3,
        ContentLogs = 4,
        ContentOptions = 5
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const QPixmap& bluredBackground() const;

signals:
    void sizeChanged();

public slots:
    void changeBackground(const QPixmap& pixmap);

protected:
    virtual void resizeEvent(QResizeEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;
    virtual bool nativeEvent(const QByteArray&, void *message, long *result) override;

private slots:
    void repaintBackground();
    void checkUpdateResult(Updater::InstallationStatus status);
    void updateResult(Updater::InstallationStatus status);
    void updateCanceled();
    void displayUpdateWindow();

    void switchMode();
    void applyParameters();

    void launchGame();
    void checkGameRunning(bool running);

    void displayNews();
    void closeNews();
    void displayCredits();
    void closeCredits();
    void displaySaves();
    void closeSaves();
    void displayLogs();
    void closelogs();
    void displayOptions();
    void closeOptions();

private:
    Ui::MainWindow* ui{};
    Updater* m_updater{};
    News* m_news{};
    Credits* m_credits{};
    Saves* m_saves{};
    Logs* m_logs{};
    Options* m_options{};
    std::uint32_t m_backgroundId{};
    QPixmap m_backgroundImage{}; //The image
    QPixmap m_scaledBackground{};
    QPixmap m_bluredBackground{}; //The image which size == size() but blured
    std::vector<QWidget*> m_treatAsTitlebar{};
    bool m_newMaximize{};
    GameWatcher* m_watcher{};
};

#endif // MAINWINDOW_HPP

#ifndef UPDATEWINDOW_HPP
#define UPDATEWINDOW_HPP

#include <QWidget>

#include "updater.hpp"

namespace Ui
{
class UpdateStatus;
}

class UpdateStatus : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateStatus(QWidget* parent = nullptr);
    ~UpdateStatus();

signals:
    void sizeChanged();
    void updateCanceled();
    void closed();

public slots:
    void minimize();
    void maximize();
    void switchSize();
    void reset();
    void result(Updater::InstallationStatus status);
    void stepChanged(origins::step_changed step);
    void statusChanged(origins::download_status status);
    void error(const QString& message);

private slots:
    void cancel();

protected:
    virtual void resizeEvent(QResizeEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

private:
    QString formatData(std::uint64_t bytes);
    QString formatTime(std::uint64_t seconds);
    QString formatRecover(const origins::download_status& status);
    QString formatDownload(const origins::download_status& status);
    QString formatInstall(const origins::download_status& status);

private:
    Ui::UpdateStatus* ui;
    std::vector<origins::download_status> m_statuses{};
    origins::download_step m_step{};
    std::uint64_t m_recoveredFileCount{};
    std::uint64_t m_recoveredFileData{};
};

#endif // UPDATEWINDOW_HPP

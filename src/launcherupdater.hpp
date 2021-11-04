#ifndef LAUNCHERUPDATER_HPP
#define LAUNCHERUPDATER_HPP

#include "updater.hpp"

class LauncherUpdater : public QThread
{
    Q_OBJECT

public:
    LauncherUpdater(QObject* parent = nullptr);
    ~LauncherUpdater();

    bool running();

public slots:
    void cancelUpdate();
    void detachUpdater();

signals:
    void result(Updater::InstallationStatus status);
    void stepChanged(origins::step_changed step);
    void statusChanged(origins::download_status status);
    void error(const QString& message);

protected:
    virtual void run() override;
    void processPipe(std::istream& is);

private:
    nes::process m_process{};
    std::mutex m_mutex{};
};

#endif // LAUNCHERUPDATER_HPP

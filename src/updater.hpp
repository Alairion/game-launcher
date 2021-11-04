#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <QThread>

#include <mutex>

#include <nes/process.hpp>

#include <origins/updater.hpp>

Q_DECLARE_METATYPE(origins::step_changed)
Q_DECLARE_METATYPE(origins::download_status)

class Updater : public QThread
{
    Q_OBJECT

private:
    enum Work : std::uint32_t
    {
        CheckUpdate = 0,
        Update = 1
    };

public:
    enum InstallationStatus : std::uint32_t
    {
        NotInstalled = 0,
        Outdated = 1,
        UpToDate = 2,
        Error = 3,
        Cancelled = 4
    };

public:
    Updater(QObject* parent = nullptr);
    ~Updater();

    QString getGameVersion() const;
    bool isInstalled() const;
    void checkInstallation();
    void runUpdate();
    void cancelUpdate();
    bool running();

signals:
    void result(InstallationStatus status);
    void stepChanged(origins::step_changed step);
    void statusChanged(origins::download_status status);
    void error(const QString& message);

protected:
    virtual void run() override;
    void checkUpdate();
    void update();
    void processPipe(std::istream& is);

private:
    nes::process m_process{};
    std::mutex m_mutex{};
    QString m_lastVersion{};
    Work m_work{};
};

Q_DECLARE_METATYPE(Updater::InstallationStatus);

#endif // UPDATER_HPP

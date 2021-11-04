#include "launcherupdater.hpp"

#include <origins/localization.hpp>

#include <QDir>

LauncherUpdater::LauncherUpdater(QObject* parent)
:QThread{parent}
{

}

LauncherUpdater::~LauncherUpdater()
{
    std::unique_lock lock{m_mutex};
    if(m_process.joinable())
    {
        m_process.kill();
        emit result(Updater::InstallationStatus::Cancelled);
    }
    lock.unlock();

    wait();
}

void LauncherUpdater::cancelUpdate()
{
    std::unique_lock lock{m_mutex};
    if(m_process.joinable())
        m_process.kill();
    lock.unlock();

    wait();
}

void LauncherUpdater::detachUpdater()
{
    std::unique_lock lock{m_mutex};
    if(m_process.joinable())
        m_process.detach();
}

bool LauncherUpdater::running()
{
    std::lock_guard lock{m_mutex};
    return m_process.active();
}

void LauncherUpdater::run()
{
    std::unique_lock lock{m_mutex};
    m_process = nes::process{std::string{origins::updater_directory} + "/origins_downloader.exe", std::vector<std::string>{"-launcher"}};
    lock.unlock();

    nes::pipe_istream pipe{origins::localize() + origins::updater_pipe_name};
    processPipe(pipe);

    lock.lock();

    if(m_process.joinable())
    {
        m_process.join();
    }
    else
    {
        emit result(Updater::InstallationStatus::Cancelled);
        return;
    }

    if(m_process.return_code() == 1)
        emit result(Updater::InstallationStatus::UpToDate);
    else if(m_process.return_code() == 3)
        emit result(Updater::InstallationStatus::Outdated);
    else
        emit result(Updater::InstallationStatus::Error);
}

void LauncherUpdater::processPipe(std::istream& is)
{
    origins::message_type type{};
    while(origins::read(is, type))
    {
        if(type == origins::message_type::step_changed)
        {
            origins::step_changed step{};
            origins::read(is, step);

            if(step.step >= origins::download_step::installing)
            {
                detachUpdater();
            }

            emit stepChanged(step);
        }
        else if(type == origins::message_type::download_status)
        {
            origins::download_status status{};
            origins::read(is, status);
            emit statusChanged(status);
        }
        else if(type == origins::message_type::error)
        {
            std::string message{};
            origins::read(is, message);
            emit error(QString::fromStdString(message));
        }
    }
}

#include "updater.hpp"

#include <origins/localization.hpp>

#include <fstream>
#include <sstream>

#include <QDir>

Updater::Updater(QObject* parent)
:QThread{parent}
{

}

Updater::~Updater()
{
    std::unique_lock lock{m_mutex};
    if(m_process.joinable())
    {
        m_process.kill();
        emit result(InstallationStatus::Cancelled);
    }
    lock.unlock();

    wait();
}

QString Updater::getGameVersion() const
{
    std::ifstream ifs{"game_version"};
    if(ifs)
        return QString::fromStdString(std::string{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{}});

    return "0.0.0";
}

bool Updater::isInstalled() const
{
    return getGameVersion() != "0.0.0";
}

void Updater::checkInstallation()
{
    m_work = Work::CheckUpdate;
    start();
}

void Updater::runUpdate()
{
    m_work = Work::Update;
    start();
}

void Updater::cancelUpdate()
{
    std::unique_lock lock{m_mutex};
    if(m_process.joinable())
        m_process.kill();
    lock.unlock();

    wait();
}

bool Updater::running()
{
    std::lock_guard lock{m_mutex};
    return m_process.active();
}

void Updater::run()
{
    if(m_work == Work::CheckUpdate)
    {
        checkUpdate();
    }
    else if(m_work == Work::Update)
    {
        update();
    }
}

void Updater::checkUpdate()
{
    if(!isInstalled())
        emit result(InstallationStatus::NotInstalled);

    std::unique_lock lock{m_mutex};
    m_process = nes::process{std::string{origins::updater_directory} + "/origins_downloader.exe", std::vector<std::string>{"-check"}};
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
        emit result(InstallationStatus::Cancelled);
        return;
    }

    if(m_process.return_code() == 1 || m_process.return_code() == 2)
        emit result(InstallationStatus::UpToDate);
    else if(m_process.return_code() == 3)
        emit result(InstallationStatus::Outdated);
    else
        emit result(InstallationStatus::Error);
}

void Updater::update()
{
    std::unique_lock lock{m_mutex};
    m_process = nes::process{std::string{origins::updater_directory} + "/origins_downloader.exe"};
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
        emit result(InstallationStatus::Cancelled);
        return;
    }

    if(m_process.return_code() == 1 || m_process.return_code() == 2)
        emit result(InstallationStatus::UpToDate);
    else if(m_process.return_code() == 3)
        emit result(InstallationStatus::Outdated);
    else
        emit result(InstallationStatus::Error);
}

void Updater::processPipe(std::istream& is)
{
    origins::message_type type{};
    while(origins::read(is, type))
    {
        if(type == origins::message_type::step_changed)
        {
            origins::step_changed step{};
            origins::read(is, step);
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

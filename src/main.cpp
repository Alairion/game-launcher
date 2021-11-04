#include "mainwindow.hpp"
#include "updatedialog.hpp"

#include <QApplication>
#include <QDir>

#include <origins/localization.hpp>
#include <origins/updater.hpp>

static bool needUpdate()
{
    nes::process process{std::string{origins::updater_directory} + "/origins_downloader.exe", std::vector<std::string>{"-launcher", "-check"}};
    nes::pipe_istream{origins::localize() + origins::updater_pipe_name}.ignore(std::numeric_limits<std::streamsize>::max());

    if(process.joinable())
        process.join();

    if(process.return_code() == 3)
        return true;

    return false;
}

static bool update(nes::named_mutex& mutex)
{
    try
    {
        if(needUpdate())
        {
            UpdateDialog dialog{mutex};
            dialog.setTitle("Mise à jour du launcher");
            dialog.setText("Une mise à jour du launcher est disponible.\nSouhaitez-vous le mettre à jour ?");
            dialog.exec();

            if(static_cast<QDialog&>(dialog).result() == QDialog::Accepted)
            {
                return true;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Can not check or launcher update: " << e.what() << std::endl;
    }

    return false;
}

int main(int argc, char** argv)
{
    nes::named_mutex mutex{origins::localize() + origins::launcher_mutex_name};
    if(!mutex.try_lock())
        return 0;

    QApplication application{argc, argv};

    qRegisterMetaType<origins::step_changed>("origins_step_changed");
    qRegisterMetaType<origins::download_status>("origins_download_status");
    qRegisterMetaType<Updater::InstallationStatus>("InstallationStatus");

    nes::named_mutex updaterMutex{origins::localize() + origins::updater_mutex_name};
    if(update(updaterMutex))
        return 0;

    if(QDir dir{}; !dir.exists(origins::game_directory))
        dir.mkdir(origins::game_directory);

    MainWindow window{};
    window.show();

    return application.exec();
}

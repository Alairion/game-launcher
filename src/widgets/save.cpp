#include "save.hpp"
#include "ui_save.h"

#include <fstream>
#include <cstring>

#include <QFileDialog>
#include <QStandardPaths>
#include <QPainter>

#include <origins/updater.hpp>

#include <nes/hash.hpp>

Save::Save(QWidget* parent)
:QWidget{parent}
,ui{new Ui::Save}
,m_copyTimer{new QTimer}
,m_restoreTimer{new QTimer}
,m_deleteTimer{new QTimer}
{
    ui->setupUi(this);

    ui->import_button->setText("Importer une sauvegarde");

    ui->copy_button->setText("Créer une copie");

    ui->restore_button->setText("Restaurer un backup");
    ui->restore_button->drawLine(MenuButton::Top);

    ui->delete_button->setText("Supprimer");
    ui->delete_button->drawLine(MenuButton::Top);

    ui->area->setCurrentIndex(0);

    m_copyTimer->setInterval(3000);
    m_copyTimer->setSingleShot(true);
    m_restoreTimer->setInterval(3000);
    m_restoreTimer->setSingleShot(true);
    m_deleteTimer->setInterval(3000);
    m_deleteTimer->setSingleShot(true);

    connect(ui->import_button, &MenuButton::pushed, this, &Save::importSave);
    connect(ui->copy_button, &MenuButton::pushed, this, &Save::copySave);
    connect(ui->restore_button, &MenuButton::pushed, this, &Save::restoreBackup);
    connect(ui->delete_button, &MenuButton::pushed, this, &Save::confirmDelete);

    connect(m_copyTimer, &QTimer::timeout, this, &Save::restoreCopyButtonText);
    connect(m_restoreTimer, &QTimer::timeout, this, &Save::restoreRestoreButtonText);
    connect(m_deleteTimer, &QTimer::timeout, this, &Save::cancelDelete);
}

Save::~Save()
{
    delete ui;
}

void Save::setFile(const QString& path)
{
    m_file = path;

    std::ifstream ifs{path.toStdString(), std::ios_base::binary};
    if(!ifs)
    {
        ui->area->setCurrentIndex(0);
        return;
    }

    ui->area->setCurrentIndex(1);

    const std::string data{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{}};
    const std::uint64_t newHash{nes::hash<std::string>{}(data)[0]};

    if(newHash == m_hash)
        return;

    m_hash = newHash;

    auto read_long = [](auto& it) -> std::int32_t
    {
        std::int32_t value{static_cast<std::int32_t>(*it++)};

        if(value == 0)
            return 0;

        if(value > 4 && value < 128)
            return value - 5;

        if(value < -4 && value > -129)
            return value + 5;

        if(value < 5)
        {
            std::uint32_t out{};
            for(std::int32_t i{}; i < value; ++i)
                out |= (static_cast<std::uint8_t>(*it++) << (i * 8));

            return static_cast<std::int32_t>(out);
        }

        if(value > -4)
        {
            value = -value;

            std::uint32_t out{};
            for(std::int32_t i{}; i < value; ++i)
                out |= (static_cast<std::uint8_t>(*it++) << (i * 8));

            return static_cast<std::int32_t>(out);
        }

        return 0;
    };

    auto read_string = [read_long](auto& it) -> QString
    {
        const auto size{read_long(it)};
        const auto out{QString::fromUtf8(it, size)};

        it += size;

        return out;
    };

    auto format_time = [](std::int32_t seconds)
    {
        const auto hours{seconds / (60 * 60)};
        const auto minutes{(seconds / 60) - (hours * 60)};

        const auto minutesStr{QString::number(minutes)};

        return QString::number(hours) + ":" + (std::size(minutesStr) == 2 ? minutesStr : "0" + minutesStr);
    };

    if(auto index{data.find("\x04\x08\x69")}; index != std::string::npos)
    {
        const auto* data_ptr{std::data(data) + index + 3};
        ui->time_label->setText("Temps de jeu : " + format_time(read_long(data_ptr) / 40));
    }
    else
    {
        ui->time_label->setText("Temps de jeu : ?");
    }

    if(auto index{data.find("\x04\x08\x5b\x09\x22")}; index != std::string::npos && index == data.rfind("\x04\x08\x5b\x09\x22"))
    {
        const auto* data_ptr{std::data(data) + index + 5};
        ui->name_label->setText("Nom : " + read_string(data_ptr));
        ++data_ptr;
        ui->id_label->setText("ID de dresseur : " + read_string(data_ptr));
        ++data_ptr;
        ui->caught_label->setText("Pokémon capturés : " + QString::number(read_long(data_ptr)));
    }
    else
    {
        ui->name_label->setText("Nom : ?");
        ui->id_label->setText("ID : ?");
        ui->caught_label->setText("Capturé : ?");
    }
}

bool Save::isFilled() const
{
    return ui->area->currentIndex() == 1;
}

void Save::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    if(ui->area->currentIndex() == 1)
    {
        painter.drawLine(1 + buttonWidth, 1, 1 + buttonWidth, height() - 2);
    }
}

void Save::importSave()
{
    const QString path{QFileDialog::getOpenFileName(this, tr("Importer une sauvegarde"), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0], "Sauvegarde (*.rxdata)")};

    if(!path.isEmpty())
    {
        QFile::copy(path, m_file);
        filled();
    }
}

void Save::copySave()
{
    const QString path{QFileDialog::getSaveFileName(this, tr("Créer une copie"), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0], "Sauvegarde (*.rxdata)")};

    if(!path.isEmpty())
    {
        QFile::remove(path);

        if(QFile::copy(m_file, path))
            ui->copy_button->setText("Copie réussi");
        else
            ui->copy_button->setText("Impossible de copier");
    }
    else
    {
        ui->copy_button->setText("Impossible de copier");
    }

    m_copyTimer->start();
}

void Save::restoreBackup()
{
    const QString dir{origins::game_directory + ("/Backups/" + m_file.mid(std::strlen(origins::game_directory) + 1, 5))};

    if(QDir{dir}.entryList(QStringList{"*.rxdata"}, QDir::NoDotAndDotDot | QDir::Files).size() > 0)
    {
        const QString path{QFileDialog::getOpenFileName(this, tr("Restaurer un backup"), dir, "Sauvegarde (*.rxdata)")};

        if(!path.isEmpty())
        {
            QFile::remove(m_file);
            QFile::copy(path, m_file);
            setFile(m_file);
        }
    }
    else
    {
        ui->restore_button->setText("Aucun backup disponible");
        m_restoreTimer->start();
    }
}

void Save::confirmDelete()
{
    disconnect(ui->delete_button, &MenuButton::pushed, this, &Save::confirmDelete);
    connect(ui->delete_button, &MenuButton::pushed, this, &Save::endDelete);

    ui->delete_button->setText("Confirmer la suppression");
    m_deleteTimer->start();
}

void Save::endDelete()
{
    disconnect(ui->delete_button, &MenuButton::pushed, this, &Save::endDelete);
    connect(ui->delete_button, &MenuButton::pushed, this, &Save::confirmDelete);

    QFile::remove(m_file);

    ui->area->setCurrentIndex(0);
    ui->delete_button->setText("Supprimer");
    m_deleteTimer->stop();

    removed();
}

void Save::restoreCopyButtonText()
{
    ui->copy_button->setText("Créer une copie");
}

void Save::restoreRestoreButtonText()
{
    ui->restore_button->setText("Restaurer un backup");
}

void Save::cancelDelete()
{
    disconnect(ui->delete_button, &MenuButton::pushed, this, &Save::endDelete);
    connect(ui->delete_button, &MenuButton::pushed, this, &Save::confirmDelete);

    ui->delete_button->setText("Supprimer");
}

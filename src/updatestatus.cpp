#include "updatestatus.hpp"
#include "ui_updatestatus.h"

#include <sstream>
#include <iomanip>

#include <QPainter>

UpdateStatus::UpdateStatus(QWidget* parent)
:QWidget{parent}
,ui{new Ui::UpdateStatus}
{
    ui->setupUi(this);

    ui->cancel_button->setType(SystemButton::Close);
    ui->hide_button->setType(SystemButton::DownArrow);

    connect(ui->cancel_button, &SystemButton::pushed, this, &UpdateStatus::cancel);
    connect(ui->hide_button, &SystemButton::pushed, this, &UpdateStatus::switchSize);
}

UpdateStatus::~UpdateStatus()
{
    delete ui;
}

void UpdateStatus::minimize()
{
    ui->details->setVisible(false);
    ui->hide_button->setType(SystemButton::UpArrow);
    setFixedHeight(32);
}

void UpdateStatus::maximize()
{
    ui->details->setVisible(true);
    ui->hide_button->setType(SystemButton::DownArrow);
    setFixedHeight(64);
}

void UpdateStatus::switchSize()
{
    ui->details->isVisible() ? minimize() : maximize();
}

void UpdateStatus::reset()
{
    m_statuses.clear();
    m_step = origins::download_step{};

    ui->cancel_button->setEnabled(true);

    ui->progress_bar->setText("");
    ui->progress_bar->setPercent(0.0);
    ui->details->setText("");
}

void UpdateStatus::result(Updater::InstallationStatus status)
{
    if(status == Updater::InstallationStatus::UpToDate)
        ui->details->setText("Vous pouvez fermer l'interface de mise à jour !");

    ui->cancel_button->setEnabled(true);
}

void UpdateStatus::stepChanged(origins::step_changed step)
{
    auto to_string = [](origins::download_step step) -> QString
    {
        switch(step)
        {
            case origins::download_step::checking_update: return "Étape 1/7 | Vérification de la version du jeu...";
            case origins::download_step::downloading_manifest: return "Étape 2/7 | Téléchargement du manifeste de téléchargement...";
            case origins::download_step::listing_files: return "Étape 3/7 | Détection des fichiers...";
            case origins::download_step::recovering: return "Étape 4/7 | Récupération des fichiers déjà téléchargés...";
            case origins::download_step::downloading_files: return "Étape 5/7 | Téléchargement des fichiers...";
            case origins::download_step::installing: return "Étape 6/7 | Installation...";
            case origins::download_step::retrieving_space: return "Étape 7/7 | Récupération d'espace disque...";
            case origins::download_step::finished: return "Vous pouvez maintenant jouer !";
        }

        return {};
    };

    if(step.step >= origins::download_step::installing)
        ui->cancel_button->setEnabled(false);

    m_step = step.step;
    m_statuses.clear();
    ui->progress_bar->setPercent(0.0);
    ui->progress_bar->setText(to_string(step.step));
    ui->details->setText("");
}

void UpdateStatus::statusChanged(origins::download_status status)
{
    if(m_step == origins::download_step::recovering)
    {
        m_recoveredFileCount = status.files_transferred;
        m_recoveredFileData = status.bytes_transferred;

        ui->details->setText(formatRecover(status));
    }
    else if(m_step == origins::download_step::downloading_manifest || m_step == origins::download_step::downloading_files)
    {
        ui->details->setText(formatDownload(status));
    }
    else if(m_step == origins::download_step::installing)
    {
        ui->details->setText(formatInstall(status));
    }
}

void UpdateStatus::error(const QString& error)
{
    maximize();
    ui->progress_bar->setText("Une erreur est survenue.");
    ui->details->setText(error);
}

void UpdateStatus::cancel()
{
    emit updateCanceled();
}

void UpdateStatus::resizeEvent(QResizeEvent*)
{
    emit sizeChanged();
}

void UpdateStatus::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(palette().dark().color());
    painter.drawLine(0, 0, width(), 0);
}

QString UpdateStatus::formatData(std::uint64_t bytes)
{
    std::stringstream ss{};
    ss << std::setprecision(2);
    if(bytes < 1024)
        ss << bytes << " o";
    else if(bytes < 1024 * 1024)
        ss << std::fixed << bytes / 1024.0 << " kio";
    else
        ss << std::fixed << bytes / (1024.0 * 1024.0) << " Mio";

    return QString::fromStdString(ss.str());
}

QString UpdateStatus::formatTime(std::uint64_t seconds)
{
    if(seconds < 60)
    {
        return QString::number(seconds) + "s";
    }
    else if(seconds < 60 * 60)
    {
        return QString::number(seconds / 60) + "min";
    }
    else
    {
        const auto hours{seconds / (60 * 60)};
        const auto minutes{(seconds / 60) - (60 * hours)};

        if(minutes > 0)
            return QString::number(hours) + "h et " + QString::number(minutes) + "min";
        else
            return QString::number(hours) + "h";
    }
}

QString UpdateStatus::formatRecover(const origins::download_status &status)
{
    return QString::number(status.files_transferred) + " fichiers récupérés | Quantité de données : " + formatData(status.bytes_transferred);
}

QString UpdateStatus::formatDownload(const origins::download_status& status)
{
    QString details{};

    if(status.files_to_transfer > 1)
    {
        const auto total_transfered{status.files_transferred + m_recoveredFileCount};
        const auto total_to_transfer{status.files_to_transfer + m_recoveredFileCount};

        details += QString::number(total_transfered) + '/' + QString::number(total_to_transfer) + " fichiers | ";
    }

    if(status.bytes_to_transfer != 0)
    {
        const auto total_transfered{status.bytes_transferred + m_recoveredFileData};
        const auto total_to_transfer{status.bytes_to_transfer + m_recoveredFileData};

        ui->progress_bar->setPercent(static_cast<double>(total_transfered) / (total_to_transfer));
        details += formatData(total_transfered) + " / " + formatData(total_to_transfer) + " ";
    }

    m_statuses.push_back(status);
    for(auto it{std::cbegin(m_statuses)}; it != std::cend(m_statuses); ++it)
    {
        if(status.download_time - it->download_time < 5.0)
        {
            m_statuses.erase(std::cbegin(m_statuses), it);
            break;
        }
    }

    if(m_statuses.back().download_time - m_statuses.front().download_time > 1.0)
    {
        const auto elapsed{m_statuses.back().download_time - m_statuses.front().download_time};
        const auto last_transfered{m_statuses.back().bytes_transferred + m_recoveredFileData};
        const auto first_transfered{m_statuses.front().bytes_transferred + m_recoveredFileData};
        const auto rate{static_cast<std::uint64_t>((last_transfered - first_transfered) / elapsed)};

        details += " (" + formatData(rate) + "/s) | Temps restant estimé: ";
        details += formatTime((status.bytes_to_transfer - status.bytes_transferred) / rate);
    }
    else
    {
        details += "(?) | Temps restant estimé : ?";
    }

    return details;
}

QString UpdateStatus::formatInstall(const origins::download_status& status)
{
    const auto total_transfered{status.files_transferred + m_recoveredFileCount};
    const auto total_to_transfer{status.files_to_transfer + m_recoveredFileCount};

    ui->progress_bar->setPercent(static_cast<double>(total_transfered) / total_to_transfer);

    return QString::number(total_transfered) + '/' + QString::number(total_to_transfer) + " fichiers installés";
}

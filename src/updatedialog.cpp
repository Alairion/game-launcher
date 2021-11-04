#include "updatedialog.hpp"
#include "ui_updatedialog.h"

#include <QGraphicsScene>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QRandomGenerator>
#include <QDir>

#ifdef _WIN32
    #include <windows.h>
    #include <windowsx.h>
    #include <dwmapi.h>
#endif

UpdateDialog::UpdateDialog(nes::named_mutex& mutex, QWidget* parent)
:QDialog{parent, Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint}
,ui{new Ui::UpdateDialog}
,m_mutex{&mutex}
,m_updater{new LauncherUpdater{this}}
{
#ifdef _WIN32
    SetWindowLongW(reinterpret_cast<HWND>(winId()), GWL_STYLE, GetWindowLongW(reinterpret_cast<HWND>(winId()), GWL_STYLE) | WS_CAPTION);
    const MARGINS shadow{5, 5, 5, 5};
    DwmExtendFrameIntoClientArea(reinterpret_cast<HWND>(winId()), &shadow);
#endif

    ui->setupUi(this);

    m_treatAsTitlebar.push_back(ui->title);

    ui->confimation_button->setText("Oui");
    ui->confimation_button->drawLine(MenuButton::Top);
    ui->cancel_button->setText("Non");
    ui->cancel_button->drawLine(MenuButton::Top | MenuButton::Right);
    ui->close_button->setType(SystemButton::Close);

    connect(ui->confimation_button, &MenuButton::pushed, [this]{startUpdate();});
    connect(ui->cancel_button, &MenuButton::pushed, [this]{this->done(QDialog::Rejected);});
    connect(ui->close_button, &SystemButton::pushed, [this]{this->done(QDialog::Rejected);});

    m_backgroundId = QRandomGenerator::global()->bounded(0, 3);
    changeBackground(QPixmap{":/assets/background_" + QString::number(m_backgroundId) + ".png"});
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::setText(const QString& text)
{
    ui->body->setText(text);
}

void UpdateDialog::setTitle(const QString& title)
{
    ui->title->setText(" " + title);
}

void UpdateDialog::changeBackground(const QPixmap& pixmap)
{
    m_backgroundImage = pixmap;
    repaintBackground();
}

void UpdateDialog::resizeEvent(QResizeEvent*)
{
    repaintBackground();
}

void UpdateDialog::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.drawPixmap(rect(), m_bluredBackground, QRect{blurRadius, blurRadius, width(), height()});

    int left{}, top{}, right{}, bottom{};
    getContentsMargins(&left, &top, &right, &bottom);
    left += borderWidth;
    top += borderWidth;
    right += borderWidth;
    bottom += borderWidth;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{"#80000000"});
    painter.drawRect(rect());
}

void UpdateDialog::repaintBackground()
{
    if(m_backgroundImage.isNull())
        return;

    auto scaledBackground{m_backgroundImage.scaled(size() + QSize{blurRadius * 2, blurRadius * 2}, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation)};
    m_bluredBackground = QPixmap{size() + QSize{blurRadius * 2, blurRadius * 2}};

    auto* effect{new QGraphicsBlurEffect};
    effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    effect->setBlurRadius(blurRadius);

    auto* item{new QGraphicsPixmapItem};
    item->setPixmap(scaledBackground);
    item->setGraphicsEffect(effect);

    QGraphicsScene scene{};
    scene.addItem(item);

    QPainter painter{&m_bluredBackground};
    scene.render(&painter, m_bluredBackground.rect(), scaledBackground.rect(), Qt::KeepAspectRatioByExpanding);

    update();
}

void UpdateDialog::startUpdate()
{
    ui->confimation_button->setVisible(false);
    ui->cancel_button->setVisible(false);
    ui->body->setText("Lancement de la mise à jour...");

    ui->close_button->disconnect();
    connect(ui->close_button, &SystemButton::pushed, m_updater, &LauncherUpdater::cancelUpdate);

    connect(m_updater, &LauncherUpdater::stepChanged, this, &UpdateDialog::stepChanged);
    connect(m_updater, &LauncherUpdater::result, this, &UpdateDialog::result);
    connect(m_updater, &LauncherUpdater::error, this, &UpdateDialog::error);

    m_mutex->lock();

    m_updater->start();
}

void UpdateDialog::result(Updater::InstallationStatus status)
{
    if(status == Updater::UpToDate)
    {
        done(QDialog::Accepted);
    }
    else
    {
        ui->confimation_button->setText("Réessayer");
        ui->confimation_button->setVisible(true);
        ui->cancel_button->setText("Annuler");
        ui->cancel_button->setVisible(true);
        ui->close_button->setEnabled(true);

        ui->close_button->disconnect();
        disconnect(m_updater, &LauncherUpdater::stepChanged, this, &UpdateDialog::stepChanged);
        disconnect(m_updater, &LauncherUpdater::result, this, &UpdateDialog::result);
        disconnect(m_updater, &LauncherUpdater::error, this, &UpdateDialog::error);

        connect(ui->close_button, &SystemButton::pushed, [this]{this->done(QDialog::Rejected);});
    }
}

void UpdateDialog::stepChanged(origins::step_changed step)
{
    auto to_string = [](origins::download_step step) -> QString
    {
        switch(step)
        {
            case origins::download_step::checking_update: return "Étape 1/7 | Vérification de la version du launcher...";
            case origins::download_step::downloading_manifest: return "Étape 2/7 | Téléchargement du manifeste de téléchargement...";
            case origins::download_step::listing_files: return "Étape 3/7 | Détection des fichiers...";
            case origins::download_step::recovering: return "Étape 4/7 | Récupération des fichiers déjà téléchargés...";
            case origins::download_step::downloading_files: return "Étape 5/7 | Téléchargement des fichiers...";
            case origins::download_step::installing: return "Étape 6/7 | Installation...";
            case origins::download_step::retrieving_space: return "Étape 7/7 | Récupération d'espace disque...";
            case origins::download_step::finished: return "Mise à jour réussi !";
        }

        return {};
    };

    if(step.step >= origins::download_step::installing)
    {
        done(QDialog::Accepted);
    }

    ui->body->setText(to_string(step.step));
}

void UpdateDialog::error(const QString& message)
{
    ui->body->setText("Erreur : " + message);
}

bool UpdateDialog::nativeEvent(const QByteArray&, void* message, long* result)
{
#ifdef _WIN32
    auto* msg{reinterpret_cast<MSG*>(message)};

    if(msg->message == WM_NCCALCSIZE)
    {
        auto& params{*reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam)};

        if(params.rgrc[0].top != 0)
            params.rgrc[0].top -= 1;

        *result = WVR_REDRAW;
        return true;
    }

    if(msg->message == WM_NCHITTEST)
    {
        *result = 0;
        if(!isEnabled())
            return true;

        const LONG x{GET_X_LPARAM(msg->lParam)};
        const LONG y{GET_Y_LPARAM(msg->lParam)};
        const int dpr{devicePixelRatio()};

        if(!ui->titlebar->rect().contains(ui->titlebar->mapFromGlobal(QPoint{x / dpr, y / dpr})))
            return false;

        for(auto&& widget : m_treatAsTitlebar)
        {
            if(widget->rect().contains(widget->mapFromGlobal(QPoint{x / dpr, y / dpr})))
            {
                *result = HTCAPTION;
                return true;
            }
        }

        if(!ui->titlebar->childAt(ui->titlebar->mapFromGlobal(QPoint{x / dpr, y / dpr})))
        {
             *result = HTCAPTION;
             return true;
        }

        return false;
    }
#endif

    return false;
}

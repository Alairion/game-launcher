#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <algorithm>
#include <fstream>

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsBlurEffect>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QDesktopServices>
#include <QUrl>
#include <QStackedLayout>
#include <QDebug>
#include <QPaintEngine>

#ifdef _WIN32
    #include <Windows.h>
    #include <windowsx.h>
    #include <dwmapi.h>
#endif

#include "widgets/scrollbar.hpp"

static QString getLauncherVersion()
{
    std::ifstream ifs{"launcher_version"};
    if(ifs)
        return QString::fromStdString(std::string{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{}});

    return {};
}

static constexpr std::array pokequote
{
    "« Il y a Serpang sous roche. »",
    "« Il y a Wailord sous gravier. »",
    "« Miaouss échaudé craint l'eau froide. »",
    "« Ne réveillez pas le Skitty qui dort. »",
    "« A Galopa donné on ne regarde pas la bride. »",
    "« Les Ponchiens aboient, la caravane passe. »",
    "« Petit à petit, le Roucool fait son nid. »",
    "« Les Medhyènas ne se mangent pas entre eux. »",
    "« Il ne faut pas vendre la peau de l'Ursaring avant de l'avoir capturé. »",
    "« Ponchiot qui aboie ne mord pas. »",
    "« Petit Barloche deviendra grand. »",
    "« La nuit, tous les Chaglams sont gris. »",
    "« Avoir un Chaglam dans la gorge. »",
    "« Quand on parle du Feunard, on en voit la queue. »",
    "« Quand Persian n’est pas là, les Pichus dansent. »",
    "« On n’apprend pas à un vieux Férosinge à faire la grimace. »",
    "« Un vent à décorner les Écrémeuhs. »",
    "« Muet comme un Magicarpe. »",
    "« Tôt ou tard, petit Aspicot, tu tomberas sur un bec. »",
    "« Un Nirondelle ne fait pas le printemps. »",
    "« Avoir un Mimigal au plafond. »",
    "« Être accroché comme un Kokiyas à son rocher. »",
    "« Gravalanch qui roule n'amasse pas mousse. »",
    "« Statitik du matin, chagrin. »",
    "« Il fait un froid de Couaneton. »",
    "« Un caractère de Grotichon. »",
    "« Mettre les pieds dans l'Archéomire. »",
    "« Être connu comme le Zoroark chromatique. »",
    "« Être myope comme un Rototaupe. »",
    "« Être rusé comme un Feunard. »",
    "« Avaler des Abos. »",
    "« Il ne faut pas courir deux Laporeilles à la fois. »",
    "« Il ne faut pas mettre tous ses Noeuneufs dans la même Pokéball. »",
    "« Prendre le Tauros par les cornes. »",
    "« Avoir un oeil de Lixy. »",
    "« Bailler aux Cornèbres. »",
    "« Avoir des larmes de Crocorible. »",
    "« Être doux comme un Wattouat. »",
    "« Ça ne casse pas trois pattes à un Couaneton. »",
    "« Être plat comme un Limonde. »",
    "« Un Sharpedo de la finance. »",
    "« Un panier de Krabboss. »",
    "« Celui qui ne voit rien d'étrange n'a jamais regardé un Colhomard en face. »",
    "« Donner de la confiture aux Grorets. »",
    "« On n'attrape pas les Dardargnans avec du vinaigre. »",
    "« Une langue de Séviper. »",
    "« Avoir des Charmillons dans le ventre. »",
    "« Avoir des Staris dans les yeux. »",
    "« Ça ne se trouve pas sous le sabot d'un Ponyta. »",
    "« Être fait comme un Rattata. »",
    "« Pousser comme des Trompignons. »",
    "« Il court, il court, le Farfuret. »",
    "« Quand les Poussifeus auront des dents. »",
    "« Avoir une mémoire de Donphan. »",
    "« Avoir des Chovsourirs dans le beffroi. »",
    "« Les Mystherbes sont toujours plus verts chez le voisin. »",
    "« La bave du Crapustule n'atteint pas le blanc Lakmécygne. »",
    "« C'est le Simularbre qui cache la forêt. »",
    "« Mygavolt du soir, espoir. »",
    "« Ça ne casse pas trois pattes à un Canarticho. »",
    "« Il court, il court, le Farfuret ! »",
    "« C'est en charpentant que l'on devient Charpenti. »",
    "« Être gai comme un Tylton. »",
    "« Quand le Noarfang chante, la nuit est silence. »",
    "« Il n'y a pas de Salamèche qui ne finisse par se brûler. »",
    "« La pensée est Chrysacier, la parole est Papilord. »",
    "« L'Aspicot file un mauvais Coconfort. »",
    "« Les mots sont des Rapasdepics qui tournent hallucinés. »",
    "« J'entends l'Absol, le Feunard, le Sabelette ! J'entends l'Absol et le Feunard chanter ! »",
    "« A Goupix endormi, rien ne lui tombe en gueule. »",
    "« Lorsque le Bekipan se gratte le séant, c'est qu'il va faire mauvais temps. »",
    "« L’expérience est un Lanturn que l’on porte sur le dos et qui n’éclaire jamais que le chemin parcouru. »",
    "« L’évolution est au Barpau ce que le Pharamp est aux bateaux. »",
    "« Pour les Roigadas, le monde est très simplifié. Tous les Ramoloss sont des sujets. »",
    "« Gare en fuyant le Scorplane à éviter la morsure d'Arbok. »",
    "« Un Roucool dans la main vaut mieux que deux dans les hautes herbes. »",
    "« Tant va le Nénupiot à l'eau qu'à la fin il se lasse. »",
    "« Quand les Goélises ont pied, il est temps de virer. »",
    "« Ronflex affamé n'a point d'oreilles. »",
};

MainWindow::MainWindow(QWidget *parent)
:QWidget{parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |  Qt::WindowMaximizeButtonHint}
,ui{new Ui::MainWindow}
,m_updater{new Updater{this}}
,m_news{new News}
,m_credits{new Credits}
,m_saves{new Saves}
,m_logs{new Logs}
,m_options{new Options}
,m_watcher{new GameWatcher}
{
#ifdef _WIN32
    SetWindowLongW(reinterpret_cast<HWND>(winId()), GWL_STYLE, GetWindowLongW(reinterpret_cast<HWND>(winId()), GWL_STYLE) | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
    const MARGINS shadow{1, 1, 1, 1};
    DwmExtendFrameIntoClientArea(reinterpret_cast<HWND>(winId()), &shadow);
#endif

    ui->setupUi(this);

    m_news->setScrollBar(new ScrollBar{this}, new ScrollBar{this});
    m_credits->setScrollBar(new ScrollBar{this});
    m_options->setScrollBar(new ScrollBar{this});

    ui->quote->setText(pokequote[QRandomGenerator::global()->bounded(std::size_t{}, std::size(pokequote) - 1)]);

    ui->content->addWidget(m_news);
    ui->content->addWidget(m_credits);
    ui->content->addWidget(m_saves);
    ui->content->addWidget(m_logs);
    ui->content->addWidget(m_options);

    m_treatAsTitlebar.push_back(ui->logo);

    ui->facebook_link->setText("Facebook");
    ui->facebook_link->setIcon(QPixmap{":/assets/facebook.png"});
    ui->discord_link->setText("Discord");
    ui->discord_link->setIcon(QPixmap{":/assets/discord.png"});
    ui->website_link->setText("Site");
    ui->website_link->setIcon(QPixmap{":/assets/ball.png"});
    ui->workshop_link->setText("Workshop");
    ui->workshop_link->setIcon(QPixmap{":/assets/pokemon_workshop.png"});

    ui->minimize_button->setType(SystemButton::Minimize);
    ui->maximize_button->setType(SystemButton::Maximize);
    ui->close_button->setType(SystemButton::Close);

    const auto version{m_updater->getGameVersion()};
    ui->game_version->setText(version == "0.0.0" ? "" : version);
    ui->launcher_version->setText(getLauncherVersion());

    ui->play_button->setText("Jouer");
    ui->play_button->setPixmap(QPixmap{":/assets/play.png"});

    ui->update_button->setText("Mise à jour");
    ui->update_button->setPixmap(QPixmap{":/assets/download.png"});
    ui->update_button->drawLine(MenuButton::Top);

    ui->news_button->setText("Actualités");
    ui->news_button->setPixmap(QPixmap{":/assets/news.png"});
    ui->news_button->drawLine(MenuButton::Top);
    ui->news_button->setCheckable(true);

    ui->credits_button->setText("Crédits");
    ui->credits_button->setPixmap(QPixmap{":/assets/credits.png"});
    ui->credits_button->drawLine(MenuButton::Top);
    ui->credits_button->setCheckable(true);

    ui->saves_button->setText("Sauvegardes");
    ui->saves_button->setPixmap(QPixmap{":/assets/save.png"});
    ui->saves_button->drawLine(MenuButton::Top);
    ui->saves_button->setCheckable(true);

    ui->logs_button->setText("Logs");
    ui->logs_button->setPixmap(QPixmap{":/assets/logs.png"});
    ui->logs_button->drawLine(MenuButton::Top);
    ui->logs_button->setCheckable(true);

    ui->options_button->setText("Options");
    ui->options_button->setPixmap(QPixmap{":/assets/options.png"});
    ui->options_button->drawLine(MenuButton::Top);
    ui->options_button->setCheckable(true);

    ui->update_status->minimize();
    ui->update_status->setVisible(false);

    connect(ui->facebook_link, &TextButton::pushed, []{QDesktopServices::openUrl(QUrl{"https://www.facebook.com/pages/Pok%C3%A9mon-Origins/223348097822997"});});
    connect(ui->discord_link, &TextButton::pushed,  []{QDesktopServices::openUrl(QUrl{"https://discord.gg/39G3pju"});});
    connect(ui->website_link, &TextButton::pushed,  []{QDesktopServices::openUrl(QUrl{"https://www.originsproject.eu"});});
    connect(ui->workshop_link, &TextButton::pushed, []{QDesktopServices::openUrl(QUrl{"https://pokemonworkshop.fr/forum/index.php"});});

    connect(ui->minimize_button, &SystemButton::pushed, this, &QWidget::showMinimized);
    connect(ui->maximize_button, &SystemButton::pushed, this, &MainWindow::switchMode);
    connect(ui->close_button, &SystemButton::pushed, this, &QWidget::close);

    connect(ui->play_button, &MenuButton::pushed, this, &MainWindow::launchGame);

    if(!m_updater->isInstalled())
    {
        ui->play_button->setVisible(false);
        ui->update_button->setText("Installer");
        ui->update_button->drawLine(MenuButton::None);
        connect(ui->update_button, &MenuButton::pushed, this, &MainWindow::displayUpdateWindow);
    }
    else
    {
        ui->update_button->setText("Vérification...");
        m_updater->checkInstallation();
        connect(m_updater, &Updater::result, this, &MainWindow::checkUpdateResult);
    }

    connect(ui->news_button, &MenuButton::pushed, this, &MainWindow::displayNews);
    connect(ui->news_button, &MenuButton::bumped, this, &MainWindow::closeNews);
    connect(ui->news_button, &MenuButton::pushed, ui->credits_button, &MenuButton::bump);
    connect(ui->news_button, &MenuButton::pushed, ui->saves_button, &MenuButton::bump);
    connect(ui->news_button, &MenuButton::pushed, ui->logs_button, &MenuButton::bump);
    connect(ui->news_button, &MenuButton::pushed, ui->options_button, &MenuButton::bump);

    connect(ui->credits_button, &MenuButton::pushed, this, &MainWindow::displayCredits);
    connect(ui->credits_button, &MenuButton::pushed, ui->news_button, &MenuButton::bump);
    connect(ui->credits_button, &MenuButton::bumped, this, &MainWindow::closeCredits);
    connect(ui->credits_button, &MenuButton::pushed, ui->saves_button, &MenuButton::bump);
    connect(ui->credits_button, &MenuButton::pushed, ui->logs_button, &MenuButton::bump);
    connect(ui->credits_button, &MenuButton::pushed, ui->options_button, &MenuButton::bump);

    connect(ui->saves_button, &MenuButton::pushed, this, &MainWindow::displaySaves);
    connect(ui->saves_button, &MenuButton::pushed, ui->news_button, &MenuButton::bump);
    connect(ui->saves_button, &MenuButton::pushed, ui->credits_button, &MenuButton::bump);
    connect(ui->saves_button, &MenuButton::bumped, this, &MainWindow::closeSaves);
    connect(ui->saves_button, &MenuButton::pushed, ui->logs_button, &MenuButton::bump);
    connect(ui->saves_button, &MenuButton::pushed, ui->options_button, &MenuButton::bump);

    connect(ui->logs_button, &MenuButton::pushed, this, &MainWindow::displayLogs);
    connect(ui->logs_button, &MenuButton::pushed, ui->news_button, &MenuButton::bump);
    connect(ui->logs_button, &MenuButton::pushed, ui->credits_button, &MenuButton::bump);
    connect(ui->logs_button, &MenuButton::pushed, ui->saves_button, &MenuButton::bump);
    connect(ui->logs_button, &MenuButton::bumped, this, &MainWindow::closelogs);
    connect(ui->logs_button, &MenuButton::pushed, ui->options_button, &MenuButton::bump);

    connect(ui->options_button, &MenuButton::pushed, this, &MainWindow::displayOptions);
    connect(ui->options_button, &MenuButton::pushed, ui->news_button, &MenuButton::bump);
    connect(ui->options_button, &MenuButton::pushed, ui->saves_button, &MenuButton::bump);
    connect(ui->options_button, &MenuButton::pushed, ui->credits_button, &MenuButton::bump);
    connect(ui->options_button, &MenuButton::pushed, ui->logs_button, &MenuButton::bump);
    connect(ui->options_button, &MenuButton::bumped, this, &MainWindow::closeOptions);

    connect(m_options, &Options::optionsChanged, this, &MainWindow::applyParameters);
    connect(ui->update_status, &UpdateStatus::sizeChanged, this, qOverload<>(&QWidget::update));
    connect(ui->update_status, &UpdateStatus::updateCanceled, this, &MainWindow::updateCanceled);

    connect(m_watcher, &GameWatcher::running, this, &MainWindow::checkGameRunning);

    m_backgroundId = QRandomGenerator::global()->bounded(0, 8192);
    if(m_backgroundId == 42)
    {
        changeBackground(QPixmap{":/assets/background_s.png"});
    }
    else
    {
        const auto backgrounds{m_options->availableBackground()};
        m_backgroundId = QRandomGenerator::global()->bounded(0, 3);

        if(!backgrounds[m_backgroundId])
        {
            m_backgroundId = QRandomGenerator::global()->bounded(0, 3);
            while(!backgrounds[m_backgroundId])
                m_backgroundId = (m_backgroundId + 1) % 3;
        }

        changeBackground(QPixmap{":/assets/background_" + QString::number(m_backgroundId % 3) + ".png"});
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QPixmap& MainWindow::bluredBackground() const
{
    return m_bluredBackground;
}

void MainWindow::changeBackground(const QPixmap& pixmap)
{
    m_backgroundImage = pixmap;
    repaintBackground();
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    repaintBackground();
    emit sizeChanged();
}

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.drawPixmap(rect(), m_bluredBackground, QRect{blurRadius, blurRadius, width(), height()});

    int left{}, top{}, right{}, bottom{};
    getContentsMargins(&left, &top, &right, &bottom);
    left += borderWidth;
    top += borderWidth;
    right += borderWidth;
    bottom += borderWidth;

    if(ui->update_status->isVisible())
        bottom += ui->update_status->height();

    if(ui->content->currentIndex() == ContentNone)
    {
        if(ui->quote->isVisible())
            bottom += 32;

        const QRect target{menuWidth + left, titlebarHeight + top, width() - menuWidth - left - right, height() - titlebarHeight - top - bottom};
        const QRect source{menuWidth + left + blurRadius, titlebarHeight + top + blurRadius, width() - menuWidth - left - right, height() - titlebarHeight - top - bottom};
        painter.drawPixmap(target, m_scaledBackground, source);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{"#80000000"});
    painter.drawRect(rect());

    QRadialGradient gradient{};
    gradient.setRadius(std::max(width(), height()));
    gradient.setColorAt(0.0, QColor{"#80000000"});
    gradient.setColorAt(1.0, QColor{"#00000000"});

    gradient.setFocalPoint(0, 0);
    painter.setBrush(gradient);
    painter.drawRect(rect());

    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(left + menuWidth - 1, top + titlebarHeight, left + menuWidth - 1, height() - bottom);
}

void MainWindow::repaintBackground()
{
    if(m_backgroundImage.isNull())
        return;

    m_scaledBackground = m_backgroundImage.scaled(size() + QSize{blurRadius * 2, blurRadius * 2}, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    m_bluredBackground = QPixmap{size() + QSize{blurRadius * 2, blurRadius * 2}};

    auto* effect{new QGraphicsBlurEffect};
    effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    effect->setBlurRadius(blurRadius);

    auto* item{new QGraphicsPixmapItem};
    item->setPixmap(m_scaledBackground);
    item->setGraphicsEffect(effect);

    QGraphicsScene scene{};
    scene.addItem(item);

    QPainter painter{&m_bluredBackground};
    scene.render(&painter, m_bluredBackground.rect(), m_scaledBackground.rect(), Qt::KeepAspectRatioByExpanding);

    update();
}

void MainWindow::checkUpdateResult(Updater::InstallationStatus status)
{
    if(status == Updater::Outdated)
    {
        auto palette{ui->update_button->palette()};
        palette.setColor(QPalette::ButtonText, QColor{"#00a8ff"});
        ui->update_button->setPalette(palette);

        if(m_options->autoUpdateEnable())
        {
            displayUpdateWindow();
        }
        else
        {
            ui->update_button->setText("Mise à jour");
            connect(ui->update_button, &MenuButton::pushed, this, &MainWindow::displayUpdateWindow);
        }
    }
    else if(status == Updater::UpToDate)
    {
        ui->play_button->setVisible(true);
        ui->update_button->setVisible(true);
        ui->update_button->drawLine(MenuButton::Top);
        ui->update_button->setText("Vous êtes à jour !");
        ui->update_button->setPixmap(QPixmap{":/assets/check_mark.png"});
        ui->news_button->drawLine(MenuButton::Top);
    }
    else if(status == Updater::Error)
    {
        ui->update_button->setVisible(true);
        ui->update_button->setText("Déconnecté");
        ui->news_button->drawLine(MenuButton::Top);
    }
}

void MainWindow::updateResult(Updater::InstallationStatus status)
{
    if(status == Updater::UpToDate)
    {
        auto palette{ui->update_button->palette()};
        palette.setColor(QPalette::ButtonText, QColor{"#ffffff"});
        ui->update_button->setPalette(palette);

        ui->play_button->setVisible(true);
        ui->update_button->setVisible(true);
        ui->update_button->drawLine(MenuButton::Top);
        ui->update_button->setText("Vous êtes à jour !");
        ui->update_button->setPixmap(QPixmap{":/assets/check_mark.png"});
        ui->news_button->drawLine(MenuButton::Top);

        const auto version{m_updater->getGameVersion()};
        ui->game_version->setText(version == "0.0.0" ? "" : version);

        m_credits->refresh();
    }
    else if(status == Updater::Error)
    {
        ui->update_button->setVisible(true);
        ui->update_button->setText("Réessayer");
        ui->update_button->drawLine(MenuButton::None);
        ui->news_button->drawLine(MenuButton::Top);

        disconnect(m_updater, &Updater::result, this, &MainWindow::updateResult);
        disconnect(m_updater, &Updater::result, ui->update_status, &UpdateStatus::result);
        disconnect(m_updater, &Updater::stepChanged, ui->update_status, &UpdateStatus::stepChanged);
        disconnect(m_updater, &Updater::statusChanged, ui->update_status, &UpdateStatus::statusChanged);
        disconnect(m_updater, &Updater::error, ui->update_status, &UpdateStatus::error);

        connect(ui->update_button, &MenuButton::pushed, this, &MainWindow::displayUpdateWindow);
    }
}

void MainWindow::switchMode()
{
    isMaximized() ? showNormal() : showMaximized();
}

void MainWindow::applyParameters()
{
    const auto backgrounds{m_options->availableBackground()};

    if(!backgrounds[m_backgroundId])
    {
        m_backgroundId = QRandomGenerator::global()->bounded(0, 3);
        while(!backgrounds[m_backgroundId])
            m_backgroundId = (m_backgroundId + 1) % 3;

        changeBackground(QPixmap{":/assets/background_" + QString::number(m_backgroundId % 3) + ".png"});
    }
}

void MainWindow::updateCanceled()
{
    if(m_updater->running())
    {
        m_updater->cancelUpdate();
        connect(ui->update_button, &MenuButton::pushed, this, &MainWindow::displayUpdateWindow);
    }

    disconnect(m_updater, &Updater::result, this, &MainWindow::updateResult);
    disconnect(m_updater, &Updater::result, ui->update_status, &UpdateStatus::result);
    disconnect(m_updater, &Updater::stepChanged, ui->update_status, &UpdateStatus::stepChanged);
    disconnect(m_updater, &Updater::statusChanged, ui->update_status, &UpdateStatus::statusChanged);
    disconnect(m_updater, &Updater::error, ui->update_status, &UpdateStatus::error);

    if(m_updater->isInstalled())
    {
        ui->play_button->setVisible(true);
        ui->update_button->drawLine(MenuButton::Top);
    }

    ui->update_button->setVisible(true);
    ui->news_button->drawLine(MenuButton::Top);
    ui->update_status->setVisible(false);
    ui->quote->setVisible(true);

    repaintBackground();
}

void MainWindow::displayUpdateWindow()
{
    disconnect(ui->update_button, &MenuButton::pushed, this, &MainWindow::displayUpdateWindow);

    connect(m_updater, &Updater::result, this, &MainWindow::updateResult);
    connect(m_updater, &Updater::result, ui->update_status, &UpdateStatus::result);
    connect(m_updater, &Updater::stepChanged, ui->update_status, &UpdateStatus::stepChanged);
    connect(m_updater, &Updater::statusChanged, ui->update_status, &UpdateStatus::statusChanged);
    connect(m_updater, &Updater::error, ui->update_status, &UpdateStatus::error);

    m_updater->runUpdate();

    ui->play_button->setVisible(false);
    ui->update_button->setVisible(false);
    ui->news_button->drawLine(MenuButton::None);
    ui->update_status->reset();
    ui->update_status->setVisible(true);
    ui->quote->setVisible(false);

    repaintBackground();
}

void MainWindow::launchGame()
{
    nes::process{std::string{origins::game_directory} + "/Pokémon Origins.exe", origins::game_directory}.detach();

    if(m_options->autoCloseEnable())
        QApplication::exit();
}

void MainWindow::checkGameRunning(bool running)
{
    if(m_updater->isInstalled())
    {
        if(running)
        {
            ui->play_button->setText("Le jeu est lancé !");
        }
        else
        {
            ui->play_button->setText("Jouer");
        }
    }
}

void MainWindow::displayNews()
{
    if(ui->content->currentIndex() != ContentNews)
    {
        ui->content->setCurrentIndex(ContentNews);
        update();
    }
}

void MainWindow::closeNews()
{
    if(ui->content->currentIndex() == ContentNews)
    {
        ui->content->setCurrentIndex(ContentNone);
        update();
    }
}

void MainWindow::displayCredits()
{
    if(ui->content->currentIndex() != ContentCredits)
    {
        ui->content->setCurrentIndex(ContentCredits);
        update();
    }
}

void MainWindow::closeCredits()
{
    if(ui->content->currentIndex() == ContentCredits)
    {
        ui->content->setCurrentIndex(ContentNone);
        update();
    }
}

void MainWindow::displaySaves()
{
    if(ui->content->currentIndex() != ContentSaves)
    {
        ui->content->setCurrentIndex(ContentSaves);
        update();
    }
}

void MainWindow::closeSaves()
{
    if(ui->content->currentIndex() == ContentSaves)
    {
        ui->content->setCurrentIndex(ContentNone);
        update();
    }
}

void MainWindow::displayLogs()
{
    if(ui->content->currentIndex() != ContentLogs)
    {
        ui->content->setCurrentIndex(ContentLogs);
        update();
    }
}

void MainWindow::closelogs()
{
    if(ui->content->currentIndex() == ContentLogs)
    {
        ui->content->setCurrentIndex(ContentNone);
        update();
    }
}

void MainWindow::displayOptions()
{
    if(ui->content->currentIndex() != ContentOptions)
    {
        ui->content->setCurrentIndex(ContentOptions);
        update();
    }
}

void MainWindow::closeOptions()
{
    if(ui->content->currentIndex() == ContentOptions)
    {
        ui->content->setCurrentIndex(ContentNone);
        update();
    }
}

bool MainWindow::nativeEvent(const QByteArray&, void* message, long* result)
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

        RECT winrect{};
        GetWindowRect(reinterpret_cast<HWND>(winId()), &winrect);

        const LONG x{GET_X_LPARAM(msg->lParam)};
        const LONG y{GET_Y_LPARAM(msg->lParam)};

        const bool resizeWidth{minimumWidth() != maximumWidth()};
        const bool resizeHeight{minimumHeight() != maximumHeight()};

        if(resizeWidth)
        {
            if(x >= winrect.left && x < winrect.left + borderWidth)
                *result = HTLEFT;

            if(x < winrect.right && x >= winrect.right - borderWidth)
                *result = HTRIGHT;
        }
        if(resizeHeight)
        {
            if(y < winrect.bottom && y >= winrect.bottom - borderWidth)
                *result = HTBOTTOM;

            if(y >= winrect.top && y < winrect.top + borderWidth)
                *result = HTTOP;
        }
        if(resizeWidth && resizeHeight)
        {
            if(x >= winrect.left && x < winrect.left + borderWidth && y < winrect.bottom && y >= winrect.bottom - borderWidth)
                *result = HTBOTTOMLEFT;

            if(x < winrect.right && x >= winrect.right - borderWidth && y < winrect.bottom && y >= winrect.bottom - borderWidth)
                *result = HTBOTTOMRIGHT;

            if(x >= winrect.left && x < winrect.left + borderWidth && y >= winrect.top && y < winrect.top + borderWidth)
                *result = HTTOPLEFT;

            if(x < winrect.right && x >= winrect.right - borderWidth && y >= winrect.top && y < winrect.top + borderWidth)
                *result = HTTOPRIGHT;
        }

        if(*result)
            return true;

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

    if(msg->message == WM_GETMINMAXINFO)
    {
        if(IsZoomed(msg->hwnd))
        {
            RECT frame{};
            AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);

            const double dpr{devicePixelRatioF()};

            QMargins margins{};
            margins.setLeft(static_cast<int>(std::max<int>(0, frame.left - borderWidth) / dpr));
            margins.setTop(static_cast<int>(std::max<int>(0, frame.bottom - borderWidth) / dpr));
            margins.setRight(static_cast<int>(std::max<int>(0, frame.right - borderWidth) / dpr));
            margins.setBottom(static_cast<int>(std::max<int>(0, frame.bottom - borderWidth) / dpr));

            setContentsMargins(margins.left(), margins.top(), margins.right(), margins.bottom());

            ui->maximize_button->setType(SystemButton::Restore);
            m_newMaximize = true;
        }
        else
        {
            if(m_newMaximize)
            {
                setContentsMargins(0, 0, 0, 0);

                ui->maximize_button->setType(SystemButton::Maximize);
                m_newMaximize = false;
            }
        }
    }
#endif

    return false;
}

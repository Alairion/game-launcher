#include "options.hpp"
#include "ui_options.h"

#include <fstream>

#include <QPainter>

#include <origins/updater.hpp>

Options::Options(QWidget *parent)
:QWidget{parent}
,ui{new Ui::Options}
,m_gameOptionsTimer{new QTimer{this}}
,m_launcherOptionsTimer{new QTimer{this}}
{
    ui->setupUi(this);

    m_gameOptionsTimer->setSingleShot(true);
    m_gameOptionsTimer->setInterval(1000);

    m_launcherOptionsTimer->setSingleShot(true);
    m_launcherOptionsTimer->setInterval(1000);

    connect(ui->backup_groupbox, &QGroupBox::toggled, [this](bool checked)
    {
        if(checked)
        {
            ui->backup_delay_spinbox->setVisible(true);
            ui->backup_maximum_spinbox->setVisible(true);
        }
        else
        {
            ui->backup_delay_spinbox->setVisible(false);
            ui->backup_maximum_spinbox->setVisible(false);
        }
    });

    connect(ui->se_volume_slider, &QSlider::valueChanged, [this](int value){ui->se_volume_value->setText(QString::number(value) + "%");});
    connect(ui->me_volume_slider, &QSlider::valueChanged, [this](int value){ui->me_volume_value->setText(QString::number(value) + "%");});
    connect(ui->bgs_volume_slider, &QSlider::valueChanged, [this](int value){ui->bgs_volume_value->setText(QString::number(value) + "%");});
    connect(ui->bgm_volume_slider, &QSlider::valueChanged, [this](int value){ui->bgm_volume_value->setText(QString::number(value) + "%");});

    setupGameOptions();
    setupLauncherOptions();

    connect(m_gameOptionsTimer, &QTimer::timeout, [this]{writeGameOptions();});
    connect(m_launcherOptionsTimer, &QTimer::timeout, [this]{writeLauncherOptions();});

    connect(ui->backup_groupbox, &QGroupBox::clicked, [this](bool){m_gameOptionsTimer->start();});
    connect(ui->backup_delay_spinbox, qOverload<int>(&QSpinBox::valueChanged), [this](int){m_gameOptionsTimer->start();});
    connect(ui->backup_maximum_spinbox, qOverload<int>(&QSpinBox::valueChanged), [this](int){m_gameOptionsTimer->start();});
    connect(ui->se_volume_slider, &QSlider::valueChanged, [this](int){m_gameOptionsTimer->start();});
    connect(ui->me_volume_slider, &QSlider::valueChanged, [this](int){m_gameOptionsTimer->start();});
    connect(ui->bgs_volume_slider, &QSlider::valueChanged, [this](int){m_gameOptionsTimer->start();});
    connect(ui->bgm_volume_slider, &QSlider::valueChanged, [this](int){m_gameOptionsTimer->start();});

    connect(ui->auto_update_checkbox, &QCheckBox::toggled, [this](bool){changeLauncherOptions();});
    connect(ui->auto_close, &QCheckBox::toggled, [this](bool){changeLauncherOptions();});
    connect(ui->swamp_checkbox, &QCheckBox::toggled, [this](bool){changeLauncherOptions();});
    connect(ui->forest_checkbox, &QCheckBox::toggled, [this](bool){changeLauncherOptions();});
    connect(ui->mountain_checkbox, &QCheckBox::toggled, [this](bool){changeLauncherOptions();});
}

Options::~Options()
{
    if(m_gameOptionsTimer->isActive())
        writeGameOptions();
    if(m_launcherOptionsTimer->isActive())
        writeLauncherOptions();

    delete ui;
}

void Options::setScrollBar(QScrollBar* scrollbar)
{
    ui->area->setVerticalScrollBar(scrollbar);
}

std::bitset<32> Options::availableBackground() const
{
    return std::bitset<32>
    {
        (static_cast<std::uint32_t>(ui->swamp_checkbox->isChecked()) << 0)
      | (static_cast<std::uint32_t>(ui->forest_checkbox->isChecked()) << 1)
      | (static_cast<std::uint32_t>(ui->mountain_checkbox->isChecked()) << 2)
    };
}

bool Options::autoUpdateEnable() const
{
    return ui->auto_update_checkbox->isChecked();
}

bool Options::autoCloseEnable() const
{
    return ui->auto_close->isChecked();
}

void Options::writeGameOptions()
{
    std::ofstream ofs{std::string{origins::game_directory} + "/options.txt", std::ios_base::binary};
    if(!ofs)
        return;

    ofs << "se_volume = " << ui->se_volume_slider->value() << '\n';
    ofs << "me_volume = " << ui->me_volume_slider->value() << '\n';
    ofs << "bgs_volume = " << ui->bgs_volume_slider->value() << '\n';
    ofs << "bgm_volume = " << ui->bgm_volume_slider->value() << '\n';
    ofs << "enable_backup = " << std::boolalpha << ui->backup_groupbox->isChecked() << '\n';
    ofs << "backup_minimum_delay = " << ui->backup_delay_spinbox->value() << '\n';
    ofs << "backup_maximum_count = " << ui->backup_maximum_spinbox->value() << std::endl;
}

void Options::setupGameOptions()
{
    std::ifstream ifs{std::string{origins::game_directory} + "/options.txt", std::ios_base::binary};
    if(!ifs)
        return;

    std::string variable_name{};
    std::string variable_value{};
    while(ifs)
    {
        ifs >> variable_name;
        ifs.seekg(3, std::ios_base::cur);
        std::getline(ifs, variable_value);

        if(variable_name == "se_volume")
        {
            ui->se_volume_slider->setValue(std::stoi(variable_value));
        }
        else if(variable_name == "me_volume")
        {
            ui->me_volume_slider->setValue(std::stoi(variable_value));
        }
        else if(variable_name == "bgs_volume")
        {
            ui->bgs_volume_slider->setValue(std::stoi(variable_value));
        }
        else if(variable_name == "bgm_volume")
        {
            ui->bgm_volume_slider->setValue(std::stoi(variable_value));
        }
        else if(variable_name == "enable_backup")
        {
            if(variable_value == "true")
                ui->backup_groupbox->setChecked(true);
            else
                ui->backup_groupbox->setChecked(false);
        }
        else if(variable_name == "backup_minimum_delay")
        {
            ui->backup_delay_spinbox->setValue(std::stoi(variable_value));
        }
        else if(variable_name == "backup_maximum_count")
        {
            ui->backup_maximum_spinbox->setValue(std::stoi(variable_value));
        }
    }
}

void Options::writeLauncherOptions()
{
    std::ofstream ofs{"options.txt", std::ios_base::binary};
    if(!ofs)
        return;

    ofs << "auto_update = " << std::boolalpha << ui->auto_update_checkbox->isChecked() << std::endl;
    ofs << "auto_close = " << std::boolalpha << ui->auto_close->isChecked() << std::endl;
    ofs << "backgrounds = " << availableBackground().to_ulong() << '\n';
}

void Options::changeLauncherOptions()
{
    const auto backgrounds{availableBackground()};
    if(backgrounds.count() == 1)
    {
        if(backgrounds[0])
            ui->swamp_checkbox->setEnabled(false);
        else if(backgrounds[1])
            ui->forest_checkbox->setEnabled(false);
        else if(backgrounds[2])
            ui->mountain_checkbox->setEnabled(false);
    }
    else
    {
        ui->swamp_checkbox->setEnabled(true);
        ui->forest_checkbox->setEnabled(true);
        ui->mountain_checkbox->setEnabled(true);
    }

    m_launcherOptionsTimer->start();
    emit optionsChanged();
}

void Options::setupLauncherOptions()
{
    std::ifstream ifs{"options.txt", std::ios_base::binary};
    if(!ifs)
        return;

    std::string variable_name{};
    std::string variable_value{};
    while(ifs)
    {
        ifs >> variable_name;
        ifs.seekg(3, std::ios_base::cur);
        std::getline(ifs, variable_value);

        if(variable_name == "auto_update")
        {
            if(variable_value == "true")
                ui->auto_update_checkbox->setChecked(true);
            else
                ui->auto_update_checkbox->setChecked(false);
        }
        else if(variable_name == "auto_close")
        {
            if(variable_value == "true")
                ui->auto_close->setChecked(true);
            else
                ui->auto_close->setChecked(false);
        }
        else if(variable_name == "backgrounds")
        {
            const std::bitset<32> backgrounds{std::stoul(variable_value)};

            ui->swamp_checkbox->setChecked(backgrounds[0]);
            ui->forest_checkbox->setChecked(backgrounds[1]);
            ui->mountain_checkbox->setChecked(backgrounds[2]);

            if(backgrounds.count() == 1)
            {
                if(backgrounds[0])
                    ui->swamp_checkbox->setEnabled(false);
                else if(backgrounds[1])
                    ui->forest_checkbox->setEnabled(false);
                else if(backgrounds[2])
                    ui->mountain_checkbox->setEnabled(false);
            }
        }
    }
}

void Options::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{"#20000000"});
    painter.drawRect(rect());
}

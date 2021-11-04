#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <bitset>

#include <QWidget>
#include <QScrollBar>
#include <QTimer>

namespace Ui {
class Options;
}

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = nullptr);
    ~Options();

    void setScrollBar(QScrollBar* scrollbar);
    std::bitset<32> availableBackground() const;
    bool autoUpdateEnable() const;
    bool autoCloseEnable() const;

signals:
    void optionsChanged();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private slots:
    void writeGameOptions();
    void setupGameOptions();
    void writeLauncherOptions();
    void changeLauncherOptions();
    void setupLauncherOptions();

private:
    Ui::Options *ui;
    QTimer* m_gameOptionsTimer{};
    QTimer* m_launcherOptionsTimer{};
};

#endif // OPTIONS_HPP

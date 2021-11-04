#ifndef UPDATEDIALOG_HPP
#define UPDATEDIALOG_HPP

#include <QDialog>
#include <QThread>
#include <QPixmap>

#include <nes/named_mutex.hpp>

#include "launcherupdater.hpp"

namespace Ui
{
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

private:
    static constexpr long borderWidth{3};
    static constexpr int blurRadius{16};
    static constexpr int titlebarHeight{32};

public:
    explicit UpdateDialog(nes::named_mutex& mutex, QWidget* parent = nullptr);
    ~UpdateDialog();

    void setText(const QString& text);
    void setTitle(const QString& title);
    void changeBackground(const QPixmap& pixmap);

protected:
    virtual void resizeEvent(QResizeEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;
    virtual bool nativeEvent(const QByteArray&, void *message, long *result) override;

private slots:
    void repaintBackground();
    void startUpdate();
    void result(Updater::InstallationStatus status);
    void stepChanged(origins::step_changed step);
    void error(const QString& message);

private:
    Ui::UpdateDialog *ui;
    nes::named_mutex* m_mutex;
    LauncherUpdater* m_updater{};
    std::uint32_t m_backgroundId{};
    QPixmap m_backgroundImage{}; //The image
    QPixmap m_bluredBackground{}; //The image which size == size() but blured
    std::vector<QWidget*> m_treatAsTitlebar{};
};

#endif // UPDATEDIALOG_HPP

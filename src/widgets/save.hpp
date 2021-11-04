#ifndef SAVE_HPP
#define SAVE_HPP

#include <QWidget>
#include <QTimer>

namespace Ui
{
class Save;
}

class Save : public QWidget
{
    Q_OBJECT

    static constexpr int buttonWidth{224};

public:
    explicit Save(QWidget* parent = nullptr);
    ~Save();

    void setFile(const QString& path);
    bool isFilled() const;

signals:
    void removed();
    void filled();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private slots:
    void importSave();
    void copySave();
    void restoreBackup();
    void confirmDelete();
    void endDelete();

    void restoreCopyButtonText();
    void restoreRestoreButtonText();
    void cancelDelete();

private:
    Ui::Save* ui;
    QTimer* m_copyTimer{};
    QTimer* m_restoreTimer{};
    QTimer* m_deleteTimer{};
    QString m_file{};
    std::uint64_t m_hash{};
};

#endif // SAVE_HPP

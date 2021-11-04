#ifndef SAVES_HPP
#define SAVES_HPP

#include <QWidget>

class Save;

namespace Ui
{
class Saves;
}

void shiftSaves();

class Saves : public QWidget
{
    Q_OBJECT

public:
    explicit Saves(QWidget* parent = nullptr);
    ~Saves();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private slots:
    void slotRemoved();
    void slotFilled();

private:
    void showSaves();

private:
    Ui::Saves *ui;
    QTimer* m_timer{};
    std::array<Save*, 3> m_saves{};
};

#endif // SAVES_HPP

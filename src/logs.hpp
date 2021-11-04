#ifndef LOGS_HPP
#define LOGS_HPP

#include <QWidget>

namespace Ui
{
class Logs;
}

class Logs : public QWidget
{
    Q_OBJECT

public:
    explicit Logs(QWidget *parent = nullptr);
    ~Logs();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    Ui::Logs *ui;
};

#endif // LOGS_HPP

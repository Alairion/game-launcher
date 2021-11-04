#ifndef LOG_HPP
#define LOG_HPP

#include <QWidget>

namespace Ui
{
class Log;
}

class Log : public QWidget
{
    Q_OBJECT

public:
    explicit Log(QWidget *parent = nullptr);
    ~Log();

private:
    Ui::Log *ui;
};

#endif // LOG_HPP

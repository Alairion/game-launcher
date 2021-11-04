#ifndef GAMEWATCHER_HPP
#define GAMEWATCHER_HPP

#include <QObject>
#include <QTimer>

#include <nes/named_mutex.hpp>

class GameWatcher : public QObject
{
    Q_OBJECT

public:
    GameWatcher();

signals:
    void running(bool running);

private:
    nes::named_mutex m_mutex;
    QTimer* m_timer{};
};

#endif // GAMEWATCHER_HPP

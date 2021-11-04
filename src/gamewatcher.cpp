#include "gamewatcher.hpp"

#include <mutex>

#include <origins/updater.hpp>
#include <origins/localization.hpp>

GameWatcher::GameWatcher()
:m_mutex{origins::localize(origins::game_directory) + origins::game_mutex_name}
,m_timer{new QTimer{this}}
{
    connect(m_timer, &QTimer::timeout, [this]
    {
        std::unique_lock lock{m_mutex, std::try_to_lock};
        emit running(!lock.owns_lock());
    });

    m_timer->start(1000);
}

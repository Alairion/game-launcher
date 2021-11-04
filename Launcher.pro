#-------------------------------------------------
#
# Project created by QtCreator 2019-08-21T20:07:26
#
#-------------------------------------------------

QT += core gui sql network
QTPLUGIN += QSQLMYSQL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = launcher
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3 -s

SOURCES += \
        src/credits.cpp \
        src/gamewatcher.cpp \
        src/launcherupdater.cpp \
        src/logs.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/news.cpp \
        src/options.cpp \
        src/saves.cpp \
        src/titlebar.cpp \
        src/updatedialog.cpp \
        src/updater.cpp \
        src/updatestatus.cpp \
        src/widgets/article.cpp \
        src/widgets/log.cpp \
        src/widgets/menubutton.cpp \
        src/widgets/minimalwebbrowser.cpp \
        src/widgets/progressbar.cpp \
        src/widgets/save.cpp \
        src/widgets/scrollbar.cpp \
        src/widgets/systembutton.cpp \
        src/widgets/textbutton.cpp

HEADERS += \
        src/credits.hpp \
        src/gamewatcher.hpp \
        src/launcherupdater.hpp \
        src/logs.hpp \
        src/mainwindow.hpp \
        src/news.hpp \
        src/options.hpp \
        src/saves.hpp \
        src/titlebar.hpp \
        src/updatedialog.hpp \
        src/updater.hpp \
        src/updatestatus.hpp \
        src/widgets/article.hpp \
        src/widgets/log.hpp \
        src/widgets/menubutton.hpp \
        src/widgets/minimalwebbrowser.hpp \
        src/widgets/progressbar.hpp \
        src/widgets/save.hpp \
        src/widgets/scrollbar.hpp \
        src/widgets/systembutton.hpp \
        src/widgets/textbutton.hpp

FORMS += \
        ui/article.ui \
        ui/credits.ui \
        ui/log.ui \
        ui/logs.ui \
        ui/mainwindow.ui \
        ui/news.ui \
        ui/options.ui \
        ui/save.ui \
        ui/saves.ui \
        ui/updatedialog.ui \
        ui/updatestatus.ui

INCLUDEPATH += ../origins_base/include \
               ../not_enough_standards/include

win32: LIBS += -ldwmapi
win32: RC_FILE += resources.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

CONFIG(release, debug|release) DESTDIR = $$PWD/test/launcher_test
CONFIG(debug, debug|release) DESTDIR = $$PWD/test/launcher_test

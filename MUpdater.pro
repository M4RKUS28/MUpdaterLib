QT       += core gui widgets network

TEMPLATE = lib
CONFIG += staticlib
#CONFIG += shared_and_static build_all

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/dialogueber.cpp \
    src/flowlayout.cpp \
    src/mupdater.cpp \
    src/stylehandler.cpp \
    src/switch.cpp \
    src/wakeuplistener.cpp

HEADERS += \
    src/dialogueber.h \
    src/flowlayout.h \
    src/mupdater.h \
    src/style.h \
    src/stylehandler.h \
    src/switch.h \
    src/wakeuplistener.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    src/dialogueber.ui

RESOURCES += \
    src/res/darkstyleicons.qrc \
    src/res/themes_icon.qrc \
    src/res/ressources.qrc

# Application Icon
win32:RC_ICONS += icons/appicon.ico

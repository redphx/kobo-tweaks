#pragma once
#include <QWidget>

#include <NickelHook.h>

#ifndef ADDON_VERSION
    #define ADDON_VERSION "1.0.0"
#endif

#ifndef DATA_DIR
    #define DATA_DIR "/mnt/onboard/.adds/tweaks"
#endif

#ifndef IMAGES_DIR
    #define IMAGES_DIR DATA_DIR "/images"
#endif

#ifndef KOBO_TWEAKS_DELETE_FILE
	#define KOBO_TWEAKS_DELETE_FILE DATA_DIR "/uninstall"
#endif

#ifndef KOBO_TWEAKS_INSTALL_FILE
	#define KOBO_TWEAKS_INSTALL_FILE DATA_DIR "/DELETE_TO_UNINSTALL.txt"
#endif

enum class ReadingFooterPosition { Header, Footer };

typedef QWidget ReadingView;
typedef QWidget GestureReceivingContainer;
typedef QWidget ReadingFooter;
typedef QObject HardwareInterface;
typedef void SearchAutoCompleteController;

class Volume {
public:
    virtual ~Volume() {}
};

using GetBatteryLevelFn = int(*)(HardwareInterface*);
using ChargingStateFn = uint(*)(HardwareInterface*);

extern void (*ReadingView_constructor)(ReadingView* self);
extern void (*ReadingFooter_setFooterMargin)(QWidget* self, int margin);
extern void (*SearchAutoCompleteController_handleSpecialCommands)(SearchAutoCompleteController* self, const QString& command);
extern void (*ConfirmationDialogFactory_showOKDialog)(QString const& title, QString const& body);
extern QWidget* (*DogEarDelegate_constructor)(QWidget* self, QWidget* parent, const QString& image);
extern void (*Content_getTitle)(QString* result, const Volume* self);

extern HardwareInterface* (*HardwareFactory_sharedInstance)();
extern uintptr_t** HardwareInterface_vtable;
extern int (*HardwareInterface_getBatteryLevel)(HardwareInterface* self);
extern uint (*HardwareInterface_chargingState)(HardwareInterface* self);

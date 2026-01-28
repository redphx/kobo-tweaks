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
typedef QWidget MediumVertSpacer;
typedef QObject HardwareInterface;
typedef void SearchAutoCompleteController;
typedef void ReadingSettings;
typedef void ReadingStats;
typedef QObject BrightnessEventFilter;

class Content { public: virtual ~Content() {} };
class Volume : public Content { public: virtual ~Volume() {} };
class Shortcover : public Content { public: virtual ~Shortcover() {} };

using GetBatteryLevelFn = int(*)(HardwareInterface*);
using ChargingStateFn = uint(*)(HardwareInterface*);

extern void (*ReadingView_constructor)(ReadingView* self);
extern void (*ReadingView_getChapterTitle)(QString* result, ReadingView* self);
extern int (*ReadingView_chapterCurrentPage)(ReadingView* self);
extern int (*ReadingView_chapterTotalPages)(ReadingView* self);
extern int (*ReadingView_fullBookCurrentPage)(ReadingView* self);
extern int (*ReadingView_fullBookTotalPages)(ReadingView* self);
extern bool (*ReadingView_hasValidReadingStats)(ReadingView* self);
extern ReadingStats* (*ReadingView_readingStats)(ReadingStats* result, ReadingView* self);
extern int (*ReadingView_getCalculatedReadProgress)(ReadingView* self);
extern int (*ReadingStats_currentChapterEstimate)(ReadingStats* self);
extern int (*ReadingStats_restOfBookEstimate)(ReadingStats* self);
extern void (*ReadingStats_deconstructor)(ReadingStats* self);
extern void (*Content_getTitle)(QString* result, const Content* self);

extern int (*ReadingSettings_getBookProgressType)(ReadingSettings* self);
extern int (*ReadingSettings_getChapterProgressType)(ReadingSettings* self);

extern void (*ReadingFooter_setFooterMargin)(QWidget* self, int margin);
extern void (*SearchAutoCompleteController_handleSpecialCommands)(SearchAutoCompleteController* self, const QString& command);
extern void (*ConfirmationDialogFactory_showOKDialog)(const QString& title, const QString& body);
extern void (*DogEarDelegate_constructor)(QWidget* self, QWidget* parent, const QString& image);
extern void (*BrightnessEventFilter_updateBrightnessHeader)(BrightnessEventFilter* self, const QString& text, const QString& sth);

extern void* (*MainWindowController_sharedInstance)();
extern QWidget* (*MainWindowController_currentView)(void*);

extern HardwareInterface* (*HardwareFactory_sharedInstance)();
extern uintptr_t** HardwareInterface_vtable;
extern int (*HardwareInterface_getBatteryLevel)(HardwareInterface* self);
extern uint (*HardwareInterface_chargingState)(HardwareInterface* self);

// libadobe
typedef void PluginState;
extern void (*AdobeReader_constructor)(QWidget* self, QWidget* parent, PluginState* state, const QString& image);

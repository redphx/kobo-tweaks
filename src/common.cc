#include "common.h"

void (*ReadingView_constructor)(ReadingView* self) = nullptr;
void (*ReadingView_getChapterTitle)(QString* result, ReadingView* self) = nullptr;
int (*ReadingView_chapterCurrentPage)(ReadingView* self) = nullptr;
int (*ReadingView_chapterTotalPages)(ReadingView* self) = nullptr;
int (*ReadingView_fullBookCurrentPage)(ReadingView* self) = nullptr;
int (*ReadingView_fullBookTotalPages)(ReadingView* self) = nullptr;
bool (*ReadingView_hasValidReadingStats)(ReadingView* self) = nullptr;
ReadingStats* (*ReadingView_readingStats)(ReadingStats* result, ReadingView* self) = nullptr;
int (*ReadingView_getCalculatedReadProgress)(ReadingView* self) = nullptr;
int (*ReadingStats_currentChapterEstimate)(ReadingStats* self) = nullptr;
int (*ReadingStats_restOfBookEstimate)(ReadingStats* self) = nullptr;
void (*ReadingStats_deconstructor)(ReadingStats* self) = nullptr;
void (*Content_getTitle)(QString* result, const Content* self) = nullptr;

int (*ReadingSettings_getBookProgressType)(ReadingSettings* self) = nullptr;
int (*ReadingSettings_getChapterProgressType)(ReadingSettings* self) = nullptr;

void (*ReadingFooter_setFooterMargin)(QWidget* self, int margin) = nullptr;
void (*ReadingView_pageChanged)(ReadingView* self, int pageIndex) = nullptr;
void (*SearchAutoCompleteController_handleSpecialCommands)(SearchAutoCompleteController* self, const QString& command) = nullptr;

void (*ConfirmationDialogFactory_showOKDialog)(const QString& title, const QString& body) = nullptr;
void (*DogEarDelegate_constructor)(QWidget* self, QWidget* parent, const QString& image) = nullptr;
void (*BrightnessEventFilter_updateBrightnessHeader)(BrightnessEventFilter* self, const QString& text, const QString& sth) = nullptr;

void* (*MainWindowController_sharedInstance)() = nullptr;
QWidget* (*MainWindowController_currentView)(void*) = nullptr;

HardwareInterface* (*HardwareFactory_sharedInstance)() = nullptr;
uintptr_t** HardwareInterface_vtable = nullptr;
int (*HardwareInterface_getBatteryLevel)(HardwareInterface* self) = nullptr;
uint (*HardwareInterface_chargingState)(HardwareInterface* self) = nullptr;

// libadobe
void (*AdobeReader_constructor)(QWidget* self, QWidget* parent, PluginState* state, const QString& image) = nullptr;

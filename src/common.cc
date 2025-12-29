#include "common.h"

void (*ReadingView_constructor)(ReadingView* self) = nullptr;
int (*ReadingView_chapterIndex)(ReadingView* self) = nullptr;
int (*ReadingView_mergedTOCAndSpineIndex)(ReadingView* self) = nullptr;
void (*ReadingView_shortcoverWithChapterIndex)(Shortcover* result, ReadingView* self, uint chapterIndex) = nullptr;
void (*Content_getTitle)(QString* result, const Content* self) = nullptr;

void (*ReadingFooter_setFooterMargin)(QWidget* self, int margin) = nullptr;
void (*ReadingView_pageChanged)(ReadingView* self, int pageIndex) = nullptr;
void (*SearchAutoCompleteController_handleSpecialCommands)(SearchAutoCompleteController* self, const QString& command) = nullptr;
void (*ConfirmationDialogFactory_showOKDialog)(QString const& title, QString const& body) = nullptr;
QWidget* (*DogEarDelegate_constructor)(QWidget* self, QWidget* parent, const QString& image) = nullptr;

HardwareInterface* (*HardwareFactory_sharedInstance)() = nullptr;
uintptr_t** HardwareInterface_vtable = nullptr;
int (*HardwareInterface_getBatteryLevel)(HardwareInterface* self) = nullptr;
uint (*HardwareInterface_chargingState)(HardwareInterface* self) = nullptr;

#include "common.h"

ReadingView* (*ReadingView_constructor)(ReadingView* self) = nullptr;
void (*ReadingFooter_setFooterMargin)(QWidget* self, int margin) = nullptr;
void (*ReadingView_pageChanged)(ReadingView* self, int pageIndex) = nullptr;
void (*SearchAutoCompleteController_handleSpecialCommands)(SearchAutoCompleteController* self, const QString& command) = nullptr;
void (*ConfirmationDialogFactory_showOKDialog)(QString const& title, QString const& body) = nullptr;
QWidget* (*DogEarDelegate_constructor)(QWidget* self, QWidget* parent, const QString& image) = nullptr;

HardwareInterface* (*HardwareFactory_sharedInstance)() = nullptr;
int (*MTK_getBatteryLevel)(HardwareInterface* self) = 0;
uint (*MTK_chargingState)(HardwareInterface* self) = 0;
int (*iMX508_getBatteryLevel)(HardwareInterface* self) = 0;
uint (*iMX508_chargingState)(HardwareInterface* self) = 0;
int (*AllWinner_getBatteryLevel)(HardwareInterface* self) = 0;
uint (*AllWinner_chargingState)(HardwareInterface* self) = 0;

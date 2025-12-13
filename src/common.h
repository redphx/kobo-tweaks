#pragma once

#include <QWidget>

#ifndef DATA_DIR
    #define DATA_DIR "/mnt/onboard/.adds/tweaks"
#endif

#ifndef IMAGES_DIR
    #define IMAGES_DIR DATA_DIR "/images"
#endif

#ifndef KOBO_TWEAKS_DELETE_FILE
	#define KOBO_TWEAKS_DELETE_FILE DATA_DIR "/uninstall"
#endif

enum class ReadingFooterPosition { Header, Footer };

typedef QWidget ReadingView;
typedef QWidget ReadingFooter;
typedef QObject HardwareInterface;
typedef void SearchAutoCompleteController;

using GetBatteryLevelFn = int(*)(HardwareInterface*);
using ChargingStateFn = uint(*)(HardwareInterface*);

extern ReadingView* (*ReadingView_constructor)(ReadingView* self);
extern void (*ReadingFooter_setFooterMargin)(QWidget* self, int margin);
extern void (*ReadingView_pageChanged)(ReadingView* self, int pageIndex);
extern void (*SearchAutoCompleteController_handleSpecialCommands)(SearchAutoCompleteController* self, const QString& command);
extern void (*ConfirmationDialogFactory_showOKDialog)(QString const& title, QString const& body);
extern QWidget* (*DogEarDelegate_constructor)(QWidget* self, QWidget* parent, const QString& image);

extern HardwareInterface* (*HardwareFactory_sharedInstance)();
extern int (*MTK_getBatteryLevel)(HardwareInterface* self);
extern uint (*MTK_chargingState)(HardwareInterface* self);
extern int (*iMX508_getBatteryLevel)(HardwareInterface* self);
extern uint (*iMX508_chargingState)(HardwareInterface* self);
extern int (*AllWinner_getBatteryLevel)(HardwareInterface* self);
extern uint (*AllWinner_chargingState)(HardwareInterface* self);

class BaseEventFilter : public QObject {
public:
    explicit BaseEventFilter(QObject* parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        return QObject::eventFilter(obj, event);
    }
};

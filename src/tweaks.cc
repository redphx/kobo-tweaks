#include <NickelHook.h>
#include "tweaks.h"
#include "common.h"
#include "debug_utils.h"
#include "widgets/clock_widget.h"
#include "hooks/reading_view.h"

#include <QWidget>
#include <QString>
#include <QDir>

struct nh_info Tweaks = {
    .name = "Kobo Tweaks",
    .desc = "Tweaks",
    .uninstall_flag = KOBO_TWEAKS_DELETE_FILE,
    .uninstall_xflag = KOBO_TWEAKS_INSTALL_FILE,
};

int tweaksInit() {
    // Init folder structure
    QDir("/mnt/onboard/.adds/tweaks/images");

    return 0;
}

bool tweaksUninstall() {
    // Remove uninstall flag even if we're uninstalling using the other uninstall file
    QFile(KOBO_TWEAKS_INSTALL_FILE).remove();
    return true;
}

struct nh_hook TweaksHook[] = {
    {
        .sym      = "_ZN11ReadingViewC1EP7QWidget",
        .sym_new  = "hook_ReadingView_constructor",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(ReadingView_constructor),
        .desc     = "ReadingView",
        .optional = true,
    },
    {
        .sym      = "_ZN13ReadingFooter15setFooterMarginEi",
        .sym_new  = "hook_ReadingFooter_setFooterMargin",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(ReadingFooter_setFooterMargin),
        .desc     = "ReadingView",
        .optional = true,
    },
    {
        .sym      = "_ZN11ReadingView11pageChangedEi", 
        .sym_new  = "hook_ReadingView_pageChanged",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(ReadingView_pageChanged),
        .desc     = "ReadingView",
        .optional = true,
    },
    {
        .sym      = "_ZNK28SearchAutoCompleteController21handleSpecialCommandsERK7QString",
        .sym_new  = "hook_SearchAutoCompleteController_handleSpecialCommands",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(SearchAutoCompleteController_handleSpecialCommands),
        .desc     = "ReadingView",
        .optional = true,
    },
    {
        .sym      = "_ZN14DogEarDelegateC2EP7QWidgetRK7QString",
        .sym_new  = "hook_DogEarDelegate_constructor",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(DogEarDelegate_constructor),
        .desc     = "ReadingView",
        .optional = true,
    },
    {0}
};

struct nh_dlsym TweaksDlsym[] = {
    {
        .name = "_ZN15HardwareFactory14sharedInstanceEv",
        .out  = nh_symoutptr(HardwareFactory_sharedInstance),
    },
    {
        .name     = "_ZNK3MTK15getBatteryLevelEv",
        .out      = nh_symoutptr(MTK_getBatteryLevel),
        .desc     = "MTK getBatteryLevel()",
        .optional = true,
    },
    {
        .name     = "_ZN3MTK13chargingStateEv",
        .out      = nh_symoutptr(MTK_chargingState),
        .desc     = "MTK chargingState()",
        .optional = true,
    },

    {
        .name     = "_ZNK6iMX50815getBatteryLevelEv",
        .out      = nh_symoutptr(iMX508_getBatteryLevel),
        .desc     = "iMX508 getBatteryLevel()",
        .optional = true,
    },
    {
        .name     = "_ZN6iMX50813chargingStateEv",
        .out      = nh_symoutptr(iMX508_chargingState),
        .desc     = "iMX508 chargingState()",
        .optional = true,
    },

    {
        .name     = "_ZNK3AllWinner15getBatteryLevelEv",
        .out      = nh_symoutptr(AllWinner_getBatteryLevel),
        .desc     = "AllWinner getBatteryLevel()",
        .optional = true,
    },
    {
        .name     = "_ZN9AllWinner13chargingState",
        .out      = nh_symoutptr(AllWinner_chargingState),
        .desc     = "AllWinner chargingState()",
        .optional = true,
    },

    {
        .name = "_ZN25ConfirmationDialogFactory12showOKDialogERK7QStringS2_",
        .out = nh_symoutptr(ConfirmationDialogFactory_showOKDialog)
    },

    {0},
};

NickelHook(
    .init      = &tweaksInit,
    .info      = &Tweaks,
    .hook      = TweaksHook,
    .dlsym     = TweaksDlsym,
    .uninstall = &tweaksUninstall,
);

// HOOKS
extern "C" __attribute__((visibility("default")))
ReadingView* hook_ReadingView_constructor(ReadingView* self) {
    return ReadingViewHook::constructor(self);
}

extern "C" __attribute__((visibility("default")))
void hook_ReadingFooter_setFooterMargin(QWidget* self, int margin) {
    ReadingViewHook::setFooterMargin(self, margin);
}


extern "C" __attribute__((visibility("default")))
void hook_ReadingView_pageChanged(ReadingView* self, int page_index) {
    ReadingViewHook::pageChanged(self, page_index);
}

extern "C" __attribute__((visibility("default")))
QWidget* hook_DogEarDelegate_constructor(QWidget* self, QWidget* parent, const QString& image) {
    return ReadingViewHook::DogEarDelegate::constructor(self, parent, image);
}

extern "C" __attribute__((visibility("default")))
void hook_SearchAutoCompleteController_handleSpecialCommands(SearchAutoCompleteController* self, const QString& command) {
    if (command.compare("kt ", Qt::CaseInsensitive) == 0 || command.compare("kobotweaks ", Qt::CaseInsensitive) == 0) {
        ConfirmationDialogFactory_showOKDialog(QLatin1String("KoboTweaks"), QLatin1String("!!!"));
        return;
    }

    SearchAutoCompleteController_handleSpecialCommands(self, command);
}

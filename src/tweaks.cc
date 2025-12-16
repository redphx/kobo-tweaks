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
    QDir imagesDir(IMAGES_DIR);
    imagesDir.mkpath(".");

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
        .name    = "_ZTV17HardwareInterface",
        .out     = nh_symoutptr(HardwareInterface_vtable),
        .desc    = "HardwareInterface::vtable"
    },
    {
        .name    = "_ZNK17HardwareInterface15getBatteryLevelEv",
        .out     = nh_symoutptr(HardwareInterface_getBatteryLevel),
        .desc    = "HardwareInterface::getBatteryLevel()",
        .optional = true,
    },
    {
        .name    = "_ZN17HardwareInterface13chargingStateEv",
        .out     = nh_symoutptr(HardwareInterface_chargingState),
        .desc    = "HardwareInterface::chargingState()",
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
void hook_ReadingView_constructor(ReadingView* self) {
    ReadingViewHook::constructor(self);
}

extern "C" __attribute__((visibility("default")))
void hook_ReadingFooter_setFooterMargin(QWidget* self, int margin) {
    ReadingViewHook::setFooterMargin(self, margin);
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

#include "tweaks.h"
#include "common.h"
#include "debug_utils.h"
#include "widgets/clock_widget.h"
#include "hooks/reading_view.h"
#include "adapters/reading_view.h"

#include <QWidget>
#include <QString>
#include <QDir>
#include <QIODevice>
#include <QTextStream>


struct nh_info Tweaks = {
    .name = "Kobo Tweaks",
    .desc = "Tweaks",
    .uninstall_flag = KOBO_TWEAKS_DELETE_FILE,
    .uninstall_xflag = KOBO_TWEAKS_INSTALL_FILE,
};

static bool hasNickelClock = false;
int tweaksInit() {
    // Init folder structure
    QDir imagesDir(IMAGES_DIR);
    imagesDir.mkpath(".");

    // Override DELETE_TO_UNINSTALL.txt file
    QFile installFile(KOBO_TWEAKS_INSTALL_FILE);
    if (installFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&installFile);
        out << QStringLiteral("Delete this file then restart the device to uninstall Kobo Tweaks\n")
            << QStringLiteral("------\n")
            << QStringLiteral("Installed version: %1\n").arg(ADDON_VERSION)
            << QStringLiteral("Project page: github.com/redphx/kobo-tweaks\n");
        installFile.close();
    }

    // Check NickelClock
    QFile nickelClock(QStringLiteral("/usr/local/Kobo/imageformats/libnickelclock.so"));
    QFile nickelClockFailsafe(QStringLiteral("/usr/local/Kobo/imageformats/libnickelclock.so.failsafe"));

    hasNickelClock = nickelClock.exists() || nickelClockFailsafe.exists();
    if (hasNickelClock) {
        // Uninstall NickelClock
        nickelClock.remove();
        nickelClockFailsafe.remove();
    }

    // Migrate settings
    TweaksSettings* tweaksSettings = new TweaksSettings();
    tweaksSettings->migrate();

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
        .desc     = "ReadingView::constructor()",
        .optional = true,
    },
    {
        .sym      = "_ZN13ReadingFooter15setFooterMarginEi",
        .sym_new  = "hook_ReadingFooter_setFooterMargin",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(ReadingFooter_setFooterMargin),
        .desc     = "ReadingView::setFooterMargin()",
        .optional = true,
    },
    {
        .sym      = "_ZN15ReadingSettings19getBookProgressTypeEv",
        .sym_new  = "hook_ReadingSettings_getBookProgressType",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(ReadingSettings_getBookProgressType),
        .desc     = "ReadingSettings::getBookProgressType()",
        .optional = true,
    },
    {
        .sym      = "_ZN15ReadingSettings22getChapterProgressTypeEv",
        .sym_new  = "hook_ReadingSettings_getChapterProgressType",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(ReadingSettings_getChapterProgressType),
        .desc     = "ReadingSettings::getChapterProgressType()",
        .optional = true,
    },
    {
        .sym      = "_ZN21BrightnessEventFilter22updateBrightnessHeaderERK7QStringS2_",
        .sym_new  = "hook_BrightnessEventFilter_updateBrightnessHeader",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(BrightnessEventFilter_updateBrightnessHeader),
        .desc     = "BrightnessEventFilter::updateBrightnessHeader()",
        .optional = true,
    },
    {
        .sym      = "_ZNK28SearchAutoCompleteController21handleSpecialCommandsERK7QString",
        .sym_new  = "hook_SearchAutoCompleteController_handleSpecialCommands",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(SearchAutoCompleteController_handleSpecialCommands),
        .desc     = "SearchAutoCompleteController::handleSpecialCommands()",
        .optional = true,
    },
    {
        .sym      = "_ZN14DogEarDelegateC2EP7QWidgetRK7QString",
        .sym_new  = "hook_DogEarDelegate_constructor",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(DogEarDelegate_constructor),
        .desc     = "DogEarDelegate::constructor()",
        .optional = true,
    },
    {
        .sym      = "_ZN11AdobeReaderC2EP7QWidgetP11PluginStateRK7QString",
        .sym_new  = "hook_AdobeReader_constructor",
        .lib      = "libadobe.so",
        .out      = nh_symoutptr(AdobeReader_constructor),
        .desc     = "Adobe DogEar EPUB",
        .optional = true,
    },
    {0}
};

struct nh_dlsym TweaksDlsym[] = {
    {
        .name     = "_ZNK7Content8getTitleEv",
        .out      = nh_symoutptr(Content_getTitle),
        .desc     = "Content::getTitle()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView15getChapterTitleEv",
        .out      = nh_symoutptr(ReadingView_getChapterTitle),
        .desc     = "ReadingView::getChapterTitle()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView18chapterCurrentPageEv",
        .out      = nh_symoutptr(ReadingView_chapterCurrentPage),
        .desc     = "ReadingView::chapterCurrentPage()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView17chapterTotalPagesEv",
        .out      = nh_symoutptr(ReadingView_chapterTotalPages),
        .desc     = "ReadingView::chapterTotalPages()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView19fullBookCurrentPageEv",
        .out      = nh_symoutptr(ReadingView_fullBookCurrentPage),
        .desc     = "ReadingView::fullBookCurrentPage()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView18fullBookTotalPagesEv",
        .out      = nh_symoutptr(ReadingView_fullBookTotalPages),
        .desc     = "ReadingView::fullBookTotalPages()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView20hasValidReadingStatsEv",
        .out      = nh_symoutptr(ReadingView_hasValidReadingStats),
        .desc     = "ReadingView::hasValidReadingStats()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView12readingStatsEv",
        .out      = nh_symoutptr(ReadingView_readingStats),
        .desc     = "ReadingView::readingStats()",
        .optional = true,
    },
    {
        .name     = "_ZN11ReadingView25getCalculatedReadProgressEv",
        .out      = nh_symoutptr(ReadingView_getCalculatedReadProgress),
        .desc     = "ReadingView::getCalculatedReadProgress()",
        .optional = true,
    },
    {
        .name     = "_ZNK12ReadingStats22currentChapterEstimateEv",
        .out      = nh_symoutptr(ReadingStats_currentChapterEstimate),
        .desc     = "ReadingStats::currentChapterEstimate()",
        .optional = true,
    },
    {
        .name     = "_ZNK12ReadingStats22currentChapterEstimateEv",
        .out      = nh_symoutptr(ReadingStats_currentChapterEstimate),
        .desc     = "ReadingStats::currentChapterEstimate()",
        .optional = true,
    },
    {
        .name     = "_ZNK12ReadingStats18restOfBookEstimateEv",
        .out      = nh_symoutptr(ReadingStats_restOfBookEstimate),
        .desc     = "ReadingStats::restOfBookEstimate()",
        .optional = true,
    },
    {
        .name     = "_ZN12ReadingStatsD1Ev",
        .out      = nh_symoutptr(ReadingStats_deconstructor),
        .desc     = "ReadingStats::deconstructor()",
        .optional = true,
    },

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
        .name     = "_ZNK17HardwareInterface15getBatteryLevelEv",
        .out      = nh_symoutptr(HardwareInterface_getBatteryLevel),
        .desc     = "HardwareInterface::getBatteryLevel()",
        .optional = true,
    },
    {
        .name     = "_ZN17HardwareInterface13chargingStateEv",
        .out      = nh_symoutptr(HardwareInterface_chargingState),
        .desc     = "HardwareInterface::chargingState()",
        .optional = true,
    },
    {
        .name     = "_ZN25ConfirmationDialogFactory12showOKDialogERK7QStringS2_",
        .out      = nh_symoutptr(ConfirmationDialogFactory_showOKDialog),
        .desc     = "ConfirmationDialogFactory::showOKDialog()",
        .optional = true,
    },
    {
		.name = "_ZN20MainWindowController14sharedInstanceEv",
		.out  = nh_symoutptr(MainWindowController_sharedInstance),
	},
	{
		.name = "_ZNK20MainWindowController11currentViewEv",
		.out  = nh_symoutptr(MainWindowController_currentView),
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

    if (hasNickelClock && ConfirmationDialogFactory_showOKDialog) {
        // Show a dialog prompting the user to reboot their device
        auto readerDoneLoadingAdapter = new ReadingViewAdapter::ReaderDoneLoading(self);
        QObject::connect(readerDoneLoadingAdapter, &ReadingViewAdapter::ReaderDoneLoading::readerDoneLoading, self, []() {
            ConfirmationDialogFactory_showOKDialog(QStringLiteral("Kobo Tweaks"), QStringLiteral("NickelClock has been successfully uninstalled.<br>Please restart the device to complete the process."));
        });
    }

    if (QFile::exists(QStringLiteral(DATA_DIR "/debug"))) {
        QTimer::singleShot(2000, self, [self] { DebugUtils::dumpWidgetToFile(QString("/mnt/onboard/_ReadingView.log"), self); });
    }
}

extern "C" __attribute__((visibility("default")))
void hook_ReadingFooter_setFooterMargin(QWidget* self, int margin) {
    ReadingViewHook::setFooterMargin(self, margin);
}

extern "C" __attribute__((visibility("default")))
void hook_DogEarDelegate_constructor(QWidget* self, QWidget* parent, const QString& image) {
    ReadingViewHook::DogEarDelegate::constructor(self, parent, image);
}

extern "C" __attribute__((visibility("default")))
void hook_SearchAutoCompleteController_handleSpecialCommands(SearchAutoCompleteController* self, const QString& command) {
    if (command.compare("kt ", Qt::CaseInsensitive) == 0 || command.compare("kobotweaks ", Qt::CaseInsensitive) == 0) {
        ConfirmationDialogFactory_showOKDialog(QLatin1String("KoboTweaks"), QLatin1String("!!!"));
        return;
    }

    SearchAutoCompleteController_handleSpecialCommands(self, command);
}

extern "C" __attribute__((visibility("default")))
int hook_ReadingSettings_getChapterProgressType(ReadingSettings*) {
    // always hide header
    return 0;
}

extern "C" __attribute__((visibility("default")))
int hook_ReadingSettings_getBookProgressType(ReadingSettings*) {
    // always hide footer
    return 0;
}

extern "C" __attribute__((visibility("default")))
void hook_BrightnessEventFilter_updateBrightnessHeader(BrightnessEventFilter* self, const QString& text, const QString& sth) {
    // BrightnessEventFilter_updateBrightnessHeader(self, text, sth);
    ReadingViewHook::BrightnessEventFilterHook::updateBrightnessHeader(self, text, sth);
};

// libadobe
extern "C" __attribute__((visibility("default")))
void hook_AdobeReader_constructor(QWidget* self, QWidget* parent, PluginState* state, const QString& image) {
    ReadingViewHook::AdobeReader::constructor(self, parent, state, image);
}

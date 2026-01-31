#include "tweaks.h"
#include "common.h"
#include "debug_utils.h"
#include "widgets/clock_widget.h"
#include "hooks/keyboard.h"
#include "hooks/reading_view.h"
#include "adapters/reading_view.h"

#include <QWidget>
#include <QString>
#include <QDir>
#include <QIODevice>
#include <QTextStream>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSize>


class PopupKeyboardControllerShowFilter : public QObject {
protected:
    bool eventFilter(QObject*, QEvent* event) override {
        if (event->type() == QEvent::Show) {
            nh_log("PopupKeyboardController shown");
        }
        return false;
    }
};

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

    // Keyboard
    {
        .sym      = "_ZN24SearchKeyboardController13popupKeyboardEP10VirtualKey7QVectorI17KeyboardLayoutRowE",
        .sym_new  = "hook_SearchKeyboardController_popupKeyboard",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(SearchKeyboardController_popupKeyboard),
        .desc     = "SearchKeyboardController::popupKeyboard()",
        .optional = true,
    },
    {
        .sym      = "_ZNK24SearchKeyboardController6newKeyEPKcii",
        .sym_new  = "hook_SearchKeyboardController_newKey",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(SearchKeyboardController_newKey),
        .desc     = "SearchKeyboardController::hook_SearchKeyboardController_newKey()",
        .optional = true,
    },
    {
        .sym      = "_ZN23PopupKeyboardControllerC1EP26ExtendedKeyboardControllerP15VirtualKeyboard7QVectorI17KeyboardLayoutRowE",
        .sym_new  = "hook_PopupKeyboardController_constructor",
        .lib      = "libnickel.so.1.0.0",
        .out      = nh_symoutptr(PopupKeyboardController_constructor),
        .desc     = "PopupKeyboardController::constructor()",
        .optional = true,
    },

    // DogEar
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

    // Keyboard
    {
        .name     = "_ZN23PopupKeyboardController4menuEv",
        .out      = nh_symoutptr(PopupKeyboardController_menu),
        .desc     = "PopupKeyboardController::menu()",
        .optional = true,
    },
    {
        .name     = "_ZNK15VirtualKeyboard7keySizeEv",
		.out      = nh_symoutptr(VirtualKeyboard_keySize),
        .desc     = "VirtualKeyboard::keySize()",
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

extern "C" __attribute__((visibility("default")))
VirtualKey* hook_SearchKeyboardController_newKey(SearchKeyboardController* self, const char* label, int keyCode, int weight) {
    nh_log(QString("hook_SearchKeyboardController_newKey: %1 %2").arg(label).arg(keyCode).toUtf8().constData());
    auto virtualKey = SearchKeyboardController_newKey(self, label, keyCode, weight);

    // Expose label & keyCode so they can be used in other places
    virtualKey->setProperty("twksLabel", QVariant::fromValue(QString(label)));
    virtualKey->setProperty("twksKeyCode", QVariant::fromValue(keyCode));

    return virtualKey;
}

QWidget* globalPopupKeyboardController = nullptr;

extern "C" __attribute__((visibility("default")))
void hook_SearchKeyboardController_popupKeyboard(SearchKeyboardController* self, VirtualKey* key, QVector<KeyboardLayoutRow> rows) {
    QString label = key->property("twksLabel").toString();
    if (label == "a") {
        rows.clear();
        KeyboardLayoutRow row1;
        row1.keys.append(SearchKeyboardController_newKey(self, "",  0xffff0000, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "á", 0xffff0001, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "à", 0xffff0002, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ả", 0xffff0003, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ã", 0xffff0004, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ạ", 0xffff0005, 10));
        rows.append(row1);

        KeyboardLayoutRow row2;
        rows.append(row2);

        KeyboardLayoutRow row3;
        rows.append(row3);
    } else if (label == "e") {
        rows.clear();
        KeyboardLayoutRow row1;
        row1.keys.append(SearchKeyboardController_newKey(self, "",  0xffff0100, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "é", 0xffff0101, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "è", 0xffff0102, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ẻ", 0xffff0103, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ẽ", 0xffff0104, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ẹ", 0xffff0105, 10));
        rows.append(row1);

        KeyboardLayoutRow row2;
        rows.append(row2);
    } else if (label == "u") {
        rows.clear();
        KeyboardLayoutRow row1;
        row1.keys.append(SearchKeyboardController_newKey(self, "",  0xffff0200, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ú", 0xffff0201, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ù", 0xffff0202, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ủ", 0xffff0203, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ũ", 0xffff0204, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ụ", 0xffff0205, 10));
        rows.append(row1);

        KeyboardLayoutRow row2;
        rows.append(row2);
    } else if (label == "o") {
        rows.clear();
        KeyboardLayoutRow row1;
        row1.keys.append(SearchKeyboardController_newKey(self, "",  0xffff0300, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ó", 0xffff0301, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ò", 0xffff0302, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ỏ", 0xffff0303, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "õ", 0xffff0304, 10));
        row1.keys.append(SearchKeyboardController_newKey(self, "ọ", 0xffff0305, 10));
        rows.append(row1);

        KeyboardLayoutRow row2;
        rows.append(row2);

        KeyboardLayoutRow row3;
        rows.append(row3);
    }

    /*
    row3.keys.append(SearchKeyboardController_newKey(self, "ấ", 0xffffcf20, 10));
    row3.keys.append(SearchKeyboardController_newKey(self, "ầ", 0xffffcf21, 10));
    row3.keys.append(SearchKeyboardController_newKey(self, "ẩ", 0xffffcf22, 10));
    row3.keys.append(SearchKeyboardController_newKey(self, "ẫ", 0xffffcf23, 10));
    row3.keys.append(SearchKeyboardController_newKey(self, "ậ", 0xffffcf24, 10));
    row3.leftSpacer = 0;
    row3.rightSpacer = 0;
    rows.append(row3);
    */
    SearchKeyboardController_popupKeyboard(self, key, rows);
    nh_log(QString("virtualkey: %1 %2").arg(key->property("twksLabel").toString()).arg(key->property("twksKeyCode").toString()).toUtf8().constData());

    /*
    KeyboardLayoutRow* current = rows.data(); 
    KeyboardLayoutRow* end = current + rows.size();

    for (; current != end; ++current) { // Increments by sizeof(KeyboardLayoutRow) = 12
        // Inner Loop: Access the keys QVector
        QVector<VirtualKey*> rowKeys = current->keys;
        for (VirtualKey* k : rowKeys) {
            nh_log(QString("virtualkey row: %1 %2").arg(k->property("twksLabel").toString()).arg(k->property("twksKeyCode").toString()).toUtf8().constData());
        }
    }
    */

    if (globalPopupKeyboardController && PopupKeyboardController_menu) {
        NickelTouchMenu* menu = PopupKeyboardController_menu(globalPopupKeyboardController);
        PopupKeyboard* popupKeyboard = menu->findChild<PopupKeyboard*>(QString());
        if (popupKeyboard && rows.size() > 1) {
            QSize size = VirtualKeyboard_keySize(popupKeyboard);
            nh_log(QString("VirtualKeyboard_keySize %1 %2").arg(size.width()).arg(size.height()).toUtf8().constData());

            QVBoxLayout* rootLayout = qobject_cast<QVBoxLayout*>(popupKeyboard->layout());
            rootLayout->setSpacing(10);

            // QLabel* label = new QLabel();
            // label->setText("test");
            QVector<KeyboardLayoutRow> additionalRows;

            if (label == "a") {
                KeyboardLayoutRow row2;
                row2.keys.append(SearchKeyboardController_newKey(self, "ă", 0xffff0010, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ắ", 0xffff0011, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ằ", 0xffff0012, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ẳ", 0xffff0013, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ẵ", 0xffff0014, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ặ", 0xffff0015, 10));
                additionalRows.append(row2);

                KeyboardLayoutRow row3;
                row3.keys.append(SearchKeyboardController_newKey(self, "â", 0xffff0021, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ấ", 0xffff0020, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ầ", 0xffff0022, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ẩ", 0xffff0023, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ẫ", 0xffff0024, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ậ", 0xffff0025, 10));
                additionalRows.append(row3);
            } else if (label == "e") {
                KeyboardLayoutRow row2;
                row2.keys.append(SearchKeyboardController_newKey(self, "ê", 0xffff0110, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ế", 0xffff0111, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ề", 0xffff0112, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ể", 0xffff0113, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ễ", 0xffff0114, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ệ", 0xffff0115, 10));
                additionalRows.append(row2);
            } else if (label == "u") {
                KeyboardLayoutRow row2;
                row2.keys.append(SearchKeyboardController_newKey(self, "ư", 0xffff0210, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ứ", 0xffff0211, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ừ", 0xffff0212, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ử", 0xffff0213, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ữ", 0xffff0214, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ự", 0xffff0215, 10));
                additionalRows.append(row2);
            } else if (label == "o") {
                KeyboardLayoutRow row2;
                row2.keys.append(SearchKeyboardController_newKey(self, "ơ", 0xffff0310, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ớ", 0xffff0311, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ờ", 0xffff0312, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ở", 0xffff0313, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ỡ", 0xffff0314, 10));
                row2.keys.append(SearchKeyboardController_newKey(self, "ợ", 0xffff0315, 10));
                additionalRows.append(row2);

                KeyboardLayoutRow row3;
                row3.keys.append(SearchKeyboardController_newKey(self, "ô", 0xffff0321, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ố", 0xffff0320, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ồ", 0xffff0322, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ổ", 0xffff0323, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ỗ", 0xffff0324, 10));
                row3.keys.append(SearchKeyboardController_newKey(self, "ộ", 0xffff0325, 10));
                additionalRows.append(row3);
            }

            for (int i = 0; i < additionalRows.size(); ++i) {
                QHBoxLayout* hRow = new QHBoxLayout();
                // Set spacing between keys in this row (Horizontal)
                hRow->setSpacing(2);

                // Add every key in this row to the horizontal layout
                const QVector<VirtualKey*>& keys = additionalRows[i].keys;
                for (VirtualKey* k : keys) {
                    k->setContentsMargins(0, 0, 0, 0);
                    k->setFixedSize(100, 100);
                    hRow->addWidget(k);
                }

                // Add this horizontal row to the vertical root layout
                rootLayout->insertLayout(0, hRow);
            }

            popupKeyboard->setFixedSize(popupKeyboard->width(), popupKeyboard->height() * (1 + additionalRows.size()));
        }

        DebugUtils::dumpWidgetToFile(QString("/mnt/onboard/_popup.txt"), menu);
    }

    globalPopupKeyboardController = nullptr;
};


extern "C" __attribute__((visibility("default")))
void hook_PopupKeyboardController_constructor(PopupKeyboardController* self, QWidget* parent, VirtualKeyboard* keyboard, QVector<KeyboardLayoutRow> rows) {
    PopupKeyboardController_constructor(self, parent, keyboard, rows);
    globalPopupKeyboardController = self;
}

// libadobe
extern "C" __attribute__((visibility("default")))
void hook_AdobeReader_constructor(QWidget* self, QWidget* parent, PluginState* state, const QString& image) {
    ReadingViewHook::AdobeReader::constructor(self, parent, state, image);
}

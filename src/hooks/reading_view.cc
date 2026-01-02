#include "reading_view.h"
#include "../adapters/reading_view.h"

namespace ReadingViewHook {
    static TweaksSettings settings;
    static bool isDarkMode = false;
    static int originalContentsMargins = 0;
    static const Volume* currentVolume = nullptr;

    void constructor(ReadingView* view) {
        // Must parse settings before constructor since other widgets use them
        settings.load();
        settings.sync();

        ReadingView_constructor(view);

        // MUST NOT KEEP REFS TO THE WIDGETS, AS WE DON'T CONTROL THE LIFETIME

        // Note: created and passed to grabGestures in the constructor, most events are passed to it
        QWidget* gestureContainer = view->findChild<GestureReceivingContainer*>(QStringLiteral("gestureContainer"), Qt::FindDirectChildrenOnly);
        if (!gestureContainer) {
            nh_log("could not find \"gestureContainer\"");
            return;
        }

        // Find "header"
        QWidget* header = gestureContainer->findChild<ReadingFooter*>(QStringLiteral("header"), Qt::FindDirectChildrenOnly);
        QWidget* footer = gestureContainer->findChild<ReadingFooter*>(QStringLiteral("footer"), Qt::FindDirectChildrenOnly);
        if (!header || !footer) {
            nh_log("could not find \"header/footer\"");
            return;
        }

        // Update QSS
        QString rootQss = view->styleSheet();
        view->setStyleSheet(Patch::ReadingView::reduceSpacerHeight(rootQss));

        QString readingFooterQss = Qss::getContent(QStringLiteral(":/qss/ReadingFooter.qss"));
        QString patchedQss = Qss::copySelectors(readingFooterQss, QStringLiteral("#caption"), QStringList() << QStringLiteral("#twks_label") << QStringLiteral("#twks_separator"));

        auto readingSettings = settings.getReadingSettings();
        if (readingSettings.headerFooterHeightScale < 100) {
            patchedQss = Patch::ReadingView::scaleHeaderFooterHeight(patchedQss, readingSettings.headerFooterHeightScale);
        }
        patchedQss.replace(QStringLiteral("ReadingFooter"), QStringLiteral("TwWidgetZonesContainer"));

        // These adapters abstract the logic and ensure that the update methods on the widgets aren't called after either the widget or the ReadingView has been destroyed
        auto renderVolumeAdapter = new ReadingViewAdapter::RenderVolume(view);
        QObject::connect(renderVolumeAdapter, &ReadingViewAdapter::RenderVolume::renderVolume, [](const Volume& volume) {
            currentVolume = &volume;
        });

        auto darkModeAdapter = new ReadingViewAdapter::DarkMode(gestureContainer, view);
        isDarkMode = darkModeAdapter->getDarkMode();
        QObject::connect(darkModeAdapter, &ReadingViewAdapter::DarkMode::darkModeChanged, [](bool dark) {
            isDarkMode = dark;
        });

        auto readerDoneLoadingAdapter = new ReadingViewAdapter::ReaderDoneLoading(view);

        ReadingViewAdapters adapters {};
        adapters.pageChanged = new ReadingViewAdapter::PageChanged(view);
        adapters.darkMode = darkModeAdapter;
        adapters.renderVolume = renderVolumeAdapter;
        adapters.readerDoneLoading = readerDoneLoadingAdapter;

        TwWidgetZonesContainer* headerContainer = new TwWidgetZonesContainer(readingSettings, patchedQss);
        TwWidgetZonesContainer* footerContainer = new TwWidgetZonesContainer(readingSettings, patchedQss);

        QVBoxLayout* gestureLayout = qobject_cast<QVBoxLayout*>(gestureContainer->layout());
        gestureLayout->insertWidget(gestureLayout->indexOf(header) + 1, headerContainer);
        gestureLayout->insertWidget(gestureLayout->indexOf(footer) + 1, footerContainer);

        QObject::connect(readerDoneLoadingAdapter, &ReadingViewAdapter::ReaderDoneLoading::readerDoneLoading, [view, adapters, headerContainer, footerContainer, readingSettings] {
            headerContainer->setupZones(view, adapters, currentVolume, readingSettings.widgetHeaderLeft, readingSettings.widgetHeaderCenter, readingSettings.widgetHeaderRight);
            footerContainer->setupZones(view, adapters, currentVolume, readingSettings.widgetFooterLeft, readingSettings.widgetFooterCenter, readingSettings.widgetFooterRight);
        });
    }

    void setFooterMargin(QWidget* self, int margin) {
        // Save the original margin
        originalContentsMargins = margin;

        QLayout* layout = self->layout();
        layout->setContentsMargins(margin, 0, margin, 0);
    }

    namespace DogEarDelegate {
        QWidget* constructor(QWidget* self, QWidget* parent, const QString& orgImgPath) {
            QString imgPath = settings.getReadingBookmarkImage(isDarkMode);
            if (imgPath.isEmpty()) {
                imgPath = orgImgPath;
            }
            auto view = DogEarDelegate_constructor(self, parent, imgPath);
            return view;
        }
    }
}

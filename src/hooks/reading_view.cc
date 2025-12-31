#include "reading_view.h"

namespace ReadingViewHook {
    struct WidgetAdapters {
        PageChangedAdapter* pageChangedAdapter;
        DarkModeAdapter* darkModeAdapter;
        RenderVolumeAdapter* renderVolumeAdapter;
        ReaderDoneLoadingAdapter* readerDoneLoadingAdapter;
    };

    static TweaksSettings settings;
    static bool isDarkMode = false;
    static int originalContentsMargins = 0;
    static const Volume* currentVolume = nullptr;

    bool addedWidgets = false;

    PageChangedAdapter::PageChangedAdapter(ReadingView *parent) : QObject(parent) {
        if (!QObject::connect(parent, SIGNAL(pageChanged(int)), this, SLOT(notifyPageChanged()), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView11pageChangedEi");
        }
    }

    void PageChangedAdapter::notifyPageChanged() {
        // nh_log("page changed");
        pageChanged();
    }

    ReaderDoneLoadingAdapter::ReaderDoneLoadingAdapter(ReadingView *parent) : QObject(parent) {
        if (!QObject::connect(parent, SIGNAL(readerDoneLoading()), this, SLOT(notifyReaderDoneLoading()), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView17readerDoneLoadingEv");
        }
    }

    void ReaderDoneLoadingAdapter::notifyReaderDoneLoading() {
        // nh_log("readerDoneLoading");
        readerDoneLoading();
    }

    DarkModeAdapter::DarkModeAdapter(GestureReceivingContainer *parent, ReadingView *view) : QObject(parent) {
        if (!QObject::connect(view, SIGNAL(darkModeChangedSignal()), this, SLOT(notifyDarkModeChanged()), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView21darkModeChangedSignalEv");
        }
    }

    bool DarkModeAdapter::getDarkMode() {
        // the property is set by ReadingView by ReadingView::darkModeChanged
        // (which calls darkModeChangedSignal afterwards) on the
        // GestureReceivingContainer set up in Ui_ReadingView::setupUi called by
        // the constructor

        // if this is no longer viable, we could also call
        // ReadingSettings::getDarkMode, but this won't handle the cases where
        // ReadingView doesn't support dark mode for the current format (e.g.,
        // audiobooks)

        // it's also stored as a field on ReadingView, but that would require
        // hardcoding offsets

        auto prop = parent()->property("darkMode");
        if (!prop.isValid()) {
            nh_log("darkMode property not set on GestureReceivingContainer");
            return false;
        }
        return prop.toBool();
    }

    void DarkModeAdapter::notifyDarkModeChanged() {
        auto dark = getDarkMode();
        nh_log("dark mode changed (%s)", dark ? "dark" : "light");
        darkModeChanged(dark);
    }

    RenderVolumeAdapter::RenderVolumeAdapter(ReadingView *parent) : QObject(parent) {
        if (!QObject::connect(parent, SIGNAL(renderVolume(const Volume&)), this, SLOT(notifyRenderVolume(const Volume&)), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView12renderVolumeERK6Volume");
        }
    }

    void RenderVolumeAdapter::notifyRenderVolume(const Volume& volume) {
        renderVolume(volume);
    }

    static void insertWidgets(ReadingView* readingView, WidgetAdapters adapters, TwWidgetZonesContainer* zonesContainer, QVector<WidgetTypeEnum> leftWidgets, QVector<WidgetTypeEnum> rightWidgets) {
        auto readingSettings = settings.getReadingSettings();
        QHBoxLayout* parentLayout = qobject_cast<QHBoxLayout*>(zonesContainer->layout());

        bool isLeft = true;
        for (auto widgetTypes : {leftWidgets, rightWidgets}) {
            // Setup Widgets container
            QWidget* container = new QWidget;
            // Set container's width to the original margin value
            container->setMinimumWidth(originalContentsMargins);
            container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            // container->setStyleSheet("border: 1px solid black;");

            // Setup Widgets container's layout
            QHBoxLayout* containerLayout = new QHBoxLayout(container);
            containerLayout->setContentsMargins(0, 0, 0, 0);
            containerLayout->setSpacing(readingSettings.widgetSpacing);

            if (!isLeft) {
                containerLayout->addStretch(1);
            }

            int idx = -1;
            for (auto widgetType : widgetTypes) {
                ++idx;
                if (idx > 0 && readingSettings.widgetSeparator != WidgetSeparatorEnum::Invalid) {
                    // Add separator label
                    TwSeparatorLabel* separator = new TwSeparatorLabel(readingSettings.widgetSeparator);
                    containerLayout->addWidget(separator, 0);
                }

                QWidget* widget = nullptr;
                bool stretch = false;

                switch (widgetType) {
                    case WidgetTypeEnum::Clock:
                        {
                            TwClockWidgetConfig config {};
                            config.is24hFormat = readingSettings.widgetClock24hFormat;

                            auto clockWidget = new TwClockWidget(config);
                            QObject::connect(adapters.pageChangedAdapter, &PageChangedAdapter::pageChanged, clockWidget, &TwClockWidget::updateTime, Qt::UniqueConnection);
                            widget = clockWidget;
                        }
                        break;
                    case WidgetTypeEnum::Battery:
                        {
                            HardwareInterface* hardwareInterface = HardwareFactory_sharedInstance();

                            TwBatteryWidgetConfig config {};
                            config.isDarkMode = adapters.darkModeAdapter->getDarkMode();
                            config.isLeft = isLeft;
                            config.defaultStyle = readingSettings.widgetBatteryStyle;
                            config.chargingStyle = readingSettings.widgetBatteryStyleCharging;
                            config.showWhenBelow = readingSettings.widgetBatteryShowWhenBelow;

                            auto batteryWidget = new TwBatteryWidget(config, hardwareInterface);
                            QObject::connect(adapters.darkModeAdapter, &DarkModeAdapter::darkModeChanged, batteryWidget, &TwBatteryWidget::setDarkMode, Qt::UniqueConnection);
                            QObject::connect(adapters.pageChangedAdapter, &PageChangedAdapter::pageChanged, batteryWidget, &TwBatteryWidget::updateLevel, Qt::UniqueConnection);
                            widget = batteryWidget;
                        }
                        break;
                    case WidgetTypeEnum::BookPage:
                        {
                            auto bookPageWidget = new TwBookPageWidget();
                            QObject::connect(adapters.pageChangedAdapter, &PageChangedAdapter::pageChanged, [readingView, bookPageWidget]() {
                                if (ReadingView_fullBookCurrentPage && ReadingView_fullBookTotalPages) {
                                    int currentPage = ReadingView_fullBookCurrentPage(readingView);
                                    int totalPages = ReadingView_fullBookTotalPages(readingView);
                                    bookPageWidget->setProgress(currentPage, totalPages);
                                }
                            });
                            widget = bookPageWidget;
                        }
                        break;
                    case WidgetTypeEnum::BookTitle:
                        {
                            auto bookTitleWidget = new TwBookTitleWidget();
                            QObject::connect(adapters.renderVolumeAdapter, &RenderVolumeAdapter::renderVolume, bookTitleWidget, &TwBookTitleWidget::setTitle, Qt::UniqueConnection);
                            widget = bookTitleWidget;
                            stretch = true;
                        }
                        break;
                    case WidgetTypeEnum::ChapterPage:
                        {
                            auto chapterPageWidget = new TwChapterPageWidget();
                            QObject::connect(adapters.pageChangedAdapter, &PageChangedAdapter::pageChanged, [readingView, chapterPageWidget]() {
                                if (ReadingView_chapterCurrentPage && ReadingView_chapterTotalPages) {
                                    int currentPage = ReadingView_chapterCurrentPage(readingView);
                                    int totalPages = ReadingView_chapterTotalPages(readingView);
                                    chapterPageWidget->setProgress(currentPage, totalPages);
                                }
                            });
                            widget = chapterPageWidget;
                        }
                        break;
                    case WidgetTypeEnum::ChapterProgress:
                        {
                            TwChapterProgressWidgetConfig config {};
                            config.isDarkMode = adapters.darkModeAdapter->getDarkMode();

                            auto chapterProgressWidget = new TwChapterProgressWidget(config);

                            QObject::connect(adapters.darkModeAdapter, &DarkModeAdapter::darkModeChanged, chapterProgressWidget, &TwChapterProgressWidget::setDarkMode, Qt::UniqueConnection);
                            QObject::connect(adapters.pageChangedAdapter, &PageChangedAdapter::pageChanged, [readingView, chapterProgressWidget]() {
                                if (ReadingView_chapterCurrentPage && ReadingView_chapterTotalPages) {
                                    int currentPage = ReadingView_chapterCurrentPage(readingView);
                                    int totalPages = ReadingView_chapterTotalPages(readingView);
                                    chapterProgressWidget->setProgress(currentPage, totalPages);
                                }
                            });
                            widget = chapterProgressWidget;
                        }
                        break;
                    case WidgetTypeEnum::ChapterTitle:
                        {
                            auto chapterTitleWidget = new TwChapterTitleWidget();
                            QObject::connect(adapters.pageChangedAdapter, &PageChangedAdapter::pageChanged, [readingView, chapterTitleWidget]() {
                                // TODO: find another way to get chapter title, as this method always returns an uppercase string
                                QString title;
                                ReadingView_getChapterTitle(&title, readingView);
                                chapterTitleWidget->setTitle(title);
                            });
                            widget = chapterTitleWidget;
                            stretch = true;
                        }
                        break;
                    default:
                        break;
                }

                // Add widget to container
                if (widget) {
                    containerLayout->addWidget(widget, stretch ? 1 : 0);
                }
            }

            // Insert widgets container into parent layout
            if (isLeft) {
                // Insert left
                container->setContentsMargins(readingSettings.headerFooterMargins, 0, 0, 0);
                parentLayout->addWidget(container, 0, Qt::AlignLeft);
                // parentLayout->addStretch(1);
            } else {
                // Insert right
                container->setContentsMargins(0, 0, readingSettings.headerFooterMargins, 0);
                parentLayout->addWidget(container, 0, Qt::AlignRight);
            }

            if (isLeft) {
                // containerLayout->addStretch(1);
                isLeft = false;
            }
        }
    }

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

        // These adapters abstract the logic and ensure that the update methods on the widgets aren't called after either the widget or the ReadingView has been destroyed
        auto pageChangedAdapter = new PageChangedAdapter(view);
        auto darkModeAdapter = new DarkModeAdapter(gestureContainer, view);

        auto renderVolumeAdapter = new RenderVolumeAdapter(view);
        QObject::connect(renderVolumeAdapter, &RenderVolumeAdapter::renderVolume, [](const Volume& volume) {
            currentVolume = &volume;
        });

        isDarkMode = darkModeAdapter->getDarkMode();
        QObject::connect(darkModeAdapter, &DarkModeAdapter::darkModeChanged, [](bool dark) {
            isDarkMode = dark;
        });

        QString rootQss = view->styleSheet();
        view->setStyleSheet(Patch::ReadingView::reduceSpacerHeight(rootQss));

        QString readingFooterQss = Qss::getContent(QStringLiteral(":/qss/ReadingFooter.qss"));
        QString patchedQss = Qss::copySelectors(readingFooterQss, QStringLiteral("#caption"), QStringList() << QStringLiteral("#twks_label") << QStringLiteral("#twks_separator"));

        auto readingSettings = settings.getReadingSettings();
        if (readingSettings.headerFooterHeightScale < 100) {
            patchedQss = Patch::ReadingView::scaleHeaderFooterHeight(patchedQss, readingSettings.headerFooterHeightScale);
        }

        patchedQss.replace(QStringLiteral("ReadingFooter"), QStringLiteral("TwWidgetZonesContainer"));

        auto readerDoneLoadingAdapter = new ReadingViewHook::ReaderDoneLoadingAdapter(view);

        WidgetAdapters adapters {};
        adapters.pageChangedAdapter = pageChangedAdapter;
        adapters.darkModeAdapter = darkModeAdapter;
        adapters.renderVolumeAdapter = renderVolumeAdapter;
        adapters.readerDoneLoadingAdapter = readerDoneLoadingAdapter;

        TwWidgetZonesContainer* headerContainer = new TwWidgetZonesContainer(patchedQss, readingSettings.widgetSpacing);
        TwWidgetZonesContainer* footerContainer = new TwWidgetZonesContainer(patchedQss, readingSettings.widgetSpacing);

        QVBoxLayout* gestureLayout = qobject_cast<QVBoxLayout*>(gestureContainer->layout());
        gestureLayout->insertWidget(gestureLayout->indexOf(header) + 1, headerContainer);
        gestureLayout->insertWidget(gestureLayout->indexOf(footer) + 1, footerContainer);

        addedWidgets = false;
        QObject::connect(readerDoneLoadingAdapter, &ReadingViewHook::ReaderDoneLoadingAdapter::readerDoneLoading, [view, adapters, headerContainer, footerContainer, readingSettings] {
            if (addedWidgets) {
                return;
            }

            addedWidgets = true;
            insertWidgets(view, adapters, headerContainer, readingSettings.widgetHeaderLeft, readingSettings.widgetHeaderRight);
            insertWidgets(view, adapters, footerContainer, readingSettings.widgetFooterLeft, readingSettings.widgetFooterRight);
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

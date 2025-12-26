#include "reading_view.h"

namespace ReadingViewHook {
    static TweaksSettings settings;
    static bool isDarkMode = false;
    static int originalContentsMargins = 0;

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


    static void insertWidgets(PageChangedAdapter *pageChangedAdapter, DarkModeAdapter *darkModeAdapter, ReadingFooter* parent, QString qss, QVector<WidgetTypeEnum> leftWidgets, QVector<WidgetTypeEnum> rightWidgets) {
        auto readingSettings = settings.getReadingSettings();
        HardwareInterface* hardwareInterface = HardwareFactory_sharedInstance();

        parent->setStyleSheet(qss);
        parent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // stretch

        QHBoxLayout* parentLayout = qobject_cast<QHBoxLayout*>(parent->layout());
        parentLayout->setSpacing(readingSettings.widgetSpacing);

        bool isLeft = true;
        for (auto widgetTypes : {leftWidgets, rightWidgets}) {
            // Setup Widgets container
            QWidget* container = new QWidget;
            // Set container's width to the original margin value
            container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            container->setMinimumWidth(originalContentsMargins);

            // Setup Widgets container's layout
            QHBoxLayout* containerLayout = new QHBoxLayout(container);
            containerLayout->setContentsMargins(0, 0, 0, 0);
            containerLayout->setSpacing(readingSettings.widgetSpacing);

            for (auto widgetType : widgetTypes) {
                QWidget* widget = nullptr;

                switch (widgetType) {
                    case WidgetTypeEnum::Clock:
                        {
                            TwClockWidgetConfig config {};
                            config.isLeft = isLeft;
                            config.is24hFormat = readingSettings.widgetClock24hFormat;

                            auto clock = new TwClockWidget(config);
                            QObject::connect(pageChangedAdapter, &PageChangedAdapter::pageChanged, clock, &TwClockWidget::updateTime, Qt::UniqueConnection);
                            widget = clock;
                        }
                        break;
                    case WidgetTypeEnum::Battery:
                        {
                            TwBatteryWidgetConfig config {};
                            config.isDarkMode = darkModeAdapter->getDarkMode();
                            config.isLeft = isLeft;
                            config.defaultStyle = readingSettings.widgetBatteryStyle;
                            config.chargingStyle = readingSettings.widgetBatteryStyleCharging;
                            config.showWhenBelow = readingSettings.widgetBatteryShowWhenBelow;

                            auto battery = new TwBatteryWidget(config, hardwareInterface);
                            QObject::connect(darkModeAdapter, &DarkModeAdapter::darkModeChanged, battery, &TwBatteryWidget::setDarkMode, Qt::UniqueConnection);
                            QObject::connect(pageChangedAdapter, &PageChangedAdapter::pageChanged, battery, &TwBatteryWidget::updateLevel, Qt::UniqueConnection);
                            widget = battery;
                        }
                        break;
                    case WidgetTypeEnum::Separator:
                        {
                            if (!readingSettings.widgetSeparator.isEmpty()) {
                                QLabel* separator = new QLabel();
                                separator->setObjectName(QStringLiteral("twks_label"));
                                separator->setContentsMargins(0, 0, 0, 0);
                                separator->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                                separator->setText(readingSettings.widgetSeparator);
                                widget = separator;
                            }
                        }
                        break;
                    default:
                        break;
                }

                // Add widget to container
                if (widget) {
                    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
                    widget->setContentsMargins(0, 0, 0, 0);
                    containerLayout->addWidget(widget, 0);
                }
            }

            // Insert widgets container into parent layout
            if (isLeft) {
                // Insert left
                container->setContentsMargins(readingSettings.headerFooterMargins, 0, 0, 0);
                parentLayout->insertWidget(0, container, 0, Qt::AlignLeft);
            } else {
                // Insert right
                container->setContentsMargins(0, 0, readingSettings.headerFooterMargins, 0);
                parentLayout->addWidget(container, 0, Qt::AlignRight);
            }

            isLeft = false;
        }

        // Stretch center widget
        parentLayout->setStretch(1, 1);
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

        auto readingSettings = settings.getReadingSettings();

        isDarkMode = darkModeAdapter->getDarkMode();

        QObject::connect(darkModeAdapter, &DarkModeAdapter::darkModeChanged, [](bool dark) {
            isDarkMode = dark;
        });

        QString readingFooterQss = Qss::getContent(QStringLiteral(":/qss/ReadingFooter.qss"));
        QString patchedQss = Qss::copySelectors(readingFooterQss, QStringLiteral("#caption"), QStringList() << QStringLiteral("#twks_label"));

        if (readingSettings.headerFooterHeightScale < 100) {
            patchedQss = Patch::ReadingView::scaleHeaderFooterHeight(patchedQss, readingSettings.headerFooterHeightScale);
        }

        insertWidgets(pageChangedAdapter, darkModeAdapter, header, patchedQss, readingSettings.widgetHeaderLeft, readingSettings.widgetHeaderRight);
        insertWidgets(pageChangedAdapter, darkModeAdapter, footer, patchedQss, readingSettings.widgetFooterLeft, readingSettings.widgetFooterRight);
    }

    void setFooterMargin(QWidget* self, int margin) {
        // Save the original margin
        originalContentsMargins = margin;

        QLayout* layout = self->layout();
        layout->setContentsMargins(0, 0, 0, 0);
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

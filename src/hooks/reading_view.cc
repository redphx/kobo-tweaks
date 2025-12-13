#include "reading_view.h"

namespace ReadingViewHook {
    static TweaksSettings settings;
    static bool isDarkMode = false;

    PageChangedAdapter::PageChangedAdapter(ReadingView *parent) : QObject(parent) {
        if (!QObject::connect(parent, SIGNAL(pageChanged(int)), this, SLOT(notifyPageChanged()), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView11pageChangedEi");
        }
    }

    void PageChangedAdapter::notifyPageChanged() {
        nh_log("page changed");
        pageChanged();
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


    static void insertWidgets(PageChangedAdapter *pageChangedAdapter, DarkModeAdapter *darkModeAdapter, ReadingFooter* footer, QString qss, WidgetTypeEnum leftType, WidgetTypeEnum rightType) {
        bool hasLeft = false;
        auto readingSettings = settings.getReadingSettings();

        HardwareInterface* hardwareInterface = HardwareFactory_sharedInstance();

        footer->setStyleSheet(qss);
        footer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // stretch

        for (auto p : {leftType, rightType}) {
            bool isLeft = p == leftType;
            QWidget* widget;

            switch (p) {
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
                default:
                    continue;
            }

            widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

            QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(footer->layout());
            if (isLeft) {
                // Insert left
                layout->insertWidget(0, widget, 0, Qt::AlignLeft);
                layout->setStretch(0, 0);
                layout->setStretch(1, 1);
            } else {
                // Insert right
                layout->insertWidget(layout->count(), widget, 0, Qt::AlignRight);

                if (hasLeft) {
                    // Lef Mid Right
                    layout->setStretch(1, 1);
                    layout->setStretch(layout->count(), 0);
                } else {
                    // Mid Right
                    layout->setStretch(0, 1);
                    layout->setStretch(layout->count(), 0);
                }
            }

            // Set TimeLabel width to the original margin value
            int layoutMargin = footer->property("twks_margin").toInt();
            widget->setFixedWidth(layoutMargin);
            widget->setContentsMargins(20, 0, 20, 0);
            // Keep space when widget is hidden
            QSizePolicy sp = widget->sizePolicy();
            sp.setRetainSizeWhenHidden(true);
            widget->setSizePolicy(sp);

            if (isLeft) {
                hasLeft = true;
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

        auto readingSettings = settings.getReadingSettings();

        isDarkMode = darkModeAdapter->getDarkMode();

        QObject::connect(darkModeAdapter, &DarkModeAdapter::darkModeChanged, [](bool dark) {
            isDarkMode = dark;
        });

        QString readingFooterQss = Qss::getContent(QStringLiteral(":/qss/ReadingFooter.qss"));
        QString patchedQss = Qss::copySelectors(readingFooterQss, QStringLiteral("#caption"), QStringList() << QStringLiteral("#twks_clock #label") << QStringLiteral("#twks_battery #label"));

        if (readingSettings.headerFooterHeightScale < 100) {
            patchedQss = Patch::ReadingView::scaleHeaderFooterHeight(patchedQss, readingSettings.headerFooterHeightScale);
        }

        insertWidgets(pageChangedAdapter, darkModeAdapter, header, patchedQss, readingSettings.widgetHeaderLeft, readingSettings.widgetHeaderRight);
        insertWidgets(pageChangedAdapter, darkModeAdapter, footer, patchedQss, readingSettings.widgetFooterLeft, readingSettings.widgetFooterRight);
    }

    void setFooterMargin(QWidget* self, int margin) {
        auto readingSettings = settings.getReadingSettings();

        // Store the original margin to "twks_margin"
        self->setProperty("twks_margin", margin);

        QString widgetName = self->objectName();
        QLayout* layout = self->layout();

        WidgetTypeEnum leftType = WidgetTypeEnum::Invalid;
        WidgetTypeEnum rightType = WidgetTypeEnum::Invalid;
        if (widgetName == QStringLiteral("header")) {
            leftType = readingSettings.widgetHeaderLeft;
            rightType = readingSettings.widgetHeaderRight;
        } else {
            leftType = readingSettings.widgetFooterLeft;
            rightType = readingSettings.widgetFooterRight;
        }

        int leftMargin = leftType != WidgetTypeEnum::Invalid ? 0 : margin;
        int rightMargin = rightType != WidgetTypeEnum::Invalid ? 0 : margin;
        layout->setContentsMargins(leftMargin, 0, rightMargin, 0);
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

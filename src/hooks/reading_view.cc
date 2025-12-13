#include "reading_view.h"

namespace ReadingViewHook {
    TweaksSettings* settings;
    const TweaksReadingSettings* readingSettings = nullptr;
    bool isDarkMode = false;
    WidgetRefs refs;

    class GestureContainerEventFilter : public BaseEventFilter {
        bool eventFilter(QObject* obj, QEvent* event) override {
            QEvent::Type type = event->type();
            if (type == QEvent::DynamicPropertyChange) {
                auto *e = static_cast<QDynamicPropertyChangeEvent *>(event);

                const QByteArray name = e->propertyName();
                if (name == QStringLiteral("darkMode")) {
                    isDarkMode = obj->property(name.constData()).toBool();
                    _updateWidgets();
                }
            }

            return BaseEventFilter::eventFilter(obj, event);
        }
    };

    void _insertWidgets(ReadingFooter* footer, QString qss, WidgetTypeEnum leftType, WidgetTypeEnum rightType, bool isDarkMode) {
        bool hasLeft = false;

        HardwareInterface* hardwareInterface = HardwareFactory_sharedInstance();

        footer->setStyleSheet(qss);
        footer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // stretch

        TwClockWidgetConfig clockConfig {};
        TwBatteryWidgetConfig batteryConfig {};

        for (auto p : {leftType, rightType}) {
            bool isLeft = p == leftType;
            QWidget* widget;

            switch (p) {
                case WidgetTypeEnum::Clock:
                    clockConfig.isLeft = isLeft;
                    clockConfig.is24hFormat = readingSettings->widgetClock24hFormat;

                    widget = new TwClockWidget(clockConfig);
                    refs.clockWidget = qobject_cast<TwClockWidget*>(widget);
                    break;
                case WidgetTypeEnum::Battery:
                    batteryConfig.isLeft = isLeft;
                    batteryConfig.isDarkMode = isDarkMode;
                    batteryConfig.defaultStyle = readingSettings->widgetBatteryStyle;
                    batteryConfig.chargingStyle = readingSettings->widgetBatteryStyleCharging;
                    batteryConfig.showWhenBelow = readingSettings->widgetBatteryShowWhenBelow;

                    widget = new TwBatteryWidget(batteryConfig, hardwareInterface);
                    refs.batteryWidget = qobject_cast<TwBatteryWidget*>(widget);
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

    void _updateWidgets() {
        if (refs.clockWidget) {
            refs.clockWidget->updateTime();
        }

        if (refs.batteryWidget) {
            refs.batteryWidget->updateLevel(isDarkMode);
        }
    }

    ReadingView* constructor(ReadingView* self) {
        // Must parse settings before constructor
        settings = new TweaksSettings();
        readingSettings = &settings->getReadingSettings();

        auto view = ReadingView_constructor(self);

        refs = WidgetRefs{};
        refs.readingView = view;

        QWidget* gestureContainer = view->findChild<QWidget*>(QStringLiteral("gestureContainer"), Qt::FindDirectChildrenOnly);
        if (!gestureContainer) {
            nh_log("could not find \"gestureContainer\"");
            return view;
        }

        // Find "header"
        QWidget* header = gestureContainer->findChild<ReadingFooter*>(QStringLiteral("header"), Qt::FindDirectChildrenOnly);
        QWidget* footer = gestureContainer->findChild<ReadingFooter*>(QStringLiteral("footer"), Qt::FindDirectChildrenOnly);
        if (!header || !footer) {
            nh_log("could not find \"header/footer\"");
            return view;
        }

        refs.gestureContainer = gestureContainer;
        refs.header = header;
        refs.footer = footer;

        // Detect Dark mode
        isDarkMode = gestureContainer->property("darkMode").toBool();

        QString readingFooterQss = Qss::getContent(QStringLiteral(":/qss/ReadingFooter.qss"));
        QString patchedQss = Qss::copySelectors(readingFooterQss, QStringLiteral("#caption"), QStringList() << QStringLiteral("#twks_clock #label") << QStringLiteral("#twks_battery #label"));

        if (readingSettings->headerFooterHeightScale < 100) {
            patchedQss = Patch::ReadingView::scaleHeaderFooterHeight(patchedQss, readingSettings->headerFooterHeightScale);
        }

        _insertWidgets(refs.header, patchedQss, readingSettings->widgetHeaderLeft, readingSettings->widgetHeaderRight, isDarkMode);
        _insertWidgets(refs.footer, patchedQss, readingSettings->widgetFooterLeft, readingSettings->widgetFooterRight, isDarkMode);

        // Register event filter for "darkMode" property
        gestureContainer->installEventFilter(new GestureContainerEventFilter());

        return view;
    }

    void pageChanged(ReadingView* self, int page) {
        // nh_log("hook_ReadingView_pageChanged");

        _updateWidgets();
        ReadingView_pageChanged(self, page);
    }

    void setFooterMargin(QWidget* self, int margin) {
        // Store the original margin to "twks_margin"
        self->setProperty("twks_margin", margin);

        QString widgetName = self->objectName();
        QLayout* layout = self->layout();

        WidgetTypeEnum leftType = WidgetTypeEnum::Invalid;
        WidgetTypeEnum rightType = WidgetTypeEnum::Invalid;
        if (widgetName == QStringLiteral("header")) {
            leftType = readingSettings->widgetHeaderLeft;
            rightType = readingSettings->widgetHeaderRight;
        } else {
            leftType = readingSettings->widgetFooterLeft;
            rightType = readingSettings->widgetFooterRight;
        }

        int leftMargin = leftType != WidgetTypeEnum::Invalid ? 0 : margin;
        int rightMargin = rightType != WidgetTypeEnum::Invalid ? 0 : margin;
        layout->setContentsMargins(leftMargin, 0, rightMargin, 0);
    }

    namespace DogEarDelegate {
        QWidget* constructor(QWidget* self, QWidget* parent, const QString& orgImgPath) {
            QString imgPath = settings->getReadingBookmarkImage(isDarkMode);
            if (imgPath.isEmpty()) {
                imgPath = orgImgPath;
            }
            auto view = DogEarDelegate_constructor(self, parent, imgPath);
            return view;
        }
    }
}

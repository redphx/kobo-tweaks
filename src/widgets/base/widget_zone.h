#pragma once
#include "../../common.h"
#include "../../settings/settings.h"
#include "../../adapters/reading_view.h"

#include "../../widgets/clock_widget.h"
#include "../../widgets/battery_widget.h"
#include "../../widgets/book_page_widget.h"
#include "../../widgets/book_progress_widget.h"
#include "../../widgets/book_time_widget.h"
#include "../../widgets/book_title_widget.h"
#include "../../widgets/chapter_page_widget.h"
#include "../../widgets/chapter_progress_widget.h"
#include "../../widgets/chapter_time_widget.h"
#include "../../widgets/chapter_title_widget.h"
#include "../../widgets/separator_label.h"


#include <QHBoxLayout>

class TwWidgetZone : public QWidget {
    Q_OBJECT

public:
    TwWidgetZone(TweaksReadingSettings stt, ReadingView* readingView, ReadingViewAdapters adapters, const QString& contentTitle, QVector<WidgetTypeEnum> widgetTypes, bool isRight = false, QWidget* parent = nullptr) : QWidget(parent), readingSettings(stt) {
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // container->setStyleSheet("border: 1px solid black;");

        // Setup Widgets container's layout
        if (!widgetTypes.isEmpty()) {
            lay = new QHBoxLayout(this);
            lay->setContentsMargins(0, 0, 0, 0);
            lay->setSpacing(readingSettings.widgetSpacing);

            setupWidgets(readingView, adapters, contentTitle, widgetTypes, isRight);
        }
    }

private:
    QHBoxLayout* lay;
    TweaksReadingSettings readingSettings;

    void setupWidgets(ReadingView* readingView, ReadingViewAdapters adapters, const QString& contentTitle, QVector<WidgetTypeEnum> widgetTypes, bool isRight) {
        if (isRight) {
            lay->addStretch(1);
        }

        int idx = -1;
        for (auto widgetType : widgetTypes) {
            ++idx;
            if (idx > 0 && readingSettings.widgetSeparator != WidgetSeparatorEnum::Invalid) {
                // Add separator label
                TwSeparatorLabel* separator = new TwSeparatorLabel(readingSettings.widgetSeparator);
                lay->addWidget(separator, 0);
            }

            QWidget* widget = nullptr;
            bool stretch = false;

            switch (widgetType) {
                case WidgetTypeEnum::Clock:
                    {
                        TwClockWidgetConfig config {};
                        config.is24hFormat = readingSettings.widgetClock24hFormat;

                        auto clockWidget = new TwClockWidget(config);
                        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, clockWidget, &TwClockWidget::updateTime, Qt::UniqueConnection);
                        widget = clockWidget;
                    }
                    break;
                case WidgetTypeEnum::Battery:
                    {
                        HardwareInterface* hardwareInterface = HardwareFactory_sharedInstance();

                        TwBatteryWidgetConfig config {};
                        config.isDarkMode = adapters.darkMode->getDarkMode();
                        config.defaultStyle = readingSettings.widgetBatteryStyle;
                        config.chargingStyle = readingSettings.widgetBatteryStyleCharging;
                        config.showWhenBelow = readingSettings.widgetBatteryShowWhenBelow;

                        auto batteryWidget = new TwBatteryWidget(config, hardwareInterface);
                        QObject::connect(adapters.darkMode, &ReadingViewAdapter::DarkMode::darkModeChanged, batteryWidget, &TwBatteryWidget::setDarkMode, Qt::UniqueConnection);
                        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, batteryWidget, &TwBatteryWidget::updateLevel, Qt::UniqueConnection);
                        widget = batteryWidget;
                    }
                    break;
                case WidgetTypeEnum::BookPage:
                    {
                        TwBookPageConfig config {};

                        auto tmpWidget = new TwBookPageWidget(readingView, adapters, config);
                        tmpWidget->init();

                        widget = tmpWidget;
                    }
                    break;
                case WidgetTypeEnum::BookProgress:
                    {
                        TwBookProgressConfig config {};
                        config.showIcon = false;
                        config.isDarkMode = adapters.darkMode->getDarkMode();

                        auto tmpWidget = new TwBookProgressWidget(readingView, adapters, config);
                        tmpWidget->init();

                        widget = tmpWidget;
                    }
                    break;
                case WidgetTypeEnum::BookTime:
                    {
                        TwBookTimeConfig config {};
                        config.showIcon = false;
                        config.isDarkMode = adapters.darkMode->getDarkMode();

                        auto tmpWidget = new TwBookTimeWidget(readingView, adapters, config);
                        tmpWidget->init();

                        widget = tmpWidget;
                    }
                    break;
                case WidgetTypeEnum::BookTitle:
                    {
                        auto bookTitleWidget = new TwBookTitleWidget(contentTitle);
                        widget = bookTitleWidget;
                        stretch = true;
                    }
                    break;
                case WidgetTypeEnum::ChapterPage:
                    {
                        TwChapterPageConfig config {};

                        auto tmpWidget = new TwChapterPageWidget(readingView, adapters, config);
                        tmpWidget->init();

                        widget = tmpWidget;
                    }
                    break;
                case WidgetTypeEnum::ChapterProgress:
                    {
                        TwChapterProgressConfig config {};
                        config.showIcon = false;
                        config.isDarkMode = adapters.darkMode->getDarkMode();

                        auto tmpWidget = new TwChapterProgressWidget(readingView, adapters, config);
                        tmpWidget->init();

                        widget = tmpWidget;
                    }
                    break;
                case WidgetTypeEnum::ChapterTime:
                    {
                        TwChapterTimeConfig config {};
                        config.showIcon = false;
                        config.isDarkMode = adapters.darkMode->getDarkMode();

                        auto tmpWidget = new TwChapterTimeWidget(readingView, adapters, config);
                        tmpWidget->init();

                        widget = tmpWidget;
                    }
                    break;
                case WidgetTypeEnum::ChapterTitle:
                    {
                        auto chapterTitleWidget = new TwChapterTitleWidget();
                        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, chapterTitleWidget, [readingView, chapterTitleWidget]() {
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
                lay->addWidget(widget, stretch ? 1 : 0);
            }
        }
    }
};

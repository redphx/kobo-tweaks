#pragma once
#include "../../common.h"
#include "../../settings/settings.h"
#include "../../adapters/reading_view.h"

#include "../../widgets/clock_widget.h"
#include "../../widgets/battery_widget.h"
#include "../../widgets/book_page_widget.h"
#include "../../widgets/book_title_widget.h"
#include "../../widgets/chapter_page_widget.h"
#include "../../widgets/chapter_progress_widget.h"
#include "../../widgets/chapter_title_widget.h"
#include "../../widgets/separator_label.h"


#include <QHBoxLayout>

class TwWidgetZone : public QWidget {
    Q_OBJECT

public:
    TwWidgetZone(TweaksReadingSettings stt, QWidget* parent = nullptr) : QWidget(parent), readingSettings(stt) {
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // container->setStyleSheet("border: 1px solid black;");

        // Setup Widgets container's layout
        lay = new QHBoxLayout(this);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(readingSettings.widgetSpacing);
    }

    void setupWidgets(ReadingView* readingView, ReadingViewAdapters adapters, const QString& contentTitle, TweaksReadingSettings readingSettings, QVector<WidgetTypeEnum> widgetTypes) {
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
                        auto bookPageWidget = new TwBookPageWidget();
                        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, [readingView, bookPageWidget]() {
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
                        auto bookTitleWidget = new TwBookTitleWidget(contentTitle);
                        widget = bookTitleWidget;
                        stretch = true;
                    }
                    break;
                case WidgetTypeEnum::ChapterPage:
                    {
                        auto chapterPageWidget = new TwChapterPageWidget();
                        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, [readingView, chapterPageWidget]() {
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
                        config.isDarkMode = adapters.darkMode->getDarkMode();

                        auto chapterProgressWidget = new TwChapterProgressWidget(config);

                        QObject::connect(adapters.darkMode, &ReadingViewAdapter::DarkMode::darkModeChanged, chapterProgressWidget, &TwChapterProgressWidget::setDarkMode, Qt::UniqueConnection);
                        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, [readingView, chapterProgressWidget]() {
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
                        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, [readingView, chapterTitleWidget]() {
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

private:
    QHBoxLayout* lay;
    TweaksReadingSettings readingSettings;
};

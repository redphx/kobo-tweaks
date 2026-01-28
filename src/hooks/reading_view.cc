#include "reading_view.h"
#include "../adapters/reading_view.h"

#include <QHBoxLayout>

namespace ReadingViewHook {
    static TweaksSettings settings;
    static bool isDarkMode = false;
    static int originalContentsMargins = 0;

    QString contentTitle;

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

        // Find topSpacer/bottomSpacer
        QWidget* topSpacer = gestureContainer->findChild<ReadingFooter*>(QStringLiteral("topSpacer"), Qt::FindDirectChildrenOnly);
        QWidget* bottomSpacer = gestureContainer->findChild<ReadingFooter*>(QStringLiteral("bottomSpacer"), Qt::FindDirectChildrenOnly);
        if (!topSpacer || !bottomSpacer) {
            nh_log("could not find \"topSpacer/bottomSpacer\"");
            return;
        }

        // Update QSS
        auto readingSettings = settings.getReadingSettings();
        QString rootQss = view->styleSheet();

        // Adjust topSpacer & bottomSpacer's heights
        bool emptyHeader = readingSettings.widgetHeaderLeft.isEmpty() && readingSettings.widgetHeaderCenter.isEmpty() && readingSettings.widgetHeaderRight.isEmpty();
        bool emptyFooter = readingSettings.widgetFooterLeft.isEmpty() && readingSettings.widgetFooterCenter.isEmpty() && readingSettings.widgetFooterRight.isEmpty();

        // Set heights of topSpacer and bottomSpacer
        if (emptyHeader) {
            rootQss = Patch::ReadingView::setFixedHeight(rootQss, QStringLiteral("#topSpacer"), readingSettings.headerSpacerHeight);
        } else {
            rootQss = Patch::ReadingView::resetHeight(rootQss, QStringLiteral("#topSpacer"));
        }

        if (emptyFooter) {
            rootQss = Patch::ReadingView::setFixedHeight(rootQss, QStringLiteral("#bottomSpacer"), readingSettings.footerSpacerHeight);
        } else {
            rootQss = Patch::ReadingView::resetHeight(rootQss, QStringLiteral("#bottomSpacer"));
        }

        rootQss = Patch::ReadingView::addBrightnessLabelQss(rootQss);
        view->setStyleSheet(rootQss);

        // These adapters abstract the logic and ensure that the update methods on the widgets aren't called after either the widget or the ReadingView has been destroyed
        auto renderVolumeAdapter = new ReadingViewAdapter::RenderVolume(view);
        QObject::connect(renderVolumeAdapter, &ReadingViewAdapter::RenderVolume::renderVolume, view, [](const Volume& volume) {
            Content_getTitle(&contentTitle, &volume);
        });

        auto darkModeAdapter = new ReadingViewAdapter::DarkMode(gestureContainer, view);
        isDarkMode = darkModeAdapter->getDarkMode();
        QObject::connect(darkModeAdapter, &ReadingViewAdapter::DarkMode::darkModeChanged, view, [](bool dark) {
            isDarkMode = dark;
        });

        auto readerDoneLoadingAdapter = new ReadingViewAdapter::ReaderDoneLoading(view);

        ReadingViewAdapters adapters {};
        adapters.pageChanged = new ReadingViewAdapter::PageChanged(view);
        adapters.darkMode = darkModeAdapter;
        adapters.renderVolume = renderVolumeAdapter;
        adapters.readerDoneLoading = readerDoneLoadingAdapter;

        // Patch QSS
        QString readingFooterQss = Qss::getContent(QStringLiteral(":/qss/ReadingFooter.qss"));
        QString patchedQss = Qss::copySelectors(readingFooterQss, QStringLiteral("#caption"), QStringList() << QStringLiteral("#twksLabel") << QStringLiteral("#twksSeparator"));
        if (readingSettings.headerFooterHeightScale < 100) {
            patchedQss = Patch::ReadingView::scaleHeaderFooterHeight(patchedQss, readingSettings.headerFooterHeightScale);
        }
        patchedQss.replace(QStringLiteral("ReadingFooter"), QStringLiteral("TwWidgetZonesContainer"));

        TwWidgetZonesContainer* headerContainer = emptyHeader ? nullptr : new TwWidgetZonesContainer(readingSettings, patchedQss);
        TwWidgetZonesContainer* footerContainer = emptyFooter ? nullptr : new TwWidgetZonesContainer(readingSettings, patchedQss);

        if (headerContainer) {
            // add to topSpacer
            QHBoxLayout* layout = new QHBoxLayout(topSpacer);
            layout->setContentsMargins(0, readingSettings.headerSpacerHeight, 0, 0);
            layout->addWidget(headerContainer, 1);
        }

        if (footerContainer) {
            // add to bottomSpacer
            QHBoxLayout* layout = new QHBoxLayout(bottomSpacer);
            layout->setContentsMargins(0, 0, 0, readingSettings.footerSpacerHeight);
            layout->addWidget(footerContainer, 1);
        }

        QObject::connect(readerDoneLoadingAdapter, &ReadingViewAdapter::ReaderDoneLoading::readerDoneLoading, view, [view, gestureContainer, adapters, readingSettings, headerContainer, footerContainer] {
            int minimumSideWidth = qMax(10, originalContentsMargins - readingSettings.headerFooterMargins);
            if (headerContainer) {
                headerContainer->setupZones(view, adapters, contentTitle, minimumSideWidth, readingSettings.widgetHeaderLeft, readingSettings.widgetHeaderCenter, readingSettings.widgetHeaderRight);
            }

            if (footerContainer) {
                footerContainer->setupZones(view, adapters, contentTitle, minimumSideWidth, readingSettings.widgetFooterLeft, readingSettings.widgetFooterCenter, readingSettings.widgetFooterRight);
            }
        });

        // Create a QLabel for showing "Brightness" text
        // Add directly to #gestureContainer
        QLabel* label = new QLabel(gestureContainer);
        label->hide();
        label->setObjectName(QStringLiteral("twksBrightnessLabel"));
        label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        label->move(20, 20);
    }

    void setFooterMargin(QWidget* self, int margin) {
        // Save the original margin
        originalContentsMargins = margin;

        QLayout* layout = self->layout();
        layout->setContentsMargins(margin, 0, margin, 0);
    }

    namespace DogEarDelegate {
        void constructor(QWidget* self, QWidget* parent, const QString& orgImgPath) {
            QString imgPath = settings.getReadingBookmarkImage(isDarkMode);
            if (imgPath.isEmpty()) {
                imgPath = orgImgPath;
            }

            DogEarDelegate_constructor(self, parent, imgPath);
        }
    }

    namespace AdobeReader {
        void constructor(QWidget* self, QWidget* parent, PluginState* state, const QString& orgImgPath) {
            QString imgPath = settings.getReadingBookmarkImage(isDarkMode);
            if (imgPath.isEmpty()) {
                imgPath = orgImgPath;
            }

            AdobeReader_constructor(self, parent, state, imgPath);
        }
    }

    namespace BrightnessEventFilterHook {
        QLabel* findBrightnessLabel(BrightnessEventFilter* self) {
            // Check cached label
            QObject* cachedObj = self->property("cachedLabel").value<QObject*>();
            if (cachedObj) {
                return static_cast<QLabel*>(cachedObj);
            }

            // Fint QLabel
            void* mwc = MainWindowController_sharedInstance();
            QWidget* view = MainWindowController_currentView(mwc);
            QWidget* gestureContainer = view->findChild<GestureReceivingContainer*>(QStringLiteral("gestureContainer"), Qt::FindDirectChildrenOnly);
            if (!gestureContainer) {
                return nullptr;
            }

            QLabel* label = gestureContainer->findChild<QLabel*>(QStringLiteral("twksBrightnessLabel"), Qt::FindDirectChildrenOnly);
            if (label) {
                // Cache
                self->setProperty("cachedLabel", QVariant::fromValue((QObject*)label));
                // Remove cached property when the label is destroyed
                QObject::connect(label, &QObject::destroyed, self, [self]() {
                    self->setProperty("cachedLabel", QVariant());
                });
            }

            return label;
        }

        void updateBrightnessHeader(BrightnessEventFilter* self, const QString& text, const QString&) {
            self->setProperty("pendingText", text);

            QTimer* hideTimer = self->findChild<QTimer*>(QStringLiteral("hideTimer"));
            if (hideTimer) {
                hideTimer->stop();
            } else {
                hideTimer = new QTimer(self);
                hideTimer->setObjectName(QStringLiteral("hideTimer"));
                hideTimer->setSingleShot(true);

                QObject::connect(hideTimer, &QTimer::timeout, self, [self]() {
                    QLabel* label = findBrightnessLabel(self);
                    if (label) {
                        label->hide();
                    }
                });
            }

            QTimer* showTimer = self->findChild<QTimer*>(QStringLiteral("showTimer"));
            if (!showTimer) {
                showTimer = new QTimer(self);
                showTimer->setObjectName(QStringLiteral("showTimer"));
                showTimer->setSingleShot(true);
                showTimer->setInterval(25);

                QObject::connect(showTimer, &QTimer::timeout, self, [self, hideTimer]() {
                    QLabel* label = findBrightnessLabel(self);
                    if (!label) {
                        return;
                    }

                    QString finalText = self->property("pendingText").toString();

                    label->show();
                    label->setText(finalText);
                    label->adjustSize();

                    hideTimer->start(2000);
                });
            }

            showTimer->start();
        }
    }
}

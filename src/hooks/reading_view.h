#pragma once
#include <QWidget>
#include <QString>

#include "../common.h"
#include "../patches.h"
#include "../widgets/clock_widget.h"
#include "../widgets/battery_widget.h"


namespace ReadingViewHook {
    class PageChangedAdapter : public QObject {
        Q_OBJECT

    public:
        explicit PageChangedAdapter(ReadingView *parent);

    private slots:
        void notifyPageChanged();

    signals:
        void pageChanged();
    };

    class ReaderDoneLoadingAdapter : public QObject {
        Q_OBJECT

    public:
        explicit ReaderDoneLoadingAdapter(ReadingView *parent);

    private slots:
        void notifyReaderDoneLoading();

    signals:
        void readerDoneLoading();
    };

    class DarkModeAdapter : public QObject {
        Q_OBJECT

    public:
        explicit DarkModeAdapter(GestureReceivingContainer *parent, ReadingView *view);

        bool getDarkMode();

    private slots:
        void notifyDarkModeChanged();

    signals:
        void darkModeChanged(bool dark);
    };

    class RenderVolumeAdapter : public QObject {
        Q_OBJECT

    public:
        explicit RenderVolumeAdapter(ReadingView* view);

    private slots:
        void notifyRenderVolume(const Volume& volume);

    signals:
        void renderVolume(const Volume& volume);
    };

    void constructor(ReadingView* self);
    void setFooterMargin(QWidget* self, int margin);

    namespace DogEarDelegate {
        QWidget* constructor(QWidget* self, QWidget* parent, const QString& image);
    }
}

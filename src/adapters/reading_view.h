#pragma once
#include "../common.h"

namespace ReadingViewAdapter {
    // PageChanged signal
    class PageChanged : public QObject {
        Q_OBJECT

    public:
        explicit PageChanged(ReadingView *parent);

    private slots:
        void notifyPageChanged();

    signals:
        void pageChanged();
    };

    // ReaderDoneLoading signal
    class ReaderDoneLoading : public QObject {
        Q_OBJECT

    public:
        explicit ReaderDoneLoading(ReadingView *parent);

    private slots:
        void notifyReaderDoneLoading();

    signals:
        void readerDoneLoading();
    };

    // DarkMode signal
    class DarkMode : public QObject {
        Q_OBJECT

    public:
        explicit DarkMode(GestureReceivingContainer *parent, ReadingView *view);

        bool getDarkMode();

    private slots:
        void notifyDarkModeChanged();

    signals:
        void darkModeChanged(bool dark);
    };

    // RenderVolume signal
    class RenderVolume : public QObject {
        Q_OBJECT

    public:
        explicit RenderVolume(ReadingView* view);

    private slots:
        void notifyRenderVolume(const Volume& volume);

    signals:
        void renderVolume(const Volume& volume);
    };
}

struct ReadingViewAdapters {
    ReadingViewAdapter::PageChanged* pageChanged;
    ReadingViewAdapter::DarkMode* darkMode;
    ReadingViewAdapter::RenderVolume* renderVolume;
    ReadingViewAdapter::ReaderDoneLoading* readerDoneLoading;
};

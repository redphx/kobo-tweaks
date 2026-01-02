#include "reading_view.h"
#include <QVariant>

namespace ReadingViewAdapter {
    PageChanged::PageChanged(ReadingView *parent) : QObject(parent) {
        if (!QObject::connect(parent, SIGNAL(pageChanged(int)), this, SLOT(notifyPageChanged()), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView11pageChangedEi");
        }
    }

    void PageChanged::notifyPageChanged() {
        // nh_log("page changed");
        pageChanged();
    }

    ReaderDoneLoading::ReaderDoneLoading(ReadingView *parent) : QObject(parent) {
        if (!QObject::connect(parent, SIGNAL(readerDoneLoading()), this, SLOT(notifyReaderDoneLoading()), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView17readerDoneLoadingEv");
        }
    }

    void ReaderDoneLoading::notifyReaderDoneLoading() {
        // nh_log("readerDoneLoading");
        readerDoneLoading();
    }

    DarkMode::DarkMode(GestureReceivingContainer *parent, ReadingView *view) : QObject(parent) {
        if (!QObject::connect(view, SIGNAL(darkModeChangedSignal()), this, SLOT(notifyDarkModeChanged()), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView21darkModeChangedSignalEv");
        }
    }

    bool DarkMode::getDarkMode() {
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

    void DarkMode::notifyDarkModeChanged() {
        auto dark = getDarkMode();
        nh_log("dark mode changed (%s)", dark ? "dark" : "light");
        darkModeChanged(dark);
    }

    RenderVolume::RenderVolume(ReadingView *parent) : QObject(parent) {
        if (!QObject::connect(parent, SIGNAL(renderVolume(const Volume&)), this, SLOT(notifyRenderVolume(const Volume&)), Qt::UniqueConnection)) {
            nh_log("failed to connect _ZN11ReadingView12renderVolumeERK6Volume");
        }
    }

    void RenderVolume::notifyRenderVolume(const Volume& volume) {
        renderVolume(volume);
    }
}

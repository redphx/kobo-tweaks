#pragma once
#include <QWidget>
#include <QString>

#include "../common.h"
#include "../patches.h"
#include "../widgets/base/widget_zone.h"
#include "../widgets/base/widget_zones_container.h"


namespace ReadingViewHook {
    void constructor(ReadingView* self);
    void setFooterMargin(QWidget* self, int margin);

    namespace DogEarDelegate {
        void constructor(QWidget* self, QWidget* parent, const QString& image);
    }

    namespace AdobeReader {
        void constructor(QWidget* self, QWidget* parent, PluginState* state, const QString& image);
    }

    namespace BrightnessEventFilterHook {
        void updateBrightnessHeader(BrightnessEventFilter* self, const QString& text, const QString& sth);
    }
}

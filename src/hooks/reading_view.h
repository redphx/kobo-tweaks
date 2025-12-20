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

    void constructor(ReadingView* self);
    void setFooterMargin(QWidget* self, int margin);

    namespace DogEarDelegate {
        void constructor(QWidget* self, QWidget* parent, const QString& image);
    }

    namespace AdobeReader {
        void constructor(QWidget* self, QWidget* parent, PluginState* state, const QString& image);
    }
}

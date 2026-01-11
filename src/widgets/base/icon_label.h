#pragma once
#include "../../common.h"
#include "../../utils.h"
#include "../../adapters/reading_view.h"

#include <QLabel>
#include <QHBoxLayout>

struct TwIconLabelConfig {
    bool isDarkMode = false;
    bool showIcon = false;
    int spacing = 6;
};

class TwIconLabel : public QWidget {
    Q_OBJECT

public:
    TwIconLabel(ReadingView* rdv, ReadingViewAdapters adapters, TwIconLabelConfig cfg, QWidget* parent = nullptr) : QWidget(parent), readingView(rdv), currentConfig(cfg) {
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

        iconLabel = new QLabel();
        iconLabel->setContentsMargins(0, 0, 0, 0);
        iconLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        textLabel = new QLabel();
        textLabel->setObjectName(QStringLiteral("twksLabel"));
        textLabel->setContentsMargins(0, 0, 0, 0);
        textLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        lay = new QHBoxLayout(this);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(6);
        lay->addWidget(iconLabel, 0);
        lay->addWidget(textLabel, 0);
        setLayout(lay);

        QObject::connect(adapters.darkMode, &ReadingViewAdapter::DarkMode::darkModeChanged, this, &TwIconLabel::onDarkModeChanged, Qt::UniqueConnection);
        QObject::connect(adapters.pageChanged, &ReadingViewAdapter::PageChanged::pageChanged, this, &TwIconLabel::onPageChanged, Qt::UniqueConnection);

        show();
    }

    void init() {
        loadIcon();
        lastConfig = currentConfig;
    }

    void onDarkModeChanged(bool state) {
        currentConfig.isDarkMode = state;
        updateDisplay();
    }

    virtual void onPageChanged() = 0;

protected:
    ReadingView* readingView;

    QLabel* iconLabel;
    QLabel* textLabel;
    QHBoxLayout* lay;

    TwIconLabelConfig currentConfig;
    TwIconLabelConfig lastConfig;

    virtual QString iconSrc() const = 0;
    virtual QString iconDarkSrc() const = 0;

    void loadIcon() {
        if (!currentConfig.showIcon) {
            iconLabel->hide();
            return;
        }

        QPixmap icon(currentConfig.isDarkMode ? iconDarkSrc() : iconSrc());
        if (icon.isNull()) {
            iconLabel->hide();
            return;
        }

        iconLabel->setPixmap(icon);
        iconLabel->setFixedSize(icon.size());
        iconLabel->show();
    }

    void updateDisplay() {
        if (currentConfig.isDarkMode != lastConfig.isDarkMode) {
            lastConfig.isDarkMode = currentConfig.isDarkMode;
            loadIcon();
        }
    }

    void syncSeparatorVisibility() {
        WidgetUtils::syncSeparatorVisibility(this);
    }
};

#pragma once

#include "../common.h"
#include "../utils.h"
#include "../settings/settings.h"

#include <QtMath>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QEvent>

struct TwBatteryWidgetConfig {
    bool isDarkMode = false;
    bool isLeft = false;
    BatteryStyleEnum defaultStyle = BatteryStyleEnum::IconLevel;
    BatteryStyleEnum chargingStyle = BatteryStyleEnum::IconLevel;
    int showWhenBelow = 100;
};

class TwBatteryWidget : public QWidget {
    Q_OBJECT

    enum class ChargingState { Unplugged, Charging, Charged };

    QLabel* iconLabel = nullptr;
    QLabel* levelLabel = nullptr;

    bool isLeft = false;
    int currentDarkMode = false;
    int currentBatteryLevel = -1;
    ChargingState currentChargingState = ChargingState::Unplugged;
    BatteryStyleEnum currentStyle = BatteryStyleEnum::Invalid;
    BatteryStyleEnum defaultStyle = BatteryStyleEnum::Invalid;
    BatteryStyleEnum chargingStyle = BatteryStyleEnum::Invalid;
    int showWhenBelow = 100;

    HardwareInterface* hw = nullptr;
    GetBatteryLevelFn getBatteryLevelFn = nullptr;
    ChargingStateFn chargingStateFn = nullptr;

    int lastBatteryLevel = -100;
    ChargingState lastChargingState = ChargingState::Unplugged;
    int lastDarkMode = false;
    BatteryStyleEnum lastStyle = BatteryStyleEnum::Invalid;

public:
    explicit TwBatteryWidget(TwBatteryWidgetConfig config, HardwareInterface* h, QWidget* parent = nullptr)
        : QWidget(parent), isLeft(config.isLeft), currentDarkMode(config.isDarkMode), defaultStyle(config.defaultStyle), chargingStyle(config.chargingStyle), showWhenBelow(config.showWhenBelow), hw(h)
    {
        setObjectName(QStringLiteral("twks_battery"));

        // getBatteryLevelFn(blFn), chargingStateFn(csFn);
        QString hardwareType = hw->metaObject()->className();
        if (hardwareType == QStringLiteral("MTK")) {
            getBatteryLevelFn = MTK_getBatteryLevel;
            chargingStateFn = MTK_chargingState;
        } else if (hardwareType == QStringLiteral("iMX508")) {
            getBatteryLevelFn = iMX508_getBatteryLevel;
            chargingStateFn = iMX508_chargingState;
        } else if (hardwareType == QStringLiteral("AllWinner")) {
            getBatteryLevelFn = AllWinner_getBatteryLevel;
            chargingStateFn = AllWinner_chargingState;
        }

        iconLabel = new QLabel();
        iconLabel->setContentsMargins(0, 0, 0, 0);
        iconLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        // iconLabel->setStyleSheet("border: 1px solid black;");

        levelLabel = new QLabel();
        levelLabel->setObjectName(QStringLiteral("label"));
        levelLabel->setContentsMargins(0, 0, 0, 0);
        levelLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        // levelLabel->setStyleSheet("border: 1px solid black;");

        updateLevel();
        show();
    }

    void setDarkMode(bool dark) {
        currentDarkMode = dark;
        updateDisplay();
    }

    void updateLevel() {
        pollBatteryLevel();
        pollChargingState();
        updateDisplay();
    }

protected:
    bool event(QEvent* e) override {
        if (e->type() == QEvent::Show) {
            updateLevel();
        }

        return QWidget::event(e);
    }

private:
    void updateDisplay() {
        // Hide layout when level is above the threshold
        if (currentChargingState == ChargingState::Unplugged && currentBatteryLevel > showWhenBelow) {
            hide();
            return;
        }

        show();

        currentStyle = currentChargingState == ChargingState::Unplugged ? defaultStyle : chargingStyle;
        if (currentStyle != lastStyle) {
            refreshStyle();
            lastStyle = currentStyle;
        }

        if (lastDarkMode == currentDarkMode && lastBatteryLevel == currentBatteryLevel && lastChargingState == currentChargingState) {
            return;
        }

        lastBatteryLevel = currentBatteryLevel;
        lastChargingState = currentChargingState;
        lastDarkMode = currentDarkMode;

        int pixmapIndex = getPixmapIndex(currentBatteryLevel);
        QString iconPath;
        switch (currentChargingState) {
            case ChargingState::Charged:
                iconPath = QStringLiteral(":/kobo_tweaks/images/battery_charged.png");
                currentBatteryLevel = 100;
                break;
            case ChargingState::Charging:
                iconPath = QStringLiteral(":/kobo_tweaks/images/battery_charging_%1.png").arg(pixmapIndex);
                break;
            default:
                iconPath = QStringLiteral(":/kobo_tweaks/images/battery_%1.png").arg(pixmapIndex);
                break;
        }

        if (currentDarkMode) {
            iconPath = Utils::appendFileName(iconPath, QStringLiteral("_dark"));
        }

        QPixmap icon(iconPath);
        iconLabel->setPixmap(icon);
        iconLabel->setFixedSize(icon.size());

        QString text;
        if (currentBatteryLevel > 0) {
            text = QStringLiteral("%1\%").arg(currentBatteryLevel);
        } else {
            text = QStringLiteral("??%");
        }
        levelLabel->setText(text);
    }

    void refreshStyle() {
        // Delete old layout
        iconLabel->setParent(nullptr);
        levelLabel->setParent(nullptr);
        QLayout* oldLayout = layout();
        if (oldLayout) {
            setLayout(nullptr);
            delete oldLayout;
        }

        // Create a new layout
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(6);
        setLayout(layout);

        // Align right -> stretch first
        if (!isLeft) {
            layout->addStretch(1);
        }

        if (currentStyle == BatteryStyleEnum::LevelIcon) {
            layout->addWidget(levelLabel);
            layout->addWidget(iconLabel);
        } else if (currentStyle == BatteryStyleEnum::Level) {
            layout->addWidget(levelLabel);
        } else if (currentStyle == BatteryStyleEnum::Icon) {
            layout->addWidget(iconLabel);
        } else {
            layout->addWidget(iconLabel);
            layout->addWidget(levelLabel);
        }

        // Align left -> stretch last
        if (isLeft) {
            layout->addStretch(1);
        }
    }

    void pollBatteryLevel() {
        int level = -1;
        if (getBatteryLevelFn) {
            level = getBatteryLevelFn(hw);
        }
        currentBatteryLevel = level;
    }

    void pollChargingState() {
        // 0: unplugged, 1: plugged in, 2: fully charged
        ChargingState state = ChargingState::Unplugged;
        if (chargingStateFn) {
            switch (chargingStateFn(hw)) {
                case 2:
                    state = ChargingState::Charged;
                    break;
                case 1:
                    state = ChargingState::Charging;
                    break;
                default:
                    state = ChargingState::Unplugged;
                    break;
            }
        }
        currentChargingState = state;
    }

    int getPixmapIndex(int batteryLevel) {
        return qBound(1, (batteryLevel + 9) / 10, 10);
    }
};

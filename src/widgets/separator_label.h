#pragma once
#include "../common.h"
#include "../settings/settings.h"

#include <QLabel>

class TwSeparatorLabel : public QLabel {
    Q_OBJECT

public:
    TwSeparatorLabel(WidgetSeparatorEnum type, QWidget* parent = nullptr) : QLabel(parent) {
        setObjectName(QStringLiteral("twksSeparator"));
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

        QString symbol;
        switch (type) {
            case WidgetSeparatorEnum::Bullet:
                symbol = QStringLiteral("•");
                break;
            case WidgetSeparatorEnum::Dot:
                symbol = QStringLiteral("·");
                break;
            case WidgetSeparatorEnum::Pipe:
                symbol = QStringLiteral("|");
                break;
            default:
                break;
        }

        setText(symbol);
    }
};

#include <QVariant>

#include "keyboard.h"

QPointer<QWidget> globalPopupKeyboardController = nullptr;
namespace SearchKeyboardControllerHook {
    void popupKeyboard(SearchKeyboardController* self, VirtualKey* key, QVector<KeyboardLayoutRow> rows) {
        // Kobo only renders the first row of keys correctly (the rest are inserted into PopupKeyboard without wrapping inside a layout)
        // To fix this, we only let Kobo render the first row. But we also put other rows as empty lists so the popup is prepared correctly
        const QString keyboardLocale = "vi-VN";

        QVector<KeyboardLayoutRow> additionalRows;
        const QString& label = *reinterpret_cast<QString*>(VirtualKey_text(key));

        // Get custom popup keys
        if (KEYBOARD_MAP.contains(keyboardLocale)) {
            const auto& popupMap = KEYBOARD_MAP[keyboardLocale].popupMap;
            if (popupMap.contains(label)) {
                // Remove old keys
                rows.clear();

                int i = -1;
                auto popupRows = popupMap.value(label);
                for (const auto& popupRow : popupRows) {
                    ++i;
                    KeyboardLayoutRow keysRow;

                    // keyId (0xffff0000) can be the same for every keys, as long as it's not from special keys
                    for (const char* keyLabel : popupRow) {
                        keysRow.keys.append(SearchKeyboardController_newKey(self, keyLabel,  0xffff0000, 10));
                    }

                    if (i == 0) {
                        rows.append(keysRow);
                    } else {
                        // Add empty row so Kobo increases NickelTouchMenu's height
                        rows.append(KeyboardLayoutRow());
                        additionalRows.append(keysRow);
                    }
                }
            }
        }

        // Let Kobo setup the keys
        SearchKeyboardController_popupKeyboard(self, key, rows);

        if (additionalRows.isEmpty() || !globalPopupKeyboardController || !PopupKeyboardController_menu) {
            globalPopupKeyboardController = nullptr;
            return;
        }

        NickelTouchMenu* menu = PopupKeyboardController_menu(globalPopupKeyboardController);
        globalPopupKeyboardController = nullptr;

        PopupKeyboard* popupKeyboard = menu->findChild<PopupKeyboard*>(QString());
        if (!popupKeyboard) {
            return;
        }

        int rowsCount = additionalRows.size();
        QSize* keySize = VirtualKeyboard_keySize(popupKeyboard);
        QVBoxLayout* rootLayout = qobject_cast<QVBoxLayout*>(popupKeyboard->layout());
        // Disable layout updates
        rootLayout->setEnabled(false);
        rootLayout->setSpacing(0);

        for (int i = 0; i < rowsCount; ++i) {
            QHBoxLayout* hRow = new QHBoxLayout();
            hRow->setContentsMargins(0, 0, 0, 0);
            hRow->setSpacing(0);

            // Add every key in this row to the horizontal layout
            const QVector<VirtualKey*>& keys = additionalRows[i].keys;
            for (VirtualKey* k : keys) {
                k->setContentsMargins(0, 0, 0, 0);
                k->setFixedSize(*keySize);

                hRow->addWidget(k);
            }

            rootLayout->addLayout(hRow);
        }

        // Enable layout updates again
        rootLayout->setEnabled(true);
        // Increate popup's height
        popupKeyboard->setFixedSize(popupKeyboard->width(), popupKeyboard->height() * (1 + rowsCount));
    }
}

namespace PopupKeyboardControllerHook {
    void constructor(PopupKeyboardController* self, QWidget* parent, VirtualKeyboard* keyboard, QVector<KeyboardLayoutRow> rows) {
        PopupKeyboardController_constructor(self, parent, keyboard, rows);
        globalPopupKeyboardController = self;
    }
}

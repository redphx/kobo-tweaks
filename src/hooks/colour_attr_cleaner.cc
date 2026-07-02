#include "colour_attr_cleaner.h"
#include "../common.h"

// resolvedColourAttrs() is defined in hooks/reading_view.cc and declared here
// so this translation unit can call it without duplicating the logic.
const std::vector<Qt::WidgetAttribute>& resolvedColourAttrs();

ColourAttrCleaner::ColourAttrCleaner(QWidget* view, QObject* parent)
    : QObject(parent), m_view(view) {}

void ColourAttrCleaner::onFooterMenuClosed() {
    if (m_view) {
        for (auto attr : resolvedColourAttrs())
            m_view->setAttribute(attr, false);
    }
}

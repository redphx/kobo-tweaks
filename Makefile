include NickelHook/NickelHook.mk

override PKGCONF  += Qt5Widgets
override LIBRARY  := libtweaks.so
override SOURCES  += src/tweaks.cc src/common.cc src/patches.cc src/utils.cc src/settings/settings.cc src/debug_utils.cc src/hooks/reading_view.cc
override MOCS     += src/tweaks.h src/patches.h src/utils.h src/settings/settings.h src/widgets/battery_widget.h src/widgets/clock_widget.h src/hooks/reading_view.h
override QRCS     += src/resources.qrc
override CFLAGS   += -Wall -Wextra -Werror
override CXXFLAGS += -Wall -Wextra -Werror -Wno-missing-field-initializers

include NickelHook/NickelHook.mk

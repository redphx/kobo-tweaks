include NickelHook/NickelHook.mk

override PKGCONF  += Qt5Widgets
override LIBRARY  := libtweaks.so
override QRCS     += src/resources.qrc
override CFLAGS   += -Wall -Wextra -Werror
override CXXFLAGS += -Wall -Wextra -Werror -Wno-missing-field-initializers
override KOBOROOT += assets/DELETE_TO_UNINSTALL.txt:/mnt/onboard/.adds/tweaks/DELETE_TO_UNINSTALL.txt

override SOURCES += \
	src/tweaks.cc \
	src/common.cc \
	src/patches.cc \
	src/utils.cc \
	src/settings/settings.cc \
	src/debug_utils.cc \
	src/hooks/reading_view.cc

override MOCS += \
	src/tweaks.h \
	src/patches.h \
	src/utils.h \
	src/settings/settings.h \
	src/widgets/base/elided_label.h \
	src/widgets/battery_widget.h \
	src/widgets/book_page_widget.h \
	src/widgets/book_title_widget.h \
	src/widgets/chapter_page_widget.h \
	src/widgets/chapter_progress_widget.h \
	src/widgets/chapter_title_widget.h \
	src/widgets/clock_widget.h \
	src/widgets/separator_label.h \
	src/hooks/reading_view.h

include NickelHook/NickelHook.mk

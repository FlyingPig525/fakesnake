# ----------------------------
# Makefile Options
# ----------------------------

NAME = FakSnak
ICON = icon.png
DESCRIPTION = "Snake adaptation"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

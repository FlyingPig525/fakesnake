# ----------------------------
# Makefile Options
# ----------------------------

NAME = FakeSnak
ICON = icon.png
DESCRIPTION = "Snake but fake and weird"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

VERSION = 0.2.3

# nmake \
!ifndef 0 # \
CFLAGS = /DFMAKE_VERSION=\"$(VERSION)\"  # \
!if "$(type)" == "release" # \
CFLAGS = $(CFLAGS) /ZI /DNDEBUG /O2 # \
!else # \
CFLAGS = $(CFLAGS) /Zi # \
!endif # \
RM=del # \
!else
type?=debug
debugCFLAGS = -g
releaseCFLAGS = -O2
CFLAGS ?= -DFMAKE_VERSION=\"$(VERSION)\" -Wall -Wextra $($(type)CFLAGS)
PREFIX = /usr/local
# \
!endif


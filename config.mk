VERSION = 0.2.3

EXPORT = export
# nmake \
!ifndef 0 # \
CFLAGS = /DFMAKE_VERSION=\"$(VERSION)\"  # \
EXPORT = set # \
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
CFLAGS = $(CLAGS) -DFMAKE_VERSION=\"$(VERSION)\" -Wall -Wextra $($(type)CFLAGS)
PREFIX = /usr/local
RM=rm
.DEFAULT_GOAL := fmake
.MAIN := fmake
default: fmake
# \
!endif

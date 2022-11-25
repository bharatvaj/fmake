#include <stdint.h>

typedef enum {
	FMAKE_POSIX_MAKEFILE,
	FMAKE_GNU_MAKEFILE,
	FMAKE_BSD_MAKEFILE,
	FMAKE_CMAKE,
} maker_t;

typedef struct {
	const char* filename;
	maker_t type;
	const char* cmd;
	const char* args;
} maker_config_t;

static const char* cmdlists[] = {
	"make",
	"gmake",
	"bmake",
};

#define multiple_(ARG) \
	ARG, cmdlists[ARG]

static const maker_config_t makers[] = {
	{ "Makefile",    FMAKE_POSIX_MAKEFILE, "make" },
	{ "makefile",    FMAKE_POSIX_MAKEFILE, "make" },
	{ "GNUMakefile", FMAKE_GNU_MAKEFILE, "gmake"   },
	{ "BSDMakefile", FMAKE_BSD_MAKEFILE, "bmake"   },
	{ "CMakeLists.txt", FMAKE_CMAKE, "cmake"   },
};

static int8_t *detected_indices = 0;
static maker_config_t maker;


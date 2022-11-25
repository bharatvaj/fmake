#include <stdint.h>

typedef enum {
	FMAKE_POSIX_MAKEFILE,
	FMAKE_GNU_MAKEFILE,
	FMAKE_BSD_MAKEFILE,
	FMAKE_AUTOCONF,
	FMAKE_CONFIGURE,
	FMAKE_CMAKE,
	FMAKE_NPM,
	FMAKE_RUST,
	FMAKE_PIP,
} maker_t;

typedef struct {
	const char* filename;
	maker_t type;
	const char* cmd;
	const char* args;
} maker_config_t;

#define multiple_(ARG) \
	ARG, cmdlists[ARG]

static const maker_config_t makers[] = {
	{ "configure",      FMAKE_CMAKE,          "sh",    "configure"   },
	{ "Makefile",       FMAKE_POSIX_MAKEFILE, "make",  ""            },
	{ "makefile",       FMAKE_POSIX_MAKEFILE, "make",  ""            },
	{ "GNUMakefile",    FMAKE_GNU_MAKEFILE,   "gmake", ""            },
	{ "BSDMakefile",    FMAKE_BSD_MAKEFILE,   "bmake", ""            },
	{ "CMakeLists.txt", FMAKE_CMAKE,          "cmake", "-B out/"     },
	{ "package.json",   FMAKE_NPM,          "npm",   "install"     },
	{ "Cargo.toml",     FMAKE_RUST,          "cargo", "install"     },
	{ "setup.py",       FMAKE_PIP,          "pip",   "install ."   },
};

static int8_t *detected_indices = 0;
static maker_config_t maker;


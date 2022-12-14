#include <stdint.h>

typedef enum {
	FMAKE_POSIX_MAKEFILE,
	FMAKE_GNU_MAKEFILE,
	FMAKE_BSD_MAKEFILE,
	FMAKE_NINJA,
	FMAKE_O_MAKEFILE,
	FMAKE_CONFIGURE,
	FMAKE_AUTORECONF,
	FMAKE_CMAKE,
	FMAKE_GN,
	FMAKE_NPM,
	FMAKE_RUST,
	FMAKE_PIP,
	FMAKE_GRADLE,
} maker_t;

typedef struct {
	const char* filename;
	maker_t type;
	const char* cmd;
	const char* args;
} maker_config_t;

static const maker_config_t makers[] = {
{ "Makefile",       FMAKE_POSIX_MAKEFILE, "make",          ""            },
{ "makefile",       FMAKE_POSIX_MAKEFILE, "make",          ""            },
{ "GNUMakefile",    FMAKE_GNU_MAKEFILE,   "gmake",         ""            },
{ "BSDMakefile",    FMAKE_BSD_MAKEFILE,   "bmake",         ""            },
{ "build.ninja",    FMAKE_NINJA,          "ninja",         ""            },
{ "OMakefile",      FMAKE_O_MAKEFILE,     "omake",         ""            },
{ "configure",      FMAKE_CONFIGURE,      "sh",            "configure"   },
{ "configure.ac",   FMAKE_AUTORECONF,     "autoreconf",    "-i"          },
{ "CMakeLists.txt", FMAKE_CMAKE,          "cmake",         "-B out"      },
{ "BUILD.gn",       FMAKE_GN,             "gn",            "gen out"     },
{ "package.json",   FMAKE_NPM,            "npm",           "install"     },
{ "Cargo.toml",     FMAKE_RUST,           "cargo",         "install"     },
{ "setup.py",       FMAKE_PIP,            "pip",           "install ."   },
#ifdef _WIN32
{ "gradlew.bat",    FMAKE_GRADLE,         "./gradlew.bat", ""            },
#endif
{ "gradlew",        FMAKE_GRADLE,         "sh",            "gradlew"     },
};

static maker_config_t maker;


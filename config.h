#include <stdint.h>

typedef enum {
	FMAKE_POSIX_MAKEFILE,
	FMAKE_GNU_MAKEFILE,
	FMAKE_BSD_MAKEFILE,
	FMAKE_NINJA,
	FMAKE_AUTOCONF,
	FMAKE_CONFIGURE,
	FMAKE_CMAKE,
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
{ "configure",      FMAKE_CONFIGURE,      "sh",            "configure"   },
{ "CMakeLists.txt", FMAKE_CMAKE,          "cmake",         "."           },
{ "package.json",   FMAKE_NPM,            "npm",           "install"     },
{ "Cargo.toml",     FMAKE_RUST,           "cargo",         "install"     },
{ "setup.py",       FMAKE_PIP,            "pip",           "install ."   },
#ifdef _WIN32
{ "gradlew.bat",    FMAKE_GRADLE,         "./gradlew.bat", ""            },
#endif
{ "gradlew",        FMAKE_GRADLE,         "sh",            "gradlew"     },
};

static maker_config_t maker;


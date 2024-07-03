#define BUILD_SYSTEMS \
	X(0, "Makefile"            , "make") \
	X(0, "makefile"            , "make") \
	X(0, "GNUMakefile"         , "gmake") \
	X(0, "BSDMakefile"         , "bmake") \
	X(1, "pro"         , "qmake") \
	X(0, "make"                , "sh"            , "make") \
	X(0, "build.sh"            , "sh"            , "build.sh") \
	X(0, "build.ninja"         , "ninja") \
	X(0, "OMakefile"           , "omake") \
	X(0, "configure"           , "sh"            , "configure") \
	X(0, "configure.ac"        , "autoreconf"    , "-fiv") \
	X(0, "CMakeLists.txt"      , "cmake"         , "-B"               , "out/") \
	X(0, "BUILD.gn"            , "gn"            , "gen"              , "out/") \
	X(0, "nob"                 , "./nob"         , "./nob") \
	X(0, "nob.c"               , "cc"            , "cc"               , "./nob.c" , "-o"         , "nob") \
	X(0, "nobuild"             , "./nobuild"     , ) \
	X(0, "nobuild.c"           , "cc"            , "./nobuild.c"      , "-o"      , "nobuild") \
	X(0, "package.json"        , "npm"           , "run") \
	X(0, "Cargo.toml"          , "cargo"         , "build") \
	X(0, "setup.py"            , "pip"           , "install"          , ".") \
	X(0, "gradlew.bat"         , "./gradlew.bat" , "./gradlew.bat") \
	X(0, "gradlew"             , "sh"            , "gradlew") \
	X(0, "PKGBUILD"            , "makepkg"       , "-i")

typedef struct {
short check_extension;
const char* filename;
const char* cmd;
const char* args[256];
} maker_config_t;

static const maker_config_t makers[] = {
#define X(ISEXT, LOOKFOR, CMD, ...) {ISEXT, LOOKFOR, CMD, { CMD, __VA_ARGS__} },
BUILD_SYSTEMS
#undef X
};


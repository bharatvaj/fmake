/*   level    filetype   lookup name         cmd               cmd args */
#define BUILD_SYSTEMS \
	X( 1,     FMAKE_FIL, "Makefile"       , "make") \
	X( 1,     FMAKE_FIL, "makefile"       , "make") \
	X( 1,     FMAKE_FIL, "GNUMakefile"    , "gmake") \
	X( 1,     FMAKE_FIL, "BSDMakefile"    , "bmake") \
	X( 1,     FMAKE_FIL, "Justfile"       , "just") \
	X( 1,     FMAKE_EXT, "*.pro"          , "qmake") \
	X( 1,     FMAKE_EXT, "*.sln"          , "msbuild") \
	X( 1,     FMAKE_EXT, "*.vcxproj"      , "msbuild") \
	X( 1,     FMAKE_FIL, "make"           , "sh"               , "make") \
	X( 1,     FMAKE_FIL, "Gruntfile"      , "grunt") \
	X( 1,     FMAKE_FIL, "build.sh"       , "sh"               , "build.sh") \
	X( 1,     FMAKE_FIL, "build.ninja"    , "ninja") \
	X( 1,     FMAKE_FIL, "OMakefile"      , "omake") \
	X( 2,     FMAKE_FIL, "configure"      , "sh"               , "configure") \
	X( 1,     FMAKE_FIL, "configure.ac"   , "autoreconf"       , "-fiv") \
	X( 2,     FMAKE_FIL, "CMakeLists.txt" , "cmake"            , "-B"           , "out/") \
	X( 2,     FMAKE_FIL, "BUILD.gn"       , "gn"               , "gen"          , "out/") \
	X( 1,     FMAKE_FIL, "BUILD"          , "bazel"            , "build") \
	X( 1,     FMAKE_FIL, "nob"            , "./nob") \
	X( 1,     FMAKE_FIL, "nob.c"          , "cc"               , "./nob.c"      , "-o"      , "nob") \
	X( 1,     FMAKE_FIL, "nobuild"        , "./nobuild"        , ) \
	X( 1,     FMAKE_FIL, "nobuild.c"      , "cc"               , "./nobuild.c"  , "-o"      , "nobuild") \
	X( 1,     FMAKE_FIL, "packages.json"  , "npm"              , "install") \
	X( 1,     FMAKE_DIR, "node_modules"   , "npm"              , "run") \
	X( 1,     FMAKE_FIL, "Cargo.toml"     , "cargo"            , "build") \
	X( 1,     FMAKE_FIL, "setup.py"       , "pip"              , "install"      , ".") \
	X( 1,     FMAKE_FIL, "gradlew.bat"    , "./gradlew.bat") \
	X( 1,     FMAKE_FIL, "gradlew"        , "sh"               , "gradlew") \
	X( 3,     FMAKE_FIL, "APKBUILD"       , "abuild"           , "-r") \
	X( 3,     FMAKE_FIL, "PKGBUILD"       , "makepkg"          , "-i")


static const maker_config_t makers[] = {
#define X(MLEVEL, ISEXT, LOOKFOR, CMD, ...) {MLEVEL, ISEXT, LOOKFOR, \
    CMD, { CMD, __VA_ARGS__}, (sizeof((char*[]){"" __VA_ARGS__}) / sizeof(char**)) },
BUILD_SYSTEMS
#undef X
#undef NUMARGS
};

static const size_t makers_len = sizeof(makers) / sizeof(maker_config_t);

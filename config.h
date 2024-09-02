#define X(BLEVEL, FTYPE, OTYPE, LOOKFOR, CMD, ...) \
          BLEVEL, FTYPE, OTYPE, LOOKFOR, CMD, { CMD, __VA_ARGS__}, \
          (sizeof((char*[]){"" __VA_ARGS__}) / sizeof(char**)) + 1

const maker_config_t cmake_chain = { X(1, FM_FIL, 0, "CMakeCache.txt", "cmake", "--build", ".") };

/*X(level type operation           lookup name         cmd               cmd args) opt chain */
#define BUILD_SYSTEMS \
{ X(1, FM_FIL,               0     , "Makefile"       , "make")  },\
{ X(1, FM_FIL,               0     , "makefile"       , "make")  },\
{ X(1, FM_FIL,               0     , "GNUMakefile"    , "gmake")  },\
{ X(1, FM_FIL,               0     , "BSDMakefile"    , "bmake")  },\
{ X(1, FM_FIL,               0     , "Justfile"       , "just")  },\
{ X(1, FM_FIL,               0     , "OMakefile"      , "omake")  },\
{ X(1, FM_FIL,               0     , "make"           , "sh"               , "make")  },\
{ X(1, FM_FIL,               0     , "build.sh"       , "sh"               , "build.sh")  },\
{ X(1, FM_FIL,               0     , "Gruntfile"      , "grunt")  },\
{ X(1, FM_FIL,               0     , "build.ninja"    , "ninja")  },\
{ X(1, FM_FIL,               0     , "WORKSPACE"      , "bazel"            , "build")  },\
{ X(1, FM_FIL,               0     , "nob"            , "./nob")  },\
{ X(1, FM_FIL,               0     , "nob.c"          , "cc"               , "./nob.c"      , "-o"      , "nob")  },\
{ X(1, FM_FIL,               0     , "nobuild"        , "./nobuild"        , )  },\
{ X(1, FM_FIL,               0     , "nobuild.c"      , "cc"               , "./nobuild.c"  , "-o"      , "nobuild")  },\
{ X(1, FM_FIL,               0     , "packages.json"  , "npm"              , "install")  },\
{ X(1, FM_DIR,               0     , "node_modules"   , "npm"              , "run")  },\
{ X(1, FM_FIL,               0     , "Cargo.toml"     , "cargo"            , "build")  },\
{ X(1, FM_FIL,               0     , "setup.py"       , "pip"              , "install"      , ".")  },\
{ X(1, FM_FIL,               0     , "gradlew.bat"    , "cmd"              , "/c", "gradlew.bat")  },\
{ X(1, FM_FIL,               0     , "gradlew"        , "sh"               , "gradlew")  },\
{ X(1, FM_EXT,               0     , "*.pro"          , "qmake")  },\
{ X(1, FM_EXT,               0     , "*.sln"          , "msbuild")  },\
{ X(1, FM_EXT,               0     , "*.vcxproj"      , "msbuild")  },\
{ X(2, FM_FIL, FM_MKD | FM_CBB     , "configure"      , "sh"               , "../configure")  },\
{ X(2, FM_FIL, FM_CAB              , "CMakeLists.txt" , "cmake"            , "-Bout"), &cmake_chain  },\
{ X(2, FM_FIL, FM_CAB              , "BUILD.gn"       , "gn"               , "gen"          , "out/")  },\
{ X(3, FM_FIL,               0     , "configure.ac"   , "autoreconf"       , "-fiv")  },\
{ X(3, FM_FIL,               0   , "CMakePresets.json", "cmake"            , "--preset", "default")  },\
{ X(3, FM_FIL,               0     , "APKBUILD"       , "abuild"           , "-r")  },\
{ X(3, FM_FIL,               0     , "PKGBUILD"       , "makepkg"          , "-i")}

static const maker_config_t makers[] = {
BUILD_SYSTEMS
};

#undef BUILD_SYSTEMS
#undef X

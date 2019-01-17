cmake_minimum_required(VERSION 3.5)

project(psylib C CXX)

# versioning
set (PSY_VERSION_MAJOR  "0")
set (PSY_VERSION_MINOR  "0")
set (PSY_VERSION_MICRO  "0")
string (CONCAT PSY_VERSION
                "${PSY_VERSION_MAJOR}.${PSY_VERSION_MINOR}"
                )
string (CONCAT PSY_VERSION_STRING
                "PsyLib-${PSY_VERSION_MAJOR}."
                "${PSY_VERSION_MINOR}."
                "${PSY_VERSION_MICRO}"
                )

set (PSY_LIB psy)

# include cmake helper packages
include (CheckIncludeFiles)
include (GenerateExportHeader)
include (InstallRequiredSystemLibraries)

find_package(PkgConfig)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

check_include_files(stdio.h         HAVE_STDIO_H        REQUIRED)
check_include_files(stdlib.h        HAVE_STDLIB_H       REQUIRED)
check_include_files(stdarg.h        HAVE_STDARG_H       REQUIRED)
check_include_files(string.h        HAVE_STRING_H       REQUIRED)
check_include_files(sys/stat.h      HAVE_SYS__STAT_H            )
check_include_files(GL/gl.h         HAVE_GL__GL_H               )
check_include_files(OpenGL/gl.h     HAVE_OPENGL__GL_H           )
check_include_files(GL/glew.h       HAVE_GL__GLEW_H             )
check_include_files(OpenGL/glew.h   HAVE_GL__GLEW_H             )

# Present us with warnings.
if (MSVC)
    add_compile_options(/w4)
else()
    add_compile_options(-W -Wall -Wextra -pedantic)
endif()

# include some packaging information
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "${PSY_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${PSY_VERSION_MINOR}")
set (CPACK_PACKAGE_MAINTAINER    "MD")
set (CPACK_DEBIAN_PACKAGE_MAINTAINER    "MD")
include (CPack)

pkg_check_modules(SEE REQUIRED see_object)

# Add the library source directory.
add_subdirectory(src)

# Add some tests.
add_subdirectory(test)

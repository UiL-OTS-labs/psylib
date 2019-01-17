#
# This file is part of psylib library
# 
# psylib library is free software: you can redistribute it and/or modify
# it under the terms of the Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
# 
# The psylib library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# Lesser General Public License for more details.
# 
# You should have received a copy of the Lesser General Public License
# along with psylib. If not, see <http://www.gnu.org/licenses/>
#

option (BUILD_UNIT_TESTS
        "Whether or not to build the unittests. This requires CUnit, parse_cmd"
        ON
        )

if (BUILD_UNIT_TESTS)
    find_library(LIB_CUNIT cunit)
    
    pkg_check_modules(PARSE_CMD parse_cmd)

    set (UNIT_TEST unittest)
    set (UNIT_SOURCES
         unit-test.c
         error.c
         glerror.c
         glshader.c
         window.c
         globals.c
         )

    set (UNIT_HEADERS suites.h globals.h psy_test_macros.h)

    add_executable(${UNIT_TEST} ${UNIT_SOURCES} ${UNIT_HEADERS})

    target_link_libraries(${UNIT_TEST} ${LIB_CUNIT})
    target_link_libraries(${UNIT_TEST} ${SEE_LDFLAGS})
    target_link_libraries(${UNIT_TEST} ${PARSE_CMD_LDFLAGS})
    target_link_libraries(${UNIT_TEST} ${PSY_LIB})
    target_link_libraries(${UNIT_TEST} ${OPENGL_LIBRARIES})
    
    target_compile_options(${UNIT_TEST}
        PUBLIC
        ${PARSE_CMD_CFLAGS}
        ${SEE_CFLAGS}
        )

    set_property(TARGET ${UNIT_TEST} PROPERTY C_STANDARD 99)
    set_property(TARGET ${UNIT_TEST} PROPERTY C_STANDARD_REQUIRED ON)

    configure_file(
            gl_shaders/test_vertex_shader.vert
            ${CMAKE_CURRENT_BINARY_DIR}/gl_shaders/test_vertex_shader.vert
        COPYONLY
        )
endif()

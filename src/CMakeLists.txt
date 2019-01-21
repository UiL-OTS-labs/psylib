
find_package(see_object)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/psy_config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/psy_config.h"
               )
configure_file(
        psy.pc.in
        "${CMAKE_CURRENT_BINARY_DIR}/psy.pc"
        @ONLY
        )


set(PSY_SOURCES
    Error.c
    psy_init.c
    Shader.c
    ShaderProgram.c
    Window.c
    gl/glad.c
    gl/GLError.c
    )

set(PSY_HEADERS
    Error.h
    psy_init.h
    Shader.h
    ShaderProgram.h
    Window.h
    gl/glad.h
    gl/GLError.h
    )

add_library(${PSY_LIB} SHARED ${PSY_SOURCES} ${PSY_HEADERS})

#enable compiling with C99 standard
set_property(TARGET ${PSY_LIB} PROPERTY C_STANDARD 99)
set_property(TARGET ${PSY_LIB} PROPERTY CXX_STANDARD 11)

set_property(TARGET ${PSY_LIB} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${PSY_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${PSY_LIB} PROPERTIES LINKER_LANGUAGE C)

target_compile_options(${PSY_LIB} PUBLIC ${SEE_CFLAGS})
#target_link_libraries(${PSY_LIB} ${SEE_LIBS})

# trailing white space generates warnings, so trim string
string(STRIP ${SDL2_LIBRARIES} SDL2_WHITE_SPACE_STRIPPED_LIBS)
target_link_libraries(
    ${PSY_LIB}
        ${SDL2_WHITE_SPACE_STRIPPED_LIBS}
        see_object::see_object
        #${OPENGL_gl_LIBRARIES}
    )
target_include_directories(
        ${PSY_LIB}
    PRIVATE
        ${OPENGL_INCLUDE_DIR}
        )

#generate a export header
generate_export_header(${PSY_LIB})

# make sure the configure file can be found.
target_include_directories(${PSY_LIB} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

install (TARGETS ${PSY_LIB}
            LIBRARY DESTINATION "lib/${PSY_LIB}/"
            )
install (FILES ${PSY_HDR} DESTINATION "include/${PSY_LIB}")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${PSY_LIB}_export.h"
         DESTINATION "include/${PSY_LIB}"
        )
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${PSY_LIB}.pc"
         DESTINATION "share/pkgconfig"
        )


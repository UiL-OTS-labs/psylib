/*
 * This file is part of psylib.
 *
 * psylib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * psylib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with psylib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <CUnit/CUnit.h>
#include "../src/ShaderProgram.h"
#include "../src/Window.h"
#include "globals.h"
#include "psy_test_macros.h"

static PsyWindow* g_win = NULL;
static PsyShader* g_vertex_shader = NULL;
static PsyShader* g_fragment_shader = NULL;

static int g_win_x;
static int g_win_y;
static int g_win_width;
static int g_win_height;

static const char* suite_name = "gl shader program";

//static const char* vert_shader_src =
//    "#version 330 core\n"
//    "\n"
//    "layout (location = 0) in vec3 aPos;\n"
//    "\n"
//    "void main()\n"
//    "{\n"
//    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//    "}\n"
//    ;

//static const char* frag_shader_src =
//    "#version 330 core\n"
//    "\n"
//    "out vec4 FragColor;\n"
//    "\n"
//    "void main()\n"
//    "{\n"
//    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//    "}\n"
//    ;

static const char* frag_shader_src_no_main =
    "#version 330 core\n"
    "\n"
    "out vec4 FragColor;\n"
    "\n"
    "void pain()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n"
    ;

static int setup(void)
{
    SeeError*   error   = NULL;
    PsyRect     rect    = {
        {.x = g_win_x, .y = g_win_y},
        {.width = g_win_width, .height = g_win_height}
    };
    FILE* file = NULL;
    int ret;

    const char* vertex_shader[] =  {
        "./gl_shaders/test_vertex_shader.vert",
        "./test/gl_shaders/test_vertex_shader.vert",
        "./gl_shaders/test_vertex_shader_es.vert",
        "./test/gl_shaders/test_vertex_shader_es.vert"
    };

    const char* fragment_shader[] =  {
        "./gl_shaders/test_fragment_shader.frag",
        "./test/gl_shaders/test_fragment_shader.frag",
        "./gl_shaders/test_fragment_shader_es.frag",
        "./test/gl_shaders/test_fragment_shader_es.frag"
    };
    
    ret = psy_window_create_rect(&g_win, rect, &error);
    if (ret != SEE_SUCCESS) {
        fprintf(stderr, "Unable to open window: %s\n", see_error_msg(error));
        return 1;
    }

    psy_window_show(g_win);
    
    ret = psy_shader_create(
        &g_vertex_shader,
        PSY_SHADER_VERTEX,
        &error
        );
    if(ret != SEE_SUCCESS) {
        fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
        goto setup_failure;
    }
    
    ret = psy_shader_create(
        &g_fragment_shader,
        PSY_SHADER_FRAGMENT,
        &error
        );
    if(ret != SEE_SUCCESS) {
        fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
        goto setup_failure;
    }


    for (size_t i = 0;
         i < sizeof(vertex_shader)/sizeof(vertex_shader[0]);
         i++
         )
    {
        file = fopen(vertex_shader[i], "r");
        if (!file)
            continue;

        ret = psy_shader_compile_file(g_vertex_shader, file, &error);
        fclose(file);
        file = NULL;
        if (ret) {
            if (g_settings.verbose)
                fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
            see_object_decref(SEE_OBJECT(error));
            error = NULL;
        }
        else
            break;
    }
    if (!psy_shader_compiled(g_vertex_shader)) {
        fprintf(stderr, "Unable to compile a vertex shader\n");
        goto setup_failure;
    }

    for (size_t i = 0;
         i < sizeof(fragment_shader)/sizeof(fragment_shader[0]);
         i++)
    {
        file = fopen(fragment_shader[i], "r");
        if (!file)
            continue;

        ret = psy_shader_compile_file(g_fragment_shader, file, &error);
        fclose(file);
        file = NULL;
        if (ret) {
            if (g_settings.verbose)
                fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
            see_object_decref(SEE_OBJECT(error));
            error = NULL;
        }
        else
            break;
    }

    if (!psy_shader_compiled(g_fragment_shader)) {
        fprintf(stderr, "Unable to compile a fragment shader\n");
        goto setup_failure;
    }

    return 0;

    setup_failure:
    if (file)
        fclose(file);
    see_object_decref(SEE_OBJECT(g_win));
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(g_vertex_shader));
    see_object_decref(SEE_OBJECT(g_fragment_shader));
    return 1;
}

static int
teardown(void)
{
    if (g_win) {
        see_object_decref(SEE_OBJECT(g_win));
        g_win = NULL;
    }
    if (g_vertex_shader) {
        see_object_decref(SEE_OBJECT(g_vertex_shader));
        g_vertex_shader = NULL;
    }
    if (g_fragment_shader) {
        see_object_decref(SEE_OBJECT(g_fragment_shader));
        g_fragment_shader = NULL;
    }

    return 0;
}

void gl_shader_program_create(void)
{
    int ret;
    SeeError* error = NULL;
    PsyShaderProgram* program = NULL;

    ret = psy_shader_program_create(&program, NULL, NULL, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(error));
        see_object_decref(SEE_OBJECT(error));
        return;
    }
    see_object_decref(SEE_OBJECT(program));
    program = NULL;

    ret = psy_shader_program_create(
        &program,
        g_vertex_shader,
        NULL,
        &error
        );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(error));
        see_object_decref(SEE_OBJECT(error));
        return;
    }
    see_object_decref(SEE_OBJECT(program));
    program = NULL;

    ret = psy_shader_program_create(
        &program,
        NULL,
        g_fragment_shader,
        &error
        );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(error));
        see_object_decref(SEE_OBJECT(error));
        return;
    }
    CU_ASSERT_EQUAL(g_fragment_shader->parent_obj.refcount, 2);
    see_object_decref(SEE_OBJECT(program));
    CU_ASSERT_EQUAL(g_fragment_shader->parent_obj.refcount, 1);
    program = NULL;
}

void gl_shader_program_link(void)
{
    int ret;
    SeeError* error = NULL;
    PsyShaderProgram* program = NULL;

    ret = psy_shader_program_create(
        &program,
        g_vertex_shader,
        g_fragment_shader,
        &error
        );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(g_vertex_shader->parent_obj.refcount, 2);
    CU_ASSERT_EQUAL(g_fragment_shader->parent_obj.refcount, 2);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(error));
        see_object_decref(SEE_OBJECT(error));
        return;
    }

    ret = psy_shader_program_link(program, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(error));
        see_object_decref(SEE_OBJECT(error));
        see_object_decref(SEE_OBJECT(program));
        return;
    }
    CU_ASSERT(psy_shader_program_linked(program))
    // Check whether the shaders are cleaned up.
    CU_ASSERT_EQUAL(psy_shader_program_get_vertex_shader(program), NULL);
    CU_ASSERT_EQUAL(psy_shader_program_get_fragment_shader(program), NULL);
    CU_ASSERT_EQUAL(g_vertex_shader->parent_obj.refcount, 1);
    CU_ASSERT_EQUAL(g_fragment_shader->parent_obj.refcount, 1);

    see_object_decref(SEE_OBJECT(program));
}

void gl_shader_program_src(void)
{
    int ret;
    PsyShaderProgram* program = NULL;
    SeeError*         error = NULL;
    char vertex_source[BUFSIZ];
    char fragment_source[BUFSIZ];

    ret = psy_shader_source(
            g_vertex_shader,
            vertex_source,
            sizeof(vertex_source)
            );
    if (ret)
        goto gl_shader_program_src_error;
    ret = psy_shader_source(
            g_fragment_shader,
            fragment_source,
            sizeof(fragment_source)
            );
    if (ret)
        goto gl_shader_program_src_error;


    ret = psy_shader_program_create(&program, NULL, NULL, &error);

    CU_ASSERT_PTR_NOT_EQUAL(program, NULL);
    CU_ASSERT_PTR_EQUAL(error, NULL);
    if (ret != SEE_SUCCESS) {
        see_object_decref(SEE_OBJECT(error));
        return;
    }

    ret = psy_shader_program_add_vertex_src(program, vertex_source, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        goto gl_shader_program_src_error;
    ret = psy_shader_program_add_fragment_src(program, fragment_source, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        goto gl_shader_program_src_error;

    ret = psy_shader_program_link(program, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        goto gl_shader_program_src_error;

gl_shader_program_src_error:
    if (error) {
        fprintf(stderr, "%s:%s:%s", __FILE__, __func__, see_error_msg(error));
        see_object_decref(SEE_OBJECT(error));
    }
    see_object_decref(SEE_OBJECT(program));
}

void gl_shader_program_failure(void)
{
    int ret;
    PsyShaderProgram* program = NULL;
    SeeError*         error = NULL;
    char vertex_source[BUFSIZ];
    ret = psy_shader_program_create(&program, NULL, NULL, &error);

    CU_ASSERT_PTR_NOT_EQUAL(program, NULL);
    CU_ASSERT_PTR_EQUAL(error, NULL);
    if (ret != SEE_SUCCESS)
        goto gl_shader_program_src_error;

    ret = psy_shader_source(
            g_vertex_shader,
            vertex_source,
            sizeof(vertex_source)
            );
    if (ret != SEE_SUCCESS)
        goto gl_shader_program_src_error;

    ret = psy_shader_program_add_vertex_src(program, vertex_source, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        goto gl_shader_program_src_error;

    ret = psy_shader_program_add_fragment_src(
        program,
        frag_shader_src_no_main,
        &error
        );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        goto gl_shader_program_src_error;

    ret = psy_shader_program_link(program, &error);
    CU_ASSERT_EQUAL(ret, SEE_ERROR_RUNTIME);

gl_shader_program_src_error:
    if (error) {
        if (g_settings.verbose) {
            fprintf(stderr,
                    "Expected error:\n\t%s:%s:%s",
                    __FILE__,
                    __func__,
                    see_error_msg(error)
                    );
        }
        see_object_decref(SEE_OBJECT(error));
    }
    see_object_decref(SEE_OBJECT(program));
}

int add_glshader_program_suite(void)
{
    g_win_width     = g_settings.window_settings.width;
    g_win_height    = g_settings.window_settings.height;
    g_win_x         = g_settings.window_settings.x;
    g_win_y         = g_settings.window_settings.y;

    CU_pSuite suite = CU_add_suite(
        suite_name,
        setup,
        teardown
        );

    CU_pTest test = NULL;

    if (!suite) {
        fprintf(
            stderr,
            "Unable to create suite: \"%s\": %s\n",
            suite_name,
            CU_get_error_msg()
        );
        return CU_get_error();
    }

    PSY_SUITE_ADD_TEST(suite_name, gl_shader_program_create);
    PSY_SUITE_ADD_TEST(suite_name, gl_shader_program_link);
    PSY_SUITE_ADD_TEST(suite_name, gl_shader_program_src);
    PSY_SUITE_ADD_TEST(suite_name, gl_shader_program_failure);

    return 0;
}


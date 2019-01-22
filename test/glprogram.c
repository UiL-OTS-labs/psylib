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

static int setup(void)
{
    SeeError*   error   = NULL;
    PsyRect     rect    = {
        {.x = g_win_x, .y = g_win_y},
        {.width = g_win_width, .height = g_win_height}
    };
    FILE* file = NULL;

    const char* vertex_shader[] =  {
        "./gl_shaders/test_vertex_shader.vert",
        "./test/gl_shaders/test_vertex_shader.vert"
    };

    const char* fragment_shader[] =  {
        "./gl_shaders/test_fragment_shader.frag",
        "./test/gl_shaders/test_fragment_shader.frag"
    };

    int ret = psy_window_create_rect(&g_win, rect, &error);
    if (ret != SEE_SUCCESS)
        return 1;

    psy_window_show(g_win);

    for (size_t i = 0;
         i < sizeof(vertex_shader)/sizeof(vertex_shader[0]);
         i++)
    {
        file = fopen(vertex_shader[i], "r");
        if (file)
            break;
    }

    if (!file)
        goto setup_failure;

    ret = psy_shader_create(
        &g_vertex_shader,
        PSY_SHADER_VERTEX,
        &error);
    if(ret != SEE_SUCCESS) {
        fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
        goto setup_failure;
    }

    ret = psy_shader_compile_file(g_vertex_shader, file, NULL);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
        goto setup_failure;
    }

    fclose(file);
    file = NULL;

    for (size_t i = 0;
         i < sizeof(fragment_shader)/sizeof(fragment_shader[0]);
         i++)
    {
        file = fopen(fragment_shader[i], "r");
        if (file)
            break;
    }

    if (!file)
        goto setup_failure;

    ret = psy_shader_create(
        &g_fragment_shader,
        PSY_SHADER_FRAGMENT,
        &error);
    if(ret != SEE_SUCCESS) {
        fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
        goto setup_failure;
    }

    ret = psy_shader_compile_file(g_fragment_shader, file, NULL);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
        goto setup_failure;
    }

    fclose(file);
    file = NULL;

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
        see_object_decref(SEE_OBJECT(g_win));
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
    see_object_decref(SEE_OBJECT(program));
}

void gl_shader_program_failure(void)
{
    int unused;
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
    PSY_SUITE_ADD_TEST(suite_name, gl_shader_program_failure);

    return 0;
}


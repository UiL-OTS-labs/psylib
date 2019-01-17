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
#include "../src/Shader.h"
#include "../src/Window.h"
#include "globals.h"
#include "psy_test_macros.h"

static const char* suite_name = "glshader";

static PsyWindow* g_win = NULL;
static int g_win_x;
static int g_win_y;
static int g_win_width;
static int g_win_height;

static int create_window_gl(void)
{
    SeeError*   error   = NULL;
    PsyRect     rect    = {
        {.x = g_win_x, .y = g_win_y},
        {.width = g_win_width, .height = g_win_height}
    };

    int ret = psy_window_create_rect(&g_win, rect, &error);
    if (ret != SEE_SUCCESS)
        return 1;

    psy_window_show(g_win);

    return 0;
}

static int destroy_window_gl(void)
{
    if (g_win)
        see_object_decref(SEE_OBJECT(g_win));

    return 0;
}

static void gl_shader_create(void)
{
    int ret;
    SeeError* error = NULL;
    PsyShader* shader = NULL;
    psy_shader_t type;
    GLuint    id;

    ret = psy_shader_create(&shader, PSY_SHADER_VERTEX, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret != SEE_SUCCESS)
        return;

    psy_shader_id(shader, &id);
    psy_shader_type(shader, &type);

    CU_ASSERT_EQUAL(id, 0);
    CU_ASSERT_EQUAL(type, PSY_SHADER_VERTEX);

    see_object_decref(SEE_OBJECT(shader));
    shader = NULL;

    ret = psy_shader_create(&shader, PSY_SHADER_FRAGMENT, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret != SEE_SUCCESS)
        return;

    psy_shader_id(shader, &id);
    psy_shader_type(shader, &type);

    CU_ASSERT_EQUAL(id, 0);
    CU_ASSERT_EQUAL(type, PSY_SHADER_FRAGMENT);

    see_object_decref(SEE_OBJECT(shader));
}

static void gl_shader_compile(void)
{

}

static void gl_shader_compile_file(void)
{
    int ret;
    PsyShader* shader = NULL;
    SeeError*   error = NULL;
    const char* file_name = "./gl_shaders/test_vertex_shader.vert";
    FILE* file = fopen(file_name, "r");

    CU_ASSERT_PTR_NOT_EQUAL(file, NULL);
    if (!file)
        return;


    ret = psy_shader_create(&shader, PSY_SHADER_VERTEX, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        fclose(file);
        return;
    }

    ret = psy_shader_compile_file(shader, file, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);

    see_object_decref(SEE_OBJECT(shader));

    fclose(file);
}

static void gl_shader_compile_failure(void)
{

}



int add_glshader_suite()
{
    g_win_width     = g_settings.window_settings.width;
    g_win_height    = g_settings.window_settings.height;
    g_win_x         = g_settings.window_settings.x;
    g_win_y         = g_settings.window_settings.y;

    CU_pSuite suite = CU_add_suite(
        suite_name,
        create_window_gl,
        destroy_window_gl
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

    PSY_SUITE_ADD_TEST(suite_name, gl_shader_create);
    PSY_SUITE_ADD_TEST(suite_name, gl_shader_compile);
    PSY_SUITE_ADD_TEST(suite_name, gl_shader_compile_file);
    PSY_SUITE_ADD_TEST(suite_name, gl_shader_compile_failure);

    return 0;
}

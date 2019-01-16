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
#include <stdlib.h>
#include <stdio.h>
#include "psy_test_macros.h"
#include <SeeObject.h>
#include "../src/gl/GLError.h"

static const char* suite_name = "PsyGLError";

static void
glerror_create()
{
    PsyGLError* err = NULL;
    int ret = psy_glerror_create(&err);
    CU_ASSERT_PTR_NOT_EQUAL(err, NULL);
    if (ret != SEE_SUCCESS)
        return;
    see_object_decref((SeeObject*) err);
}

static void
glerror_msg()
{
    const char* msg = "Hello, glerror!";
    const char* expected = "OpenGL error: Hello, glerror!";
    PsyGLError* err = NULL;
    int ret = psy_glerror_create(&err);
    CU_ASSERT_PTR_NOT_EQUAL(err, NULL);
    if (ret != SEE_SUCCESS)
        return;

    psy_error_msg_set(PSY_ERROR(err), msg);
    CU_ASSERT_STRING_EQUAL(expected, see_error_msg(SEE_ERROR(err)));
    see_object_decref((SeeObject*) err);
}

static void
glerror_printf()
{
    char buffer[BUFSIZ];
    char *msg = buffer;
    const char* returned_msg = NULL;

    double pi = 3.141592654;
    int even_prime = 2;
    PsyGLError* err = NULL;
    int ret = psy_glerror_create(&err);
    CU_ASSERT_PTR_NOT_EQUAL(err, NULL);
    if (ret != SEE_SUCCESS)
        return;

    const char* fmt = "An irrational number %lf, a non odd prime %d";
    const char* fmtout = "%sAn irrational number %lf, a non odd prime %d";
    snprintf(buffer, sizeof(buffer), fmtout, "OpenGL error: ", pi, even_prime);
    psy_error_printf(PSY_ERROR(err), fmt, pi, even_prime);
    returned_msg = see_error_msg(SEE_ERROR(err));
    CU_ASSERT_STRING_EQUAL(msg, returned_msg);

    see_object_decref(SEE_OBJECT(err));
}

int add_glerror_suite()
{
    CU_pSuite suite = CU_add_suite(suite_name, NULL, NULL);
    CU_pTest test = NULL;

    if (!suite) {
        fprintf(stderr,
                "Unable to create suite: \"%s\": %s\n",
                suite_name,
                CU_get_error_msg()
               );
        return CU_get_error();
    }

    PSY_SUITE_ADD_TEST(suite_name, glerror_create);
    PSY_SUITE_ADD_TEST(suite_name, glerror_msg);
    PSY_SUITE_ADD_TEST(suite_name, glerror_printf);

    return 0;
}

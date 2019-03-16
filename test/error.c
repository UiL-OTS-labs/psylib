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
#include "../src/Error.h"

static const char* suite_name = "PsyError";

static void
error_create()
{
    PsyError* err = NULL;
    int ret = psy_error_create(&err);
    CU_ASSERT_PTR_NOT_EQUAL(err, NULL);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (!err)
        return;
    see_object_decref((SeeObject*) err);
}

static void error_msg()
{
    const char* msg = "Hello, error!";
    PsyError* err = NULL;
    psy_error_create(&err);
    CU_ASSERT_PTR_NOT_EQUAL(err, NULL);
    if (!err)
        return;
    psy_error_msg_set(err, msg);
    CU_ASSERT_STRING_EQUAL(msg, see_error_msg(SEE_ERROR(err)));
    see_object_decref((SeeObject*) err);
}

static void
error_printf()
{
    char buffer[BUFSIZ];
    char *msg = buffer;
    const char* returned_msg = NULL;

    double pi = 3.141592654;
    int even_prime = 2;
    PsyError* err = NULL;
    psy_error_create(&err);
    CU_ASSERT_PTR_NOT_EQUAL(err, NULL);
    if (!err)
        return;

    const char* fmt = "An irrational number %lf, a non odd prime %d";
    snprintf(buffer, sizeof(buffer), fmt, pi, even_prime);
    psy_error_printf(err, fmt, pi, even_prime);
    returned_msg = see_error_msg(SEE_ERROR(err));
    CU_ASSERT_STRING_EQUAL(msg, returned_msg);

    see_object_decref(SEE_OBJECT(err));
}

int add_error_suite()
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

    PSY_SUITE_ADD_TEST(suite_name, error_create);
    PSY_SUITE_ADD_TEST(suite_name, error_msg);
    PSY_SUITE_ADD_TEST(suite_name, error_printf);

    return 0;
}

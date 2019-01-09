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

/**
 * \file psy_test_macros.h add some convenience macros.
 */

#ifndef HAVE_PSY_TEST_MACROS_H
#define HAVE_PSY_TEST_MACROS_H

#include <CUnit/CUnit.h>
#include <stdio.h>

/**
 * Convenience macro to add a new test to a suite.
 *
 * This functions assumes there is a string with the name of the testsuite eg:
 * static const char* suite_name = "PsyError";
 * suite_name
 */
#define PSY_SUITE_ADD_TEST(suite_name, testname)                            \
    test = CU_ADD_TEST(suite, testname);                                    \
    if (!test) {                                                            \
        fprintf(stderr,                                                     \
                "In suite \"%s\", unable to create test: %s\n\t%s",         \
                suite_name,                                                 \
                #testname,                                                  \
                CU_get_error_msg()                                          \
               );                                                           \
        return CU_get_error();                                              \
    }                                                                       \

#endif

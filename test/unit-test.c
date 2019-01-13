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
#include <CUnit/Basic.h>
#include <parse_cmd.h>
#include <stdio.h>
#include "suites.h"
#include "globals.h"
#include "../src/psy_init.h"
#include "../src/Error.h"

int g_verbose = 0;
int g_silent = 0;

cmd_option prog_opts[] = {
    {'h', "help", OPT_FLAG, {0}, "print help and exit"},
    {'v', "verbose", OPT_FLAG, {0}, "print verbose information"},
    {'s', "silent" , OPT_FLAG, {0}, "reduce the verbosity"},
    {'w', "window-width", OPT_INT, {0}, "Define the window width (default 640)"},
    {0,   "window-height", OPT_INT, {0}, "Define the default window height (default 480)"}
};

int add_suites()
{
    if (add_error_suite())
        return 1;
    if (add_glerror_suite())
        return 1;
    if (add_window_suite())
        return 1;

    return 0;
}

int main(int argc, char** argv) {

    option_context* options = NULL;
    PsyError*       error   = NULL;

    options_parse(
            &options,
            argc,
            (const char * const*)argv,
            prog_opts,
            sizeof(prog_opts)/sizeof(prog_opts[0])
            );

    if (option_context_have_option(options, "verbose"))
        g_verbose = 1;
    if (option_context_have_option(options, "silent"))
        g_silent = 1;
    if (option_context_have_option(options, "window-width"))
        option_context_int_value(
            options, "window-width", &g_settings.window_settings.width
            );
    if (option_context_have_option(options, "window-height"))
        option_context_int_value(
            options, "window-height", &g_settings.window_settings.height
            );

//    if (g_verbose && g_silent)
//        g_silent = g_verbose = 0;
    if (g_verbose)
        g_settings.verbose = 1;

    int ret = psylib_init(&error);
    if (ret) {
        fprintf(stderr, "Unable to initialze the psylib: %s\n",
            see_error_msg(SEE_ERROR(error))
            );
        return EXIT_FAILURE;
    }

    if (!options)
        return 1;

    if (CU_initialize_registry() != CUE_SUCCESS) {
        fprintf(stderr, "Unable to initialize the unit testing framework...\n");
        return 1;
    }

    if (add_suites()) {
        fprintf(stderr, "Unable to initialize the unit test suites...\n");
        return 1;
    }

    if (g_verbose)
        CU_basic_set_mode(CU_BRM_VERBOSE);
    if (g_silent)
        CU_basic_set_mode(CU_BRM_SILENT);

    if (CU_basic_run_tests() != CUE_SUCCESS) {
        fprintf(stderr, "Error running the tests...\n");
        return 1;
    }

    /* free everything */
    psylib_deinit();
    CU_cleanup_registry();
    option_context_free(options);

    return 0;
}

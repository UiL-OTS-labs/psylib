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

#ifndef HAVE_SUITES_H
#define HAVE_SUITES_H 1

/*
 * Available test suites.
 */

/**
 * \private
 * \brief Tests whether PsyError works as desired.
 * @returns 0 when the test was properly registered.
 */
int add_error_suite();

/**
 * \private
 * \brief Test whether PsyGLError works
 * @return 0 when the test is successfully registered.
 */
int add_glerror_suite();

/**
 * @private
 * @brief Tests the shaders for opengl
 * @return
 */
int add_glshader_suite();

/**
 * @private
 * @brief Test whether we can link shader programs.
 * @return
 */
int add_glshader_program_suite();

/**
 * \private
 * \brief Tests whether the windowing suite is working as expected.
 *
 * @return 0 when the suite was properly registered.
 */
int add_window_suite();

#endif

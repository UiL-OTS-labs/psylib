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


#ifndef psy_init_H
#define psy_init_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes psylib.
 *
 * In order to use anything from psylib it is mandatory to initialize it.
 * If you don't none of the classes are initialized, and they will be equal
 * to NULL.
 */

int psylib_init();

/**
 * Frees the resources claimed by psylib_init
 *
 * Make sure every other psylib variable/instance is released before calling
 * psylib_free, otherwise you cannot free those instances.
 */
void psylib_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef psy_init_H

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
 * @file globals.h
 * @private
 *
 * This file contains some parameters that are used as default settings.
 * These settings are global and meant for the unit tests only.
 */

#ifndef globals_H
#define globals_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _WindowSettings {
    int x;
    int y;
    int width;
    int height;
}WindowSettings;

typedef struct _GlobalSettings {
    int verbose;
    WindowSettings window_settings;
}GlobalSettings;


extern GlobalSettings g_settings;


#ifdef __cplusplus
}
#endif

#endif //ifndef globals_H

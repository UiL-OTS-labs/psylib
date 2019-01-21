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
 * \file includes platform specific OpenGL libraries.
 */

#ifndef INCLUDES_GL_H
#define INCLUDES_GL_H

#include <psy_config.h>

// This should avoid the inclusion of windows.h on windows.
#ifdef _WIN32
    #define APIENTRY __stdcall
#endif

#include "glad.h"

#endif


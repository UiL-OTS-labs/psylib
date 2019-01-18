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

#include <psy_config.h>

// Don't know whether to use glew or another mechanism to load the OpenGL extensions
//#if defined(HAVE_GL__GLEW_H)
//    #include <GL/glew.h>
//#elif defined(HAVE_OPENGL__GLEW_H)
//    #include <OpenGL/glew.h>
//#else
//    #pragma message  ("no Glew support")
//#endif

#if defined(HAVE_GL__GL_H)
    #include <GL/gl.h>
#elif defined (HAVE__OpenGL__GL_H)
    #include <OpenGL/gl.h>
#else
    #pragma message  ("no gl.h support")
#endif



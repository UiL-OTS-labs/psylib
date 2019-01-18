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
 * \file gl_extensions.h helps with loading the right extensions.
 * \private
 */

#include <SDL2/SDL.h>
#include <Error.h>
#include "includes_gl.h"

typedef enum _load_error {
    PSY_GL_LOAD_OK,
    PSY_GL_LOAD_CONTEXT_EXISTS,
    PSY_GL_LOAD_EXTENSION_MISSING,
    PSY_GL_LOAD_CONTEXT_NOT_CURRENT,
    PSY_GL_LOAD_NO_MEMORY,
}gl_load_error_t;

typedef void (APIENTRY *ext_glShaderSource) (
    GLuint shader,
    GLsizei count,
    const GLchar* const* string,
    const GLint* length
    );

typedef void (APIENTRY *ext_glCompileShader)(GLuint shader);

typedef void (APIENTRY *ext_glGetShaderiv) (
             GLuint shader,
             GLenum pname,
             GLint *params
             );

typedef void (APIENTRY *ext_glGetShaderInfoLog)(
            GLuint shader,
            GLsizei maxLength,
            GLsizei *length,
            GLchar *infoLog
            );
/**
 * \private
 *
 * A struct containing the pointers to the extensions. Every time a OpenGL
 * context is made, there should also be a struct like this one that contains
 * the pointers to the extensions.
 * The user can see whether the extension is supported by examining whether
 * the pointer is NULL.
 */
typedef struct _GLExtensions {
    ext_glShaderSource          glShaderSource;
    ext_glCompileShader         glCompileShader;
    ext_glGetShaderiv           glGetShaderiv;
    ext_glGetShaderInfoLog      glGetShaderInfoLog;
}GLExtensions;

/**
 * \private a pointer to the current g_gl_extensions.
 */
extern GLExtensions* g_gl_extensions;

/**
 * \brief Allocate a new GLExtensions struct.
 *
 * The for each context you can register one struct of GlExtensions
 *
 * @param context  The context should be current, because otherwise
 *                 We either load the wrong functions, or reset the current
 *                 context while someone else is drawing to it.
 * @return
 */
int
allocate_glextension_for_context(SDL_GLContext context);

/**
 * \brief Set the extensions that belong to context as the current
 *        context.
 *
 * Note the current extensions is a global variable, hence it is NOT
 * THREAD SAFE.
 *
 * \private
 *
 * @param context
 * @return
 */
int
set_current_context(SDL_GLContext context, SeeError** error);

/**
 *
 * @param [in]  context
 * @param [out] extensions
 *
 * @return GL_O
 */
int
get_current_context(SDL_GLContext context, GLExtensions** extensions);

void free_glextension_for_context(SDL_GLContext context);
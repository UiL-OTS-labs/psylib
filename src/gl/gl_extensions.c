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
 * \file gl_extensions.c
 *
 * \brief loads the OpenGL extensions for a given context.
 * \private
 */

#include <DynamicArray.h>
#include "gl_extensions.h"

/**
 * \private
 * \brief Stores a SDL_GLContext and a GLExtensions struct.
 */
typedef struct _ExtentionPair {
    SDL_GLContext context;
    GLExtensions  extensions;
}ExtensionPair;


/**
 * \private
 *
 * stores the pairs of the SDL_GL_CONTEXTS
 */
SeeDynamicArray* g_array = NULL;

/**
 * \brief This set of extensions is globally accessable.
 *
 * This means that you can only change this from one single thread.
 */
GLExtensions* g_gl_extensions = NULL;


// casting function pointers generates pedantic errors
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#define LOAD_AND_CHECK_EXTENSION(name)                              \
    ext->name = (ext_##name) SDL_GL_GetProcAddress(#name);          \
    if (ext->name != NULL) {                                        \
        if (SDL_GL_ExtensionSupported(#name) != SDL_TRUE) {         \
            ext->name = NULL;                                       \
            failed_to_load = 1;                                     \
            fprintf(stderr, "failed to load %s\n", #name);          \
        }                                                           \
    }                                                               \


/**
 * \brief This function loads the extensions for a given context.
 *
 * @param [in] The SDL_GLContext that should be active.
 * @param extensions The set of extensions.
 * @return PSY_GL_LOAD_OK if everything is alright.
 */
static int
load_extentions(GLExtensions* ext)
{
    int failed_to_load = 0;

    // Here below we load functions from the opengl library via SDL2
    // via a macro, this is roughly what it does.
    ext->glShaderSource  = SDL_GL_GetProcAddress("glShaderSource");
    if (ext->glShaderSource != NULL) {
        if (SDL_GL_ExtensionSupported("glShaderSource") != SDL_TRUE) {
            ext->glShaderSource = NULL;
            failed_to_load = 1;
        }
    }

    // Is already done here above.
    //LOAD_AND_CHECK_EXTENSION(glShaderSource);
    LOAD_AND_CHECK_EXTENSION(glCompileShader);
    LOAD_AND_CHECK_EXTENSION(glGetShaderiv);
    LOAD_AND_CHECK_EXTENSION(glGetShaderInfoLog);

    if (failed_to_load)
        return PSY_GL_LOAD_EXTENSION_MISSING;
    else
        return PSY_GL_LOAD_OK;
}

//reenable all warnings
#pragma GCC diagnostic pop

int
allocate_glextension_for_context(SDL_GLContext context)
{
    ExtensionPair pair = {
        .context = context,
        .extensions = {0}
    };

    int ret;
    if (context != SDL_GL_GetCurrentContext())
        return PSY_GL_LOAD_CONTEXT_NOT_CURRENT;

    if (g_array) {
        for (size_t i = 0; i < see_dynamic_array_size(g_array); i++) {
            ExtensionPair* pair = see_dynamic_array_get(g_array, i);
            if (pair->context == context)
                return PSY_GL_LOAD_CONTEXT_EXISTS;
        }
    }
    else {
        ret = see_dynamic_array_new(
            &g_array,
            sizeof (ExtensionPair),
            NULL,
            NULL,
            NULL
            );
        if (ret != SEE_SUCCESS)
            return PSY_GL_LOAD_NO_MEMORY;
    }

    ret = load_extentions(&pair.extensions);
    if (see_dynamic_array_add(g_array, &pair) != SEE_SUCCESS)
        return PSY_GL_LOAD_NO_MEMORY;

    return ret;
}


void free_glextension_for_context(SDL_GLContext context)
{
}


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

#include "psy_init.h"
#include <see_init.h>
#include "Error.h"
#include "gl/GLError.h"
#include "Window.h"
#include "Shader.h"
#include <assert.h>
#include <SDL2/SDL.h>

static void
generate_sdl_error(PsyError** error)
{
    if (! error) {
        fprintf(stderr, "Unable to initialize SDL: %s.\n", SDL_GetError());
    }
    else {
        PsyError* err = psy_error_create();
        assert(err != NULL);
        if (!error) {
            fprintf(stderr, "Panic: Unable to initialize SDL and errors are "
                            "unavailable.\n"
            );
        }
        psy_error_printf(err, "%s:\n\t%s\n",
                         "Unable to initialize SDL",
                         SDL_GetError()
        );
        *error = err;
    }
}


// Initialize sdl. When we are using sdl, we can only initialize openGL
// When we have a valid context and in order to obtain a valid context
// we unfortunately need a valid SDL_Window. Hence, we can only initialize
// OpenGL when we have a window...

static int
init_sdl(PsyError** error)
{
    int ret = 0;

    // init SDL's video system.
    int sdl_init_flags = SDL_INIT_VIDEO;
    if (SDL_Init(sdl_init_flags)) {
        generate_sdl_error(error);
        return ret;
    }

    // tell that we want to use the 3.3 openGL core profile to exclude the
    // really old stuff.

    ret = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    if (ret) {
        generate_sdl_error(error);
        return ret;
    }

    ret = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    if (ret) {
        generate_sdl_error(error);
        return ret;
    }

    ret = SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
    );
    if (ret) {
        generate_sdl_error(error);
        return ret;
    }

    ret = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if (ret) {
        generate_sdl_error(error);
        return ret;
    }

/* // Needs a valid context first.
    ret = SDL_GL_SetSwapInterval(1);
    if (ret) {
        generate_sdl_error(error);
        return ret;
    }
*/

    return ret;
}

static void
deinit_external_libs()
{
    SDL_Quit();
}

static int
init_external_libs(PsyError** error)
{
    int ret = 0;

    // Init SDL
    ret = init_sdl(error);
    if (ret)
        return ret;

    // INIT ... library.

    return ret;
}

int psylib_init(PsyError** error)
{
    int ret = 0;

    ret = see_init();
    if (ret)
        return ret;

    if ((ret = psy_error_init()) != 0)
        return ret;
    if ((ret = psy_glerror_init()) != 0)
        return ret;
    if ((ret = psy_shader_init()) != 0)
        return ret;
    if ((ret = psy_window_init()) != 0)
        return ret;

    if ((ret = init_external_libs(error)) != 0)
        return ret;

    return ret;
}

void psylib_deinit()
{
    deinit_external_libs();

    psy_error_deinit();
    psy_glerror_deinit();
    psy_shader_deinit();
    psy_window_deinit();
}

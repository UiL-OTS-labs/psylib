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

#include "psy_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include <SeeObject.h>
#include <MetaClass.h>
#include <assert.h>

#include "Error.h"
#include "Window.h"
#include "gl/includes_gl.h"


// constants

#define PSY_WIN_DEFAULT_FLAGS ( \
    SDL_WINDOW_RESIZABLE |      \
    SDL_WINDOW_OPENGL    |      \
    SDL_WINDOW_SHOWN            \
    )

const PsyRect g_default_window_rect = {
    .pos = {.x = 100, .y = 100},
    .size= {.width = 640, .height = 480}
};

const char* g_default_window_name = "PsyWindow default name";

struct _WindowPrivate {
    SDL_Window*     pwin;
    SDL_GLContext   context;
};

/* **** dynamically linked window functions **** */

static int
window_init(PsyWindow*              win,
            const PsyWindowClass*   cls,
            const char*             name,
            int                     x,
            int                     y,
            int                     w,
            int                     h,
            int                     flags,
            PsyError**              error
            )
{
    WindowPrivate        *priv  = NULL;

    const SeeObjectClass* obj_cls = SEE_OBJECT_CLASS(cls);
    obj_cls->object_init(SEE_OBJECT(win), obj_cls);

    priv = calloc(1, sizeof(WindowPrivate));
    if (!priv) {
        int see_generate_runtime;
        return SEE_RUNTIME_ERROR;
    }

    win->window_priv = priv;

    priv->pwin = SDL_CreateWindow(name, x, y, w, h, flags);
    if (!priv->pwin)
        return SEE_RUNTIME_ERROR;

    priv->context = SDL_GL_CreateContext(priv->pwin);

    SDL_GL_MakeCurrent(priv->pwin, priv->context);
    SDL_GL_SetSwapInterval(1);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    int ret = gladLoadGL();
    if (!ret) { // gladLoadGL() return 1 if succesfull
        psy_error_create(error);
        see_error_set_msg(
                SEE_ERROR(*error),
                "Glad is unable to load openGL."
                );
        return SEE_RUNTIME_ERROR;
    }

    return SEE_SUCCESS;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const char*  name   = va_arg(args, const char*);
    int          x      = va_arg(args, int);
    int          y      = va_arg(args, int);
    int          w      = va_arg(args, int);
    int          h      = va_arg(args, int);
    int          flags  = va_arg(args, int);

    PsyError**   error  = va_arg(args, PsyError**);

    const PsyWindowClass* win_cls = PSY_WINDOW_CLASS(cls);
    return win_cls->window_init(
        PSY_WINDOW(obj),
        win_cls,
        name,
        x,
        y,
        w,
        h,
        flags,
        error
        );
}

static void
window_destroy(SeeObject* obj)
{
    PsyWindow* win = (PsyWindow*) obj;
    WindowPrivate* priv = win->window_priv;
    if (priv) {

        SDL_GL_DeleteContext(priv->context);
        if (priv->pwin) {
            SDL_DestroyWindow(priv->pwin);
            priv->pwin = NULL;
        }
        free(priv);
    }

    // Chain up to the parent to destroy the see_object itself
    see_object_class()->destroy(obj);
}

static int
window_show(PsyWindow* window)
{
    assert(window && window->window_priv);
    if (!window || !window->window_priv)
        return SEE_INVALID_ARGUMENT;
    SDL_ShowWindow(window->window_priv->pwin);
    return 0;
}

static int
window_hide(PsyWindow* window)
{
    assert(window && window->window_priv);
    if (!(window && window->window_priv))
        return SEE_INVALID_ARGUMENT;
    SDL_HideWindow(window->window_priv->pwin);
    return 0;
}

static int
window_swap_buffers(const PsyWindow* window)
{
    assert(window && window->window_priv);
    if (!(window && window->window_priv))
        return SEE_INVALID_ARGUMENT;

    SDL_GL_SwapWindow(window->window_priv->pwin);
    return 0;
}

static int
window_fullscreen(PsyWindow* window, int full)
{
    assert(window && window->window_priv);
    if (!(window && window->window_priv))
        return SEE_INVALID_ARGUMENT;
    /* For now full screen means a undecorated window at the current display
     * resolution. With sdl using SDL_WindowFullscreen means that something
     * in SDL is going to change the current resolution of the monitors
     * and I don't think that is useful.
     */

    SDL_Window* sdl_window = window->window_priv->pwin;

    //Todo error handling on all of these SDL_ functions
    if (full) {
        int display_index = SDL_GetWindowDisplayIndex(sdl_window);
        SDL_Rect display_rect;
        SDL_GetDisplayBounds(display_index, &display_rect);
        SDL_SetWindowBordered(sdl_window, SDL_FALSE);
        SDL_SetWindowPosition(sdl_window, display_rect.x, display_rect.y);
        SDL_SetWindowSize(sdl_window, display_rect.w, display_rect.h);
    }
    else {
        SDL_SetWindowBordered(sdl_window, SDL_TRUE);
    }

    return SEE_SUCCESS;
}

static int
window_get_rect(const PsyWindow* win, PsyRect* out)
{
    SDL_Window* sdl_window = win->window_priv->pwin;
    SDL_GetWindowPosition(sdl_window, &(out->pos.x), &(out->pos.y));
    SDL_GetWindowSize(sdl_window, &(out->size.width), &(out->size.height));
    return SEE_SUCCESS;
}

static int
window_set_rect(PsyWindow* win, PsyRect* in, PsyError** error)
{
    SDL_Window* sdl_window = win->window_priv->pwin;
    SDL_SetWindowPosition(sdl_window, in->pos.x, in->pos.y);
    SDL_SetWindowSize(sdl_window, in->size.width, in->size.height);
    if(error)
        *error = NULL;
    return SEE_SUCCESS;
}

static int
window_get_position(const PsyWindow* window, PsyPos* out)
{
    SDL_Window* sdl_window = window->window_priv->pwin;
    SDL_GetWindowPosition(sdl_window, &(out->x), &(out->y));
    return SEE_SUCCESS;
}

static int
window_set_position(PsyWindow* window, PsyPos* in, PsyError** error)
{
    SDL_Window *sdl_window = window->window_priv->pwin;
    SDL_SetWindowPosition(sdl_window, in->x, in->y);
    if(error)
        *error = NULL;
    return SEE_SUCCESS;
}

static int
window_get_size(const PsyWindow* window, PsySize* out)
{
    SDL_Window* sdl_window = window->window_priv->pwin;
    SDL_GetWindowSize(sdl_window, &(out->width), &(out->height));
    return SEE_SUCCESS;
}

static int
window_set_size(PsyWindow* window, PsySize* in, PsyError** error)
{
    SDL_Window *sdl_window = window->window_priv->pwin;
    SDL_SetWindowSize(sdl_window, in->width, in->height);
    if(error)
        *error = NULL;
    return SEE_SUCCESS;
}

static int
window_id(const PsyWindow* window, uint32_t *out)
{
    SDL_Window* sdl_window = window->window_priv->pwin;
    *out = SDL_GetWindowID(sdl_window);

    return SEE_SUCCESS;
}


/* **** public functions **** */

int
psy_window_create(PsyWindow** window, SeeError** error)
{
    int ret;
    const PsyWindowClass* cls = psy_window_class();
    const SeeObjectClass* see_cls = SEE_OBJECT_CLASS(cls);
    assert(cls);

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!window || *window)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    ret = see_cls->new_obj(
        see_cls,
        0,
        (SeeObject**) window,
        g_default_window_name,
        g_default_window_rect.pos.x,
        g_default_window_rect.pos.y,
        g_default_window_rect.size.width,
        g_default_window_rect.size.height,
        PSY_WIN_DEFAULT_FLAGS,
        error
        );

    return ret;
}

int
psy_window_create_rect(PsyWindow** window, PsyRect rect, SeeError** error) {
    int ret;
    const PsyWindowClass* cls = psy_window_class();
    const SeeObjectClass* see_cls = SEE_OBJECT_CLASS(cls);

    assert(cls);
    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!window || *window)
        return SEE_INVALID_ARGUMENT;

    if (error != NULL && *error)
        return SEE_INVALID_ARGUMENT;

    ret = see_cls->new_obj(
        see_cls,
        0,
        (SeeObject**) window,
        g_default_window_name,
        rect.pos.x,
        rect.pos.y,
        rect.size.width,
        rect.size.height,
        PSY_WIN_DEFAULT_FLAGS,
        error
        );

    return ret;
}

int
psy_window_fullscreen(PsyWindow* win, int full)
{
    const PsyWindowClass* win_cls;
    if (!win)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->fullscreen(win, full);
}

int
psy_window_get_rect(const PsyWindow* win, PsyRect* out)
{
    const PsyWindowClass* win_cls;
    if (!win || !out)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->get_rect(win, out);
}

int
psy_window_set_rect(PsyWindow* win, PsyRect* in, PsyError** error)
{
    const PsyWindowClass* win_cls;
    if (!win || !in)
        return SEE_INVALID_ARGUMENT;

    if (error && *error != NULL)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->set_rect(win, in, error);
}

int
psy_window_get_position(const PsyWindow* win, PsyPos* out)
{
    const PsyWindowClass* win_cls;
    if (!win || !out)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->get_position(win, out);
}

int
psy_window_set_position(PsyWindow* win, PsyPos* in, PsyError** error)
{
    const PsyWindowClass* win_cls;
    if (!win || !in)
        return SEE_INVALID_ARGUMENT;

    if (error && *error != NULL)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->set_position(win, in, error);
}

int
psy_window_get_size(const PsyWindow* win, PsySize* out)
{
    const PsyWindowClass* win_cls;
    if (!win || !out)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->get_size(win, out);
}

int
psy_window_set_size(PsyWindow* win, PsySize* in, PsyError** error)
{
    const PsyWindowClass* win_cls;
    if (!win || !in)
        return SEE_INVALID_ARGUMENT;

    if (error && *error != NULL)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->set_size(win, in, error);
}

int psy_window_show(PsyWindow* win)
{
    const PsyWindowClass* win_cls;

    if (!win)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->show(win);
}

int
psy_window_swap(const PsyWindow* win)
{
    const PsyWindowClass* win_cls;
    if (!win)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->swap_buffers(win);
}

int
psy_window_id(const PsyWindow* win, uint32_t* id)
{
    const PsyWindowClass* win_cls;
    if (!win || !id)
        return SEE_INVALID_ARGUMENT;

    win_cls = PSY_WINDOW_GET_CLASS(win);

    return win_cls->window_id(win, id);
}

/* **** Class management **** */

static PsyWindowClass* g_cls;

int
psy_window_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    // Override parent init and destroy to allocate/free window private
    new_cls->init       = init;
    new_cls->destroy    = window_destroy;

    // override virtual members
    PsyWindowClass* cls = (PsyWindowClass*)new_cls;

    // Populate the own class functions.
    cls->window_init    = window_init;
    cls->show           = window_show;
    cls->hide           = window_hide;
    cls->swap_buffers   = window_swap_buffers;
    cls->fullscreen     = window_fullscreen;
    cls->get_rect       = window_get_rect;
    cls->set_rect       = window_set_rect;
    cls->get_position   = window_get_position;
    cls->set_position   = window_set_position;
    cls->get_size       = window_get_size;
    cls->set_size       = window_set_size;
    cls->window_id      = window_id;

    return ret;
}

int
psy_window_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_cls,
        sizeof(PsyWindowClass),
        sizeof(PsyWindow),
        see_object_class(),
        sizeof(SeeObjectClass),
        psy_window_class_init
        );

    return ret;
}

void
psy_window_deinit()
{
    if(!g_cls)
        return;

    see_object_decref((SeeObject*) g_cls);
    g_cls = NULL;
}

const PsyWindowClass*
psy_window_class()
{
    return g_cls;
}


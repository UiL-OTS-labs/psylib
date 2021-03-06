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
 * \file Window.h
 * \brief  This file implements a window on which can be drawn.
 */

#ifndef Window_H
#define Window_H

#include <SeeObject.h>
#include <psy_export.h>
#include <stdint.h>
#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The size of an object.
 */
typedef struct _PsySize {
    /**
     * \brief The window width.
     */
    int width;
    /**
     * \brief The window height.
     */
    int height;
}PsySize;

/**
 * \brief The position of an object.
 */
typedef struct _PsyPos {
    /**
     * \brief the x position of a window
     */
    int x;

    /**
     * \brief the y postion of a window
     */
    int y;
} PsyPos;

/**
 * \brief a rect to tell something about rectangular regions on the screen
 *
 * A rectangle is best described by a position on the screen with a given
 * size. This rect is suited for discrete units like pixels on the screen.
 * The positions is the position of the upper left corner of the rect.
 * While the lower right corner is the postion.x + the width and position.y +
 * the height of the rectangle.
 */
typedef struct _PsyRect{
    /**
     * \brief The position on which a window can be presented
     */
    PsyPos  pos;

    /**
     * \brief the size of a window.
     */
    PsySize size;
} PsyRect;

struct _PsyWindow;
typedef struct _PsyWindow PsyWindow;
struct _PsyWindowClass;
typedef struct _PsyWindowClass PsyWindowClass;

typedef struct _WindowPrivate WindowPrivate;

/**
 * \brief The data that belongs to a window object. The private part
 * should not be touched.
 * \private
 */
struct _PsyWindow {
    SeeObject       obj;    
    WindowPrivate*  window_priv;
};

/**
 * \brief The PsyWindowClass implements all the methods of the window.
 * \private
 */
struct _PsyWindowClass {
    SeeObjectClass  see_class;
    int (*window_init)  (PsyWindow* win,
                         const PsyWindowClass* cls,
                         const char* name,
                         int x,
                         int y,
                         int w,
                         int h,
                         int flags,
                         PsyError** error
                         );
    int (*show)         (PsyWindow* window);
    int (*hide)         (PsyWindow* window);
    int (*swap_buffers) (const PsyWindow* window);
    int (*fullscreen)   (PsyWindow* window, int full);
    int (*get_rect)     (const PsyWindow* window, PsyRect* rect);
    int (*set_rect)     (PsyWindow* window, PsyRect* rect, PsyError** error);
    int (*get_position) (const PsyWindow* window, PsyPos* rect);
    int (*set_position) (PsyWindow* window, PsyPos* rect, PsyError** error);
    int (*get_size)     (const PsyWindow* window, PsySize* rect);
    int (*set_size)     (PsyWindow* window, PsySize* rect, PsyError** error);
    int (*window_id)    (const PsyWindow* window, uint32_t* out);
    int (*clear_color)  (PsyWindow* window,
                         float r,
                         float g,
                         float b,
                         float a
                         );
    int (*clear)        (PsyWindow* window);
};

/* **** function style macro cast**** */
/**
 * \brief Cast a pointer to an PsyWindow derived object to a PsyWindow*
 */
#define PSY_WINDOW(obj)                                                      \
    ((PsyWindow*)(obj))


/**
 * \brief Cast a pointer to class to a const PsyWindowClass*.
 */
#define PSY_WINDOW_CLASS(cls)                                                \
    ((const PsyWindowClass*) (cls));

/**
 * \brief Get the the class of a PsyWindow instance.
 */
#define PSY_WINDOW_GET_CLASS(obj)                                            \
    PSY_WINDOW_CLASS(see_object_get_class((SeeObject*)(obj)))


/* **** public api **** */

/**
 * \brief Create a new window with default parameters.
 *
 * @param [out] window The newly generated window should be a pointer
 *                     to a PsyWindow pointer that points to NULL.
 * @param [out] error  May be NULL or a pointer to null.
 * @return SEE_SUCCESS when successful or an number indicating the error.
 */
PSY_EXPORT int
psy_window_create(PsyWindow** window, SeeError **out);


/**
 * \brief Create a new window with the position and the dimensions of rect.
 *
 * @param [out] out A pointer to a valid PsyWindow* that points to NULL.
 * @param [in]  r a rectangle describing the rect where you would like to
 *              have the window presented.
 * @param [out] error If an error occurs the a error containing a hopefully
 *              useful message can be returned here.
 * @return SEE_SUCCESS if everything works out or another error value.
 */
PSY_EXPORT int
psy_window_create_rect(PsyWindow** out, PsyRect r, SeeError** error);

/**
 * \brief Attempts to display the window at fullscreen.
 *
 * To present a window at fullscreen does not the same thing as
 * SDL_WindowFullscreen. It removes the window decoration, and presents
 * the window at the same display resolution as the monitor the display
 * is on.
 * If you un fullscreen the window, then the decoration will be enabled
 * once more, but probably the window won't have the same dimensions and
 * postion as before.
 *
 * @param [in,out] window the window we want to modify.
 * @param [in]     full 0 to "un" fullscreen the window (adds window decoration
 *                 back or 1 to maximise the window and remove the decorations
 *                 of the window. any other value would present the window
 *                 at fullscreen.
 *
 * @returns SEE_SUCCESS when the function was called successfully.
 */
PSY_EXPORT int psy_window_fullscreen(PsyWindow* window, int full);

/**
 * \brief obtain the rectangle (position and size) of a PsyWindow.
 *
 * @param [in]  win The window of which you would like to know its dimensions.
 * @param [out] out The returned dimensions.
 *
 * @return SEE_SUCCESS when successful.
 */

PSY_EXPORT int
psy_window_get_rect(const PsyWindow* win, PsyRect* out);

/**
 * \brief change the position and/or the size of the window.
 * @param [in,out] win    The window of which you would like to change the
 *                        position.
 * @param [in]     in     The rectangle displaying the desired dimensions of
 *                        the window.
 * @param [out]    error  If an error occurs the error will be returned here.
 * @return SEE_SUCCESS when successful.
 */
PSY_EXPORT int
psy_window_set_rect(PsyWindow* win, PsyRect* in, PsyError** error);

/**
 * \brief Obtain the current position of the window.
 * @param [in]  win The window of which you would like to know the position
 * @param [out] out returned position of the window
 * @return SEE_SUCCESS when successful.
 */
PSY_EXPORT int
psy_window_get_position(const PsyWindow* win, PsyPos* out);

/**
 * \brief Attempt to move the window to the desired position.
 *
 * @param win The window to move
 * @param out The desired position.
 * @param error An error will be set here if something unexpected happens.
 *
 * @return SEE_SUCCESS if everything worked out.
 */
PSY_EXPORT int
psy_window_set_position(PsyWindow* win, PsyPos* in, PsyError** error);

/**
 * \brief obtain the current size of a window.
 *
 * @param [in]  win
 * @param [out] out The size of the window should be returned here.
 *
 * @return SEE_SUCCESS if everything worked out.
 */
PSY_EXPORT int
psy_window_get_size(const PsyWindow* win, PsySize* out);

/**
 * \brief Modify the size of the current window.
 *
 * @param [in,out]  win
 * @param [in]      in
 * @param [out]     error
 *
 * @return SEE_SUCCESS
 */
PSY_EXPORT int
psy_window_set_size(PsyWindow* win, PsySize* in, PsyError** error);

/**
 * \brief Show a window undo hiding
 *
 * @param [in,out] win the window that should become visible again.
 * @return SEE_SUCCES if everything went alright.
 */
PSY_EXPORT int
psy_window_show(PsyWindow* win);

/**
 * \brief Hides a window.
 * Hide a window
 *
 * \param [in.out] win the window that should be hidden.
 * \return SEE_SUCCESS if the window is successfully hidden.
 */
PSY_EXPORT int
psy_window_hide(PsyWindow* win);

/**
 * \brief swap the current front and back buffers.
 *
 * Swap the front and back buffers. Typically one is rendering on the back-
 * buffer, while the front buffer is on screen. Calling this function will
 * finish all drawing calls and waits until the buffer is swapped. This
 * function should wait for the vertical retrace / vertical blanking interval
 * in order to prevent tearing.
 *
 * @param [in] window The window that is going to swap the buffers.
 * @return SEE_SUCCESS when the buffer swap is completed.
 */
PSY_EXPORT int
psy_window_swap(const PsyWindow* window);


/**
 * Return the window id of the window.
 *
 * @param [in]  window  The window from which we would like to know the id.
 * @param [out] id      The window id
 *
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT.
 */
PSY_EXPORT int psy_window_id(const PsyWindow* window, uint32_t* id);

/**
 * Sets the clearcolor for the window. If the color buffer is cleared this
 * will be the new background.

 * @param [in, out] window The window whose background we would like to change.
 * @param [in] r red usefull range [0 : 1]
 * @param [in] g green usefull range [0 : 1]
 * @param [in] b blue usefull range [0 : 1]
 * @param [in] a alpha usefull range [0 : 1]
 *
 * return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
PSY_EXPORT int psy_window_set_clear_color(
        PsyWindow* window,
        float r,
        float g,
        float b,
        float a
        );

/**
 * @brief clears the window with the current clear color
 *
 * @param [in] window The window whose background we would like to change.
 *
 * return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
PSY_EXPORT int psy_window_clear(PsyWindow* window);

/* *** class related functions *** */

/**
 * \brief Free psy window class.
 *
 * Clean up the psy_window class, after calling this function
 * the functions from this header should not be used anymore.
 */
PSY_EXPORT void
psy_window_deinit();

/**
 *\brief initialize the PsyWindowClass
 */
PSY_EXPORT int
psy_window_init();


/**
 * \brief Get the psy_window_class
 */
PSY_EXPORT const PsyWindowClass*
psy_window_class();

#ifdef __cplusplus
}
#endif

#endif //ifndef Window_H

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

#include <CUnit/CUnit.h>
#include <SeeObject.h>
#include <SDL2/SDL.h>
#include <assert.h>

#include <GL/gl.h>

#include "../src/Window.h"

#include "globals.h"
#include "psy_test_macros.h"

int g_win_x;
int g_win_y;
int g_win_width;
int g_win_height;

static const char* suite_name = "PsyWindow";

/* **** utilities **** */

/**
 * \brief Tests on which display the given position lies.
 * \private
 *
 * This function tests on which display/monitor a given position is. If you
 * have overlapping monitors, it could be possible that a given position is
 * on multiple monitors. If this is the case you can use the start parameter
 * to start at the next index beyond the last returned monitor.
 *
 * @param [in] position a position.
 * @param [in] start at which display do we start. Make sure
 *             that start < SDL_GetNumVideoDisplays().
 * @return -1 if no display is found that includes position, -2 if start is
 *          larger than SDL_GetNumVideoDisplays, 0 or the first encountered
 *          display that contains the position.
 */
 int nth_display_for_position(PsyPos position, unsigned start)
{
    SDL_Rect sdl_rect;
    int n = SDL_GetNumVideoDisplays();
    if ((int)start >= n)
        return -2;

    for (int i = (int) start; i < n; i++) {
        int x = position.x, y = position.y;
        SDL_GetDisplayBounds(i, &sdl_rect);
        if (x >= sdl_rect.x &&
            x < sdl_rect.x + sdl_rect.w &&
            y >= sdl_rect.y &&
            y < sdl_rect.y + sdl_rect.h
            ) {
            return i;
        }
    }
    return -1;
}



/* **** Unit tests **** */

static void window_create(void)
{
    int ret;
    PsyWindow* window = NULL;
    SeeError*  error  = NULL;

    ret = psy_window_create(&window, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_PTR_NOT_EQUAL(window, NULL);
    CU_ASSERT_EQUAL(error, NULL);
    if (ret) {
        fprintf(stderr, "%s", see_error_msg(SEE_ERROR(error)));
        see_object_decref(SEE_OBJECT(error));
        return;
    }

    see_object_decref(SEE_OBJECT(window));
    if (error)
        see_object_decref((SeeObject*) error);
}

static void window_create_rect(void)
{
    PsyWindow*  win     = NULL;
    SeeError*   error   = NULL;
    int x = g_win_x, y = g_win_y, width = g_win_width, height = g_win_height;
    PsyRect r = {
        {x,     y},
        {width, height}
    };
    PsyRect out;

    int ret = psy_window_create_rect(&win, r, &error);
    CU_ASSERT(ret == SEE_SUCCESS);
    CU_ASSERT_PTR_NOT_EQUAL(win, NULL);
    if (ret) {
        fprintf(stderr, "%s:%s:%s\n",
                __FILE__,
                __FUNCTION__,
                see_error_msg(error)
               );
        goto fail;
    }

    psy_window_get_rect(win, &out);
    CU_ASSERT_EQUAL(r.pos.x, out.pos.x);
    CU_ASSERT_EQUAL(r.pos.y, out.pos.y);
    CU_ASSERT_EQUAL(r.size.width,  out.size.width);
    CU_ASSERT_EQUAL(r.size.height, out.size.height);

    psy_window_get_position(win, &out.pos);
    psy_window_get_size(win, &out.size);

fail:

    see_object_decref(SEE_OBJECT(win));
    see_object_decref(SEE_OBJECT(error));
}

static void window_fullscreen(void)
{
    PsyWindow*  win     = NULL;
    SeeError*   error   = NULL;

    int x = g_win_x, y = g_win_y, width = g_win_width, height = g_win_y;
    int ret, n;
    SDL_Rect sdl_rect;
    PsyRect out;

    PsyRect r = {
        {x,     y},
        {width, height}
    };

    ret = psy_window_create_rect(&win, r, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        return;

    psy_window_fullscreen(win, 1);

    // Assume the center of the window lies on the same display as the position.
    // Fingers crossed...
    n = nth_display_for_position(r.pos, 0);
    ret = SDL_GetDisplayBounds(n, &sdl_rect);
    CU_ASSERT_EQUAL(ret, 0);
    if (ret)
        return;

    psy_window_get_rect(win, &out);

    CU_ASSERT_EQUAL(sdl_rect.x, out.pos.x);
    CU_ASSERT_EQUAL(sdl_rect.y, out.pos.y);
    CU_ASSERT_EQUAL(sdl_rect.w, out.size.width);
    CU_ASSERT_EQUAL(sdl_rect.h, out.size.height);

    see_object_decref(SEE_OBJECT(win));
}

static void window_swap_synced(void)
{
    PsyWindow* win = NULL;
    SeeError* error= NULL;
    int x = g_win_x, y = g_win_y, width = g_win_width, height = g_win_height;
    double seconds;
    int ret, n;
    const int N_FRAMES = 60;
    double display_dur;
    SDL_DisplayMode mode;
    SDL_Event event = {0};

    PsyRect r = {
        {x,     y},
        {width, height}
    };

    n = nth_display_for_position(r.pos, 0);
    SDL_GetCurrentDisplayMode(n, &mode);
    display_dur = 1.0/mode.refresh_rate;

    // Clear all events we raise multiple windows in this test
    // and all of them raise events.
    while (SDL_PollEvent(&event))
        ;

    ret = psy_window_create_rect(&win, r, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    int have_expose = 0, have_shown = 0;
    if (ret)
        return;
    psy_window_show(win);

    // Wait until the window is shown and exposed.
    while(!have_expose && !have_shown){
        SDL_WaitEvent(&event);
        if (event.type == SDL_WINDOWEVENT) {

            uint32_t id;
            psy_window_id(win, &id);

            if (event.window.windowID != id)
                continue;
            if (event.window.event == SDL_WINDOWEVENT_SHOWN)
                have_shown = 1;
            if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
                have_expose = 1;
        }
    }

    double* inter_frame_interval = malloc(N_FRAMES * sizeof(double));
    assert(inter_frame_interval);
    //SDL_Delay(17);
    psy_window_set_clear_color(win, 1.0,1.0,1.0,1.0);
    psy_window_clear(win);

    /* for some currently unknown reason swapping the window takes some time
     * to stabilize
     */
    for (int i = 0; i < 60; i++)
        psy_window_swap(win);
    seconds = ((double) SDL_GetTicks()) / 1000.0;

    for (int i = 0; i < N_FRAMES; i++) {
        psy_window_clear(win);
        psy_window_swap(win);
        double now = ((double)SDL_GetTicks())/1000.0;
        inter_frame_interval[i] = now - seconds;
        seconds = now;
    }

    int n_missed = 0;
    double mean = 0.0;

    for (int i = 0; i < N_FRAMES; i++) {
        mean += inter_frame_interval[i];
        double absdur = fabs(inter_frame_interval[i] - display_dur);

        if (absdur >= 0.5 * display_dur)
            n_missed++;
    }
    mean /= N_FRAMES;
    // The mean really should be about equal to the frame rate
    CU_ASSERT_DOUBLE_EQUAL(mean, display_dur, 0.001);
    CU_ASSERT(n_missed <= 2);

    if (g_settings.verbose) {
        fprintf(stdout, "\nPrinting the times of between two window flips "
                        " should be about:%lf\n",
                display_dur
        );
        for (int i = 0 ; i < N_FRAMES; i++)
            fprintf(stdout, "frame %d, ok %d seconds = %lf\n",
                    i,
                    !(fabs(inter_frame_interval[i] - display_dur) >= 0.5 * display_dur),
                    inter_frame_interval[i]
            );
    }

    free(inter_frame_interval);
    see_object_decref(SEE_OBJECT(win));
}

static int rects_equal(PsyRect* r1, PsyRect* r2)
{
    if (r1->pos.x != r2->pos.x)
        return 0;
    if (r1->pos.y != r2->pos.y)
        return 0;
    if (r1->size.width !=  r2->size.width)
        return 0;
    if (r1->size.height != r2->size.height)
        return 0;
    return 1;
}

void window_dimensions(void)
{
    PsyWindow*  win = NULL;
    SeeError*   error = NULL;
    int x = g_win_x, y = g_win_y, width = g_win_width, height = g_win_y;
    int ret;
    PsyRect r = {
        {x,     y},
        {width, height}
    };
    PsyRect new = {
        .pos = {.x = 0, .y = 0},
        .size = {.width = 100, .height = 100}
    };
    PsyRect out = {{0,0},{0,0}};

    ret = psy_window_create_rect(&win, r, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret != SEE_SUCCESS)
        return;

    CU_ASSERT(!rects_equal(&r, &out) && !rects_equal(&r, &new));

    psy_window_set_position(win, &new.pos, NULL);
    psy_window_set_size(win, &new.size, NULL);
    psy_window_get_rect(win, &out);

    CU_ASSERT(rects_equal(&out, &new));
    see_object_decref(SEE_OBJECT(win));
}


int add_window_suite(void)
{
    g_win_width     = g_settings.window_settings.width;
    g_win_height    = g_settings.window_settings.height;
    g_win_x         = g_settings.window_settings.x;
    g_win_y         = g_settings.window_settings.y;

    CU_pSuite suite = CU_add_suite(suite_name, NULL, NULL);
    CU_pTest test = NULL;

    if (!suite) {
        fprintf(
            stderr,
            "Unable to create suite: \"%s\": %s\n",
            suite_name,
            CU_get_error_msg()
        );
        return CU_get_error();
    }

    PSY_SUITE_ADD_TEST(suite_name, window_create);
    PSY_SUITE_ADD_TEST(suite_name, window_create_rect);
    PSY_SUITE_ADD_TEST(suite_name, window_fullscreen);
    PSY_SUITE_ADD_TEST(suite_name, window_swap_synced);
    PSY_SUITE_ADD_TEST(suite_name, window_dimensions);

    return 0;
}

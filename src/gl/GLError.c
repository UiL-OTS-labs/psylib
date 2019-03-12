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
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include <MetaClass.h>

#include "GLError.h"

const char* GL_ERROR_PREFIX = "OpenGL error: ";

/* functions that implement the PsyErrorClass */

static void
glerror_msg_set(SeeError* obj, const char* msg)
{
    PsyError*            error = PSY_ERROR(obj);

    sprintf(error->msg_buffer, "%s", GL_ERROR_PREFIX);

    // Print to the buffer.
    snprintf(
        &error->msg_buffer[0] + strlen(GL_ERROR_PREFIX),
        sizeof(error->msg_buffer) - strlen(GL_ERROR_PREFIX),
        "%s",
        msg);

    SEE_ERROR_CLASS(psy_error_class())->set_msg(obj, msg);
}

static int
glerror_msg_printf(PsyError* error, const char* format, va_list ap)
{
    sprintf(error->msg_buffer, "%s", GL_ERROR_PREFIX);
    int ret = vsnprintf(
        &error->msg_buffer[0] + strlen(GL_ERROR_PREFIX),
        sizeof(error->msg_buffer) - strlen(GL_ERROR_PREFIX),
        format,
        ap
        );

    PSY_ERROR_CLASS(psy_error_class())->error_printf(error, format, ap);
    return ret;
}

/* members and functions that manage the class it self */

static PsyGLErrorClass* g_cls = NULL;

int psy_glerror_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    SeeErrorClass* see_error_class = (SeeErrorClass*) new_cls;

    /*Override SeeObject*/
    // new_cls->init = init;

    /* Override parent functions.*/
    see_error_class->set_msg    = glerror_msg_set;

    // Set our own functions/values on the class.
    PsyErrorClass* err_cls  = (PsyErrorClass*) new_cls;
    err_cls->error_printf   = glerror_msg_printf;
    //err_cls->error_init     = error_init;

    return ret;
}

int psy_glerror_init()
{
    const SeeMetaClass* meta = see_meta_class_class();
    int ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_cls,
        sizeof(PsyGLErrorClass),
        sizeof(PsyGLError),
        (SeeObjectClass*) see_error_class(),
        sizeof(SeeErrorClass),
        psy_glerror_class_init
        );

    return ret;
}

void psy_glerror_deinit()
{
    if (!g_cls)
        return;

    see_object_decref((SeeObject*) g_cls);
    g_cls = NULL;
}

/* **** public api **** */

const PsyGLErrorClass*
psy_glerror_class()
{
    return g_cls;
}

int
psy_glerror_create(PsyGLError** out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(psy_glerror_class());
    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    int ret = cls->new_obj(
        cls,
        0,
        (SeeObject**) out,
        NULL
        );

    return ret;
}





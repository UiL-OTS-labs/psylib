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
 * @file Error.c
 * \brief Implements the SeeError objects.
 * \private
 */

#include "psy_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include <MetaClass.h>

#include "Error.h"


/* functions that implement the PsyErrorClass */

static void
error_init(PsyError* error, const PsyErrorClass* cls, const char* message)
{
    const SeeErrorClass* super_cls = (const SeeErrorClass*) cls;
    super_cls->error_init((SeeError*) error, super_cls, message);
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const char* msg = va_arg(args, const char*);
    const PsyErrorClass* psy_error_cls = (const PsyErrorClass*) cls;

    psy_error_cls->error_init((PsyError*) obj, psy_error_cls, msg);

    return SEE_SUCCESS;
}

static void
error_msg_set(SeeError* obj, const char* msg)
{
    PsyError* error = PSY_ERROR(obj);

    // Print to the buffer.
    snprintf(&error->msg_buffer[0], sizeof(error->msg_buffer), "%s", msg);

    see_error_class()->set_msg(SEE_ERROR(error), error->msg_buffer);
}

static int
error_msg_printf(PsyError* error, const char* format, va_list ap)
{
    int ret = vsnprintf(
        &error->msg_buffer[0],
        sizeof(error->msg_buffer),
        format,
        ap
        );

    see_error_class()->set_msg(SEE_ERROR(error), error->msg_buffer);

    return ret;
}

/* members and functions that manage the class it self */

static PsyErrorClass* g_cls = NULL;

int psy_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    SeeErrorClass* see_error_class = (SeeErrorClass*) new_cls;

    /*Override SeeObject*/
    new_cls->init = init;
    new_cls->name = "PsyError";

    /* Override parent functions.*/
    see_error_class->set_msg    = error_msg_set;

    // Set our own functions/values on the class.
    PsyErrorClass* err_cls  = (PsyErrorClass*) new_cls;
    err_cls->error_printf   = error_msg_printf;
    err_cls->error_init     = error_init;

    return ret;
}

int psy_error_init()
{
    const SeeMetaClass* meta = see_meta_class_class();
    int ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_cls,
        sizeof(PsyErrorClass),
        sizeof(PsyError),
        (SeeObjectClass*) see_error_class(),
        sizeof(SeeErrorClass),
        psy_error_class_init
        );

    return ret;
}

void psy_error_deinit()
{
    if (!g_cls)
        return;

    see_object_decref((SeeObject*) g_cls);
    g_cls = NULL;
}

/* **** public api **** */

const PsyErrorClass*
psy_error_class()
{
    return g_cls;
}

int
psy_error_create(PsyError** error)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(psy_error_class());

    int ret = cls->new_obj(
        cls,
        0,
        (SeeObject**) error,
        error
        );

    return ret;
}

int
psy_error_msg_set(PsyError* error, const char* msg)
{
    const PsyErrorClass* cls = PSY_ERROR_GET_CLASS(error);
    const SeeErrorClass* see_error_cls = SEE_ERROR_CLASS(cls);

    if (!error || !msg)
        return SEE_INVALID_ARGUMENT;

    if (!cls)
        return SEE_NOT_INITIALIZED;

    see_error_cls->set_msg((SeeError*) error, msg);
    return SEE_SUCCESS;
}

int
psy_error_printf(PsyError* error, const char* format, ...)
{
    int ret;
    va_list ap;

    if (!error || !format)
        return SEE_INVALID_ARGUMENT;

    const PsyErrorClass* cls = PSY_ERROR_GET_CLASS(error);

    assert(error && format);

    va_start(ap, format);
    ret = cls->error_printf(error, format, ap);
    va_end(ap);

    return ret;
}




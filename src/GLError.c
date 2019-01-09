/*
 * This file is part of see-object.
 *
 * see-object is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-object is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-object.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "MetaClass.h"
#include "GLError.h"

/* **** functions that implement PsyGLError or override SeePsyError **** */

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list* args)
{
    int ret, selector;
    const SeeObjectClass* super = cls->psuper;
    PsyGLErrorClass* own_class = (PsyGLErrorClass*) cls;
    
    // Generally you could set some default values here.
    // The init loop can still override them when necessary.
    
    ret = super->init(cls, obj, args);
    if (ret != SEE_SUCCESS)
        return ret;
    
    while ((selector = va_arg(*args, int)) != PSY_GLERROR_INIT_FINAL) {
        switch (selector) {
            // handle your cases here and remove this comment.
            default:
                return SEE_INVALID_ARGUMENT;        
        }
    }
    
    // Do some extra initialization here (on demand).
    
    return SEE_SUCCESS;
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

PsyGLErrorClass* g_SeeGLErrorClass = NULL;

static int see_glerror_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    PsyGLErrorClass* cls = (PsyGLErrorClass*) new_cls;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeGLError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_glerror_init() {
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeGLErrorClass,
        sizeof(PsyGLErrorClass),
        sizeof(PsyGLError),
        see_psy_error_class(),
        sizeof(SeePsyErrorClass),
        see_glerror_class_init
        );

    return ret;
}

void
see_glerror_deinit()
{
    if(!g_SeeGLErrorClass)
        return;

    see_object_decref((SeeObject*) g_SeeGLErrorClass);
    g_SeeGLErrorClass = NULL;
}


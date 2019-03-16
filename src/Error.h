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
 * @file Error.h
 * \public
 * \author Maarten Duijndam
 *
 * \brief This file exports the public interface for SeeError object.
 */

#ifndef Error_H
#define Error_H

#include <SeeObject.h>
#include <psy_export.h>
#include <stdarg.h>
#include <stdio.h>
#include <SeeObject-0.0/Error.h>

#ifdef __cplusplus
extern "C" {
#endif


struct _PsyError;
typedef struct _PsyError PsyError;
struct _PsyErrorClass;
typedef struct _PsyErrorClass PsyErrorClass;

struct _PsyError {
    SeeError    obj;
    char        msg_buffer[BUFSIZ];
};

typedef const char* (*psy_error_msg_func)(PsyError* error);
typedef void        (*psy_error_set_msg_func)(PsyError* error, const char* msg);

struct _PsyErrorClass {
    SeeErrorClass     see_class;
    void              (*error_init)(
        PsyError*               error,
        const PsyErrorClass*    cls,
        const char*             msg
        );
    int               (*error_printf)( PsyError* error,
                                       const char* format,
                                       va_list ap
                                       );
};

/**
 * Cast a pointer to an PsyError derived object to a PsyError*
 */
#define PSY_ERROR(obj)                                                      \
    ((PsyError*)(obj))


/**
 * Cast a pointer to class to a const PsyErrorClass*.
 */
#define PSY_ERROR_CLASS(cls)                                                \
    ((const PsyErrorClass*) (cls))

/**
 * Get the the class of a PsyError instance.
 */
#define PSY_ERROR_GET_CLASS(obj)                                            \
    PSY_ERROR_CLASS(see_object_get_class((SeeObject*)(obj)))


/* ********* public psy_error api *********** */

/** Create a error to return to the user.*/
PSY_EXPORT int
psy_error_create(PsyError** error);

/**
 * \brief Initialize the error message.
 * 
 * Notice that the size of the message is limited to the
 * sizeof struct _PsyError.msg field. If the message is longer it will be
 * truncated.
 */
PSY_EXPORT int
psy_error_msg_set(PsyError* self, const char* message);

/**
 * \brief Use a standard stdio printf format to initialize the message of 
 * the error. Internally snprintf is used to truncate the message if it is
 * longer than the buffer of the PsyError.msg is large.
 */
PSY_EXPORT int
psy_error_printf(PsyError* self, const char* format, ...);

/**
 * \brief Creates the class.
 * 
 * This function creates/prepares the class to be ready for use.
 * Has got to be called prior to use any of the psy_error_* functions
 */
PSY_EXPORT int
psy_error_init();

/**
 * \brief Unrefs the class and if the class is freed it's parents
 *
 * Make sure you do not have any remaining PsyError instances after
 * deinitializing the psy_error_class.
 */
PSY_EXPORT void
psy_error_deinit();

/**
 * \Get the PsyErrorClass instance.
 */
PSY_EXPORT const PsyErrorClass*
psy_error_class();


#ifdef __cplusplus
}
#endif

#endif //ifndef Error_H

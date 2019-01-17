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


#ifndef GLError_H
#define GLError_H


#include "../Error.h"

#ifdef __cplusplus
extern "C" {
#endif


struct _PsyGLError;
typedef struct _PsyGLError PsyGLError;
struct _PsyGLErrorClass;
typedef struct _PsyGLErrorClass PsyGLErrorClass;

struct _PsyGLError {
    PsyError    obj;
    // Doesn't extend PsyError
};


struct _PsyGLErrorClass {
    PsyErrorClass     psy_error_class;

    // Doesn't extend PsyErrorClass
};

/**
 * Cast a pointer to an PsyGLError derived object to a PsyGLError*
 */
#define PSY_GLERROR(obj)                                                      \
    ((PsyGLError*)(obj))


/**
 * Cast a pointer to class to a const PsyGLErrorClass*.
 */
#define PSY_GLERROR_CLASS(cls)                                                \
    ((const PsyGLErrorClass*) (cls));

/**
 * Get the the class of a PsyGLError instance.
 */
#define PSY_GLERROR_GET_CLASS(obj)                                            \
    PSY_ERROR_CLASS(see_object_get_class((SeeObject*)(obj)))


/* ********* public psy_glerror api *********** */

/**
 * \brief construct a new PsyGLError
 * @param [out]error
 *
 * @return SEE_SUCCESS
 */
PSY_EXPORT int psy_glerror_create(PsyGLError** error);

/**
 * \brief Creates the class.
 *
 * This function creates/prepares the class to be ready for use.
 * Has got to be called prior to use any of the psy_glerror_* functions
 */
PSY_EXPORT int psy_glerror_init();

/**
 * \brief Unrefs the class and if the class is freed it's parents
 *
 * Make sure you do not have any remaining PsyError instances after
 * deinitializing the psy_glerror_class.
 */
PSY_EXPORT void psy_glerror_deinit();

/**
 * \brief Get the PsyErrorClass instance.
 */
PSY_EXPORT const PsyGLErrorClass*   psy_glerror_class();


#ifdef __cplusplus
}
#endif

#endif //ifndef Error_H

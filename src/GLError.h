/*
 * This file is part of psylib.
 *
 * see-object is free software: you can redistribute it and/or modify
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


#ifndef SEE_GLERROR_H
#define SEE_GLERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PsyGLError PsyGLError;
typedef struct _PsyGLErrorClass PsyGLErrorClass;

struct _PsyGLError {
    PsyError parent_obj;
    /*expand PsyGLError data here*/
};

struct _PsyGLErrorClass {
    PsyErrorClass parent_cls;
    /* expand PsyGLError class with extra functions here.*/
};


/*
 * See Objects are initialized with a var_args list. Typically,
 * The init func sees one of the values below and then expects the value
 * that initializes the class.
 */
enum SeeGLErrorInitValues {

    /**
     * \brief SEE_GLERROR_INIT_FIRST document here what kind of member
     * the initializer function should expect.
     *
     * NB remove this comment and member to replace it with your own!!!
     */
    PSY_GLERROR_INIT_FIRST = ERROR_INIT_SENTINAL,
     

    /**
     * \brief SEE_GLERROR_INIT_FINALE init func expects no arguments
     * it exists to tell the initializer function to stop expecting arguments
     * for the current class.
     */
    PSY_GLERROR_INIT_FINAL,
    
    /**
     * \brief Mainly used as a starting point for deriving classes.
     */
    PSY_GLERROR_INIT_SENTINAL
}; 

/* **** public functions **** */

/**
 * Gets the pointer to the SeeGLErrorClass table.
 */
PSY_EXPORT const PsyGLErrorClass*
see_glerror_class();

/* Expand the class with public functions here, don't forget the PSY_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeGLError; make it ready for use.
 */
PSY_EXPORT
int see_glerror_init();

/**
 * Deinitialize SeeGLError, after SeeGLError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
PSY_EXPORT
void see_glerror_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_GLERROR_H

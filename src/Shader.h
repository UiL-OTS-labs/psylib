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


#ifndef PSY_SHADER_H
#define PSY_SHADER_H

#include <Error.h>
#include <stdio.h>
#include "SeeObject.h"
#include "psy_export.h"
#include "gl/includes_gl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PsyShader PsyShader;
typedef struct _PsyShaderClass PsyShaderClass;

typedef enum _psy_shader_t {
    PSY_SHADER_VERTEX,
    PSY_SHADER_FRAGMENT
    // Other shaders are currently not supported.
} psy_shader_t;

struct _PsyShader {
    SeeObject parent_obj;
    /*expand PsyShader data here*/
    psy_shader_t    shader_type;
    GLuint          shader_id;
};

struct _PsyShaderClass {
    SeeObjectClass parent_cls;
    
    int (*shader_init)(
        PsyShader*            shader,
        const PsyShaderClass* shader_cls,
        psy_shader_t          type
        );

    /* expand PsyShader class with extra functions here.*/

    int (*type) (const PsyShader* shader, psy_shader_t* out);

    int (*id)   (const PsyShader* shader, GLuint* out);

    int (*shader_compile)     ( PsyShader* shader,
                                const char* src,
                                SeeError** error
                                );

    int (*shader_compile_file)( PsyShader* shader,
                                FILE*      file,
                                SeeError** error
                                );
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a PsyShader derived instance back to a
 *        pointer to PsyShader.
 */
#define PSY_SHADER(obj)                      \
    ((PsyShader*) obj)

/**
 * \brief cast a pointer to PsyShaderClass derived class back to a
 *        pointer to PsyShaderClass.
 */
#define PSY_SHADER_CLASS(cls)                      \
    ((const PsyShaderClass*) cls)

/**
 * \brief obtain a pointer to PsyShaderClass from a instance of
 *        derived from PsyShader. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define PSY_SHADER_GET_CLASS(obj)                \
    (PSY_SHADER_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )


/* **** public functions **** */

/**
 * @brief Create a new vertex or fragment shader.
 *
 * Creates a new updated shader.
 *
 * @param [out]     shader  A pointer to a pointer to PsyShader
 * @param [in]      type    PSY_VERTEX_SHADER or PSY_FRAGMENT_SHADER
 * @param [out]     error   If an error occurs it might be returned at error.
 *                          if it does the error should be decreffed.
 * @return
 */
PSY_EXPORT int
psy_shader_create(PsyShader** shader, psy_shader_t type, SeeError** error);


/**
 * Gets the pointer to the PsyShaderClass table.
 */
PSY_EXPORT const PsyShaderClass*
psy_shader_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize PsyShader; make it ready for use.
 */
PSY_EXPORT
int psy_shader_init();

/**
 * Deinitialize PsyShader, after PsyShader has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
PSY_EXPORT
void psy_shader_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef PSY_SHADER_H

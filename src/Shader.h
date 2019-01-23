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
 * \file Shader.h A implements a OpenGL shader object.
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

/**
 * \brief The type of shaders that Psylib supports
 */
typedef enum _psy_shader_t {
    /**
     * \brief A vertex shader
     */
    PSY_SHADER_VERTEX,
    /**
     * \brief A fragment shader
     */
    PSY_SHADER_FRAGMENT

    // Other shaders are currently not supported.
} psy_shader_t;

/**
 * \brief This struct declares the data of a Shader object.
 *
 * \private
 */
struct _PsyShader {
    SeeObject parent_obj;
    /*expand PsyShader data here*/
    psy_shader_t    shader_type;
    GLuint          shader_id;

    /**
     * \private
     * if non-zero the shader is compiled
     */
    int             compiled;
};

/**
 * \brief The PsyShaderClass defines the operations that can be done in order
 * to do manipulation of a shader.
 *
 * \private
 */
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

    int (*shader_compiled)(const PsyShader* shader);
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
 * Creates a new shader.
 *
 * @param [out]     shader  A pointer to a pointer to PsyShader
 * @param [in]      type    PSY_VERTEX_SHADER or PSY_FRAGMENT_SHADER
 * @param [out]     error   If an error occurs it might be returned at error.
 *                          if it does the error should be decreffed.
 *
 * @return SEE_SUCCESS when the shader has been created.
 */
PSY_EXPORT int
psy_shader_create(PsyShader** shader, psy_shader_t type, SeeError** error);


/**
 * \brief Obtain the shader id from the shader
 *
 * A shader is created with a default id (0), indicating it hasn't been
 * initialized. Once the shader is successfully compiled it this value
 * should return an integer > 0.
 *
 * @param [in]  shader an initialized PsyShader*
 * @param [out] out The shader id that we use for openGL.
 * @return SEE_SUCCESS if the id is obtained correctly.
 */
PSY_EXPORT int
psy_shader_id(const PsyShader* shader, GLuint* out);


/**
 * \brief Obtain the shader type.
 * @param [in]  shader an initialized PsyShader*
 * @param [out] out    Here is the output returned which should
 *                     currently be PSY_SHADER_VERTEX or PSY_SHADER_FRAGMENT
 * @return SEE_SUCCESS if everything worked out.
 */
PSY_EXPORT int
psy_shader_type(const PsyShader* shader, psy_shader_t* out);


/**
 * \brief compile a shader from a string.
 *
 * This method compiles a shader. Before a shader can be created we must have
 * a valid OpenGL context. Currently we can obtain a valid OpenGL context if
 * we have a opened window. If the window opens the OpenGl Extension Wrangler
 * (GLEW) is initialized (Actually Glew requires a valid OpenGL )
 *
 * @param [in, out] shader  An initialized shader.
 * @param [in]      src     The shader source code to be compiled
 * @param [out]     error   If an error occurs it should be returned here.
 * @return
 */
PSY_EXPORT int
psy_shader_compile(PsyShader* shader, const char* src, SeeError** error);


/**
 * \brief compile a shader from a file
 *
 * This method compiles a shader from an opened file. This function reads
 * the file and uses compiles it. The name notes from psy_shader_compile
 * about the OpenGL context apply here as well. Make sure the file is opened
 * for reading before passing it to this function.
 *
 * @param [in,out]  shader An initialized PsyShader pointer
 * @param [in,out]  file   A File that is opened for reading.
 *                         ATM PsyLib doesn't understand platform file
 *                         encodings, hence it's best when it gets an open
 *                         file.
 * @param [out]     error  If an error occurs it can be returned here.
 * @return SEE_SUCCESS if the file compiled, some other SEE_ERROR if it did not
 *                     in this case error should hold a informative error.
 */
PSY_EXPORT int
psy_shader_compile_file(PsyShader* shader, FILE* file, SeeError** error);


/**
 * \brief Checks whether the shader is compiled.
 *
 * @param [in] shader
 *
 * @return 0 if the shader is compiled, another value indicates that the shader
 *           is compiled.
 */
PSY_EXPORT int
psy_shader_compiled(const PsyShader* shader);

/**
 * \brief Gets the pointer to the PsyShaderClass table.
 * \private
 */
PSY_EXPORT const PsyShaderClass*
psy_shader_class();

/* **** class initialization functions **** */

/**
 * \brief Initialize PsyShader; make it ready for use.
 */
PSY_EXPORT
int psy_shader_init();

/**
 * \brief Deinitialize PsyShader.
 *
 * After PsyShader has been deinitialized, the functions in this header
 * shouldn't be used anymore.
 */
PSY_EXPORT
void psy_shader_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef PSY_SHADER_H

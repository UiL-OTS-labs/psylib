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
 * \file ShaderProgram.h
 * \brief This file allows for the creation of OpenGL Shader programs
 */

#ifndef PSY_SHADER_PROGRAM_H
#define PSY_SHADER_PROGRAM_H

#include <SeeObject.h>
#include <SeeObject-0.0/Error.h>

#include "Shader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PsyShaderProgram PsyShaderProgram;
typedef struct _PsyShaderProgramClass PsyShaderProgramClass;

struct _PsyShaderProgram {
    SeeObject parent_obj;

    /*expand PsyShaderProgram data here*/

    GLuint          program_id;
    PsyShader*      vertex_shader;
    PsyShader*      fragment_shader;
    int             linked;
};

struct _PsyShaderProgramClass {

    SeeObjectClass parent_cls;
    
    int (*shader_program_init)(
        PsyShaderProgram*            shader_program,
        const PsyShaderProgramClass* shader_program_cls,
        PsyShader*                   vertex_shader,
        PsyShader*                   fragment_shader,
        SeeError**                   error
        );

    int (*add_shader)(
        PsyShaderProgram*   program,
        PsyShader*          shader,
        SeeError**          error
        );

    int (*add_vertex_shader)(
        PsyShaderProgram*   program,
        PsyShader*          shader,
        SeeError**          error
        );

    int (*add_fragment_shader)(
        PsyShaderProgram* program,
        PsyShader*          shader,
        SeeError**          error
        );

    int (*link)(
        PsyShaderProgram* program,
        SeeError** error
        );

    int (*linked) (
        const PsyShaderProgram* program
        );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a PsyShaderProgram derived instance back to a
 *        pointer to PsyShaderProgram.
 */
#define PSY_SHADER_PROGRAM(obj)                      \
    ((PsyShaderProgram*) obj)

/**
 * \brief cast a pointer to PsyShaderProgramClass derived class back to a
 *        pointer to PsyShaderProgramClass.
 */
#define PSY_SHADER_PROGRAM_CLASS(cls)                      \
    ((const PsyShaderProgramClass*) cls)

/**
 * \brief obtain a pointer to PsyShaderProgramClass from a instance of
 *        derived from PsyShaderProgram. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define PSY_SHADER_PROGRAM_GET_CLASS(obj)                \
    (PSY_SHADER_PROGRAM_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief construct a PsyShaderProgram
 *
 * @param [out] program The newly created program will be returned here.
 *                      program should not be NULL, whereas *program should
 * @param [in] vertex_shader    A vertex shader, may be NULL.
 * @param [in] fragment_shader  A fragment shader, may be NULL.
 * @param [out] error pointer to a SeeError* that point to NULL.
 * @return SEE_SUCCESS if successful.
 */
PSY_EXPORT int
psy_shader_program_create(
    PsyShaderProgram** program,
    PsyShader*         vertex_shader,
    PsyShader*         fragment_shader,
    SeeError**         error
    );

/**
 * \brief add a shader to the program.
 *
 * This method adds a shader to the program. The method finds which type of shader
 * is being used. If there already was a shader of that type in the program,
 * the shader will be the reference count will be decrement. The added shader
 * will notify that the program isn't linked.
 *
 * @param program
 * @param shader
 * @param error
 * @return SEE_SUCCESS if successful.
 */
PSY_EXPORT int
psy_shader_program_add_shader(
    PsyShaderProgram*   program,
    PsyShader*          shader,
    SeeError**          error
    );

/**
 * \brief Add a vertex shader to the program.
 *
 * Insert a vertex shader in the program. This function does a little bit
 * makes sure that the vertex is really a vertex shader, before using
 * psy_program_add_shader internally.
 *
 * @param [in, out] program The program to which you want to add a vertex shader.
 * @param [in]      shader  The <b>Vertex</b> shader.
 * @param [out]     error
 * @return
 */
PSY_EXPORT int
psy_shader_program_add_vertex_shader(
    PsyShaderProgram*   program,
    PsyShader*          vertex_shader,
    SeeError**          error
    );

/**
 * \brief Add a fragment shader to the program.
 *
 * Insert a fragment shader in the program. This function does a little bit
 * makes sure that the fragment is really a fragment shader, before using
 * psy_program_add_shader internally.
 *
 * @param [in,out]  program the program to which you want to add a fragment shader
 * @param [in]  shader  the shader this shader must be a fragment shader.
 * @param [out] error   If an error occurs, you might get some useful info here.
 * @return SEE_SUCCESS or sometimes SEE_RUNTIME_ERROR
 */
PSY_EXPORT int
psy_shader_program_add_fragment_shader(
    PsyShaderProgram*   program,
    PsyShader*          fragment_shader,
    SeeError**          error
    );

/**
 * \brief link the program.
 *
 * The program is linked. Make sure you have added at least a valid Fragment and
 * Vertex shader.
 *
 * @param [in] program
 * @param [out]error
 * @return
 */
PSY_EXPORT int
psy_shader_program_link(
    PsyShaderProgram*   program,
    SeeError**          error
    );

/**
 * \brief check whether the program is successfully linked.
 *
 * @param program a previously create PsyShaderProgram.
 * @return a non-null value when the program was successfully linked.
 */
PSY_EXPORT int
psy_shader_linked(const PsyShaderProgram* program);

/**
 * \brief obtain the vertex shader.
 * @param [in] program
 * @return NULL if no vertex shader has been specified, a non NULL value otherwise.
 */
PSY_EXPORT PsyShader*
psy_shader_get_vertex_shader(const PsyShaderProgram* program);

/**
 * \brief obtain the fragment shader.
 * @param [in] program
 * @return NULL if no fragment shader has been specified, a non NULL value otherwise.
 */
PSY_EXPORT PsyShader*
psy_shader_get_frament_shader(const PsyShaderProgram* program);



/**
 * Gets the pointer to the PsyShaderProgramClass table.
 */
PSY_EXPORT const PsyShaderProgramClass*
psy_shader_program_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize PsyShaderProgram; make it ready for use.
 */
PSY_EXPORT
int psy_shader_program_init();

/**
 * Deinitialize PsyShaderProgram, after PsyShaderProgram has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
PSY_EXPORT
void psy_shader_program_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef PSY_SHADER_PROGRAM_H

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


#include <assert.h>
#include <DynamicArray.h>
#include "MetaClass.h"
#include "Shader.h"
#include "Error.h"
#include "gl/GLError.h"

/* **** functions that implement PsyShader or override SeeObject **** */

static int
shader_init(
    PsyShader*              shader,
    const PsyShaderClass*   shader_cls,
    psy_shader_t            type
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_CLASS(
        shader_cls
        );
        
    parent_cls->object_init(SEE_OBJECT(shader), parent_cls);

    shader->shader_id   = 0;
    shader->shader_type = type;
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const PsyShaderClass* shader_cls = PSY_SHADER_CLASS(cls);
    PsyShader* shader = PSY_SHADER(obj);
    
    /*Extract parameters here from va_list args here.*/

    psy_shader_t type = va_arg(args, psy_shader_t);

    return shader_cls->shader_init(
        shader,
        shader_cls,
        type
        );
}

static int
shader_id(const PsyShader* shader, GLuint* id)
{
    const PsyShaderClass* cls = PSY_SHADER_GET_CLASS(shader);

    if (!shader)
        return SEE_INVALID_ARGUMENT;

    return cls->id(shader, id);
}

static int
shader_type(const PsyShader* shader, psy_shader_t* type)
{
    const PsyShaderClass* cls = PSY_SHADER_GET_CLASS(shader);

    if (!shader)
        return SEE_INVALID_ARGUMENT;

    return cls->type(shader, type);
}

static int
shader_compile(PsyShader* shader, const char* src, SeeError** error)
{
    int success;
    SeeError* err = NULL;
    char log[BUFSIZ] = {0};

    glShaderSource(shader->shader_id, 1, &src, NULL);
    glCompileShader(shader->shader_id);

    /* Check whether compilation succeeded. */
    glGetShaderiv(shader->shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader->shader_id, sizeof(log), NULL, log);
        int status = psy_glerror_create((PsyGLError**)(&err));
        assert(status == SEE_SUCCESS);
        psy_error_printf(PSY_ERROR(err), "Unable to compile shader:\n%s", log);
        *error = err;
        return SEE_RUNTIME_ERROR;
    }

    return SEE_SUCCESS;
}

static int
shader_compile_file(PsyShader* shader, FILE* file, SeeError** error)
{
    int c, ret;
    char character;
    char null_byte = '0';
    const char* string = NULL;
    const PsyShaderClass* cls = PSY_SHADER_GET_CLASS(shader);

    SeeDynamicArray* array = NULL;

    ret = see_dynamic_array_new_capacity(
        &array,
        sizeof(char),
        NULL,
        NULL,
        NULL,
        1024
        );

    if (ret != SEE_SUCCESS)
        return ret;

    while ((c = fgetc(file)) != EOF) {
        character = (char) c;
        ret = see_dynamic_array_add(array, &character);
        if (ret != SEE_SUCCESS) {
            goto shader_compile_file_error;
        }
    }
    ret = see_dynamic_array_add(array, &null_byte);
    if (ret)
        goto shader_compile_file_error;

    if (ferror(file)) {
        ret = SEE_RUNTIME_ERROR;
        goto shader_compile_file_error;
    }

    string = see_dynamic_array_get(array, 0);
    ret = cls->shader_compile(shader, string, error);

    //cleanup and exit
    shader_compile_file_error:
    see_object_decref(SEE_OBJECT(array));
    return ret;
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

PsyShaderClass* g_PsyShaderClass = NULL;

static int psy_shader_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;

    /* Set the function pointers of the own class here */
    PsyShaderClass* cls = (PsyShaderClass*) new_cls;

    cls->shader_init         = shader_init;
    cls->type                = shader_type;
    cls->id                  = shader_id;
    cls->shader_compile      = shader_compile;
    cls->shader_compile_file = shader_compile_file;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes PsyShader(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
psy_shader_init() {
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_PsyShaderClass,
        sizeof(PsyShaderClass),
        sizeof(PsyShader),
        see_object_class(),
        sizeof(SeeObjectClass),
        psy_shader_class_init
        );

    return ret;
}

void
psy_shader_deinit()
{
    if(!g_PsyShaderClass)
        return;

    see_object_decref((SeeObject*) g_PsyShaderClass);
    g_PsyShaderClass = NULL;
}


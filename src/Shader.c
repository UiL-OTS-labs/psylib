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
 * file Shader.c
 * \brief implements OpenGL shaders
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

static void
shader_destroy(SeeObject* shader)
{
    assert(shader);
    if (PSY_SHADER(shader)->shader_id)
        glDeleteShader(PSY_SHADER(shader)->shader_id);
    see_object_class()->destroy(shader);
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
    if (!shader || !id)
        return SEE_INVALID_ARGUMENT;

    *id = shader->shader_id;

    return SEE_SUCCESS;
}

static int
shader_type(const PsyShader* shader, psy_shader_t* type)
{
    if (!shader || !type)
        return SEE_INVALID_ARGUMENT;

    *type = shader->shader_type;
    return SEE_SUCCESS;
}

static int
shader_compile(PsyShader* shader, const char* src, SeeError** error)
{
    int success;
    SeeError* err = NULL;
    char log[BUFSIZ];

    if (shader->shader_id)
        glDeleteShader(shader->shader_id);
    GLenum shader_type = 0;
    switch (shader->shader_type) {
        case PSY_SHADER_VERTEX:
            shader_type = GL_VERTEX_SHADER;
            break;
        case PSY_SHADER_FRAGMENT:
            shader_type = GL_FRAGMENT_SHADER;
            break;
        default:
            assert(0 == 1);
            return SEE_INTERNAL_ERROR;
    }
    shader->shader_id = glCreateShader(shader_type);
    shader->compiled = 0;

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

    shader->compiled = 1;

    return SEE_SUCCESS;
}

static int
shader_compile_file(PsyShader* shader, FILE* file, SeeError** error)
{
    int c, ret;
    char character;
    char null_byte = '\0';
    const char* string = NULL;
    const PsyShaderClass* cls = PSY_SHADER_GET_CLASS(shader);

    SeeDynamicArray* array = NULL;

    ret = see_dynamic_array_new_capacity(
        &array,
        sizeof(char),
        NULL,
        NULL,
        NULL,
        BUFSIZ
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
        perror("A error on the file was encountered");
        goto shader_compile_file_error;
    }

    string = see_dynamic_array_get(array, 0);
    ret = cls->shader_compile(shader, string, error);

    //cleanup and exit
    shader_compile_file_error:
    see_object_decref(SEE_OBJECT(array));
    return ret;
}

static int
shader_compiled(const PsyShader* shader)
{
    assert(shader != NULL);
    return shader->compiled;
}


/* **** implementation of the public API **** */

int psy_shader_create(PsyShader** shader, psy_shader_t type, SeeError** error)
{
    const PsyShaderClass* cls = psy_shader_class();
    const SeeObjectClass* see_cls = SEE_OBJECT_CLASS(cls);

    if (!shader || *shader)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return see_cls->new_obj(see_cls, 0, (SeeObject**) shader, type);
}

int psy_shader_id(const PsyShader* shader, GLuint* id)
{
    const PsyShaderClass* cls;
    if (!shader || ! id)
        return SEE_INVALID_ARGUMENT;

    cls = PSY_SHADER_GET_CLASS(shader);
    return cls->id(shader, id);
}

int psy_shader_type(const PsyShader* shader, psy_shader_t* type)
{
    const PsyShaderClass* cls;
    if (!shader || ! type)
        return SEE_INVALID_ARGUMENT;

    cls = PSY_SHADER_GET_CLASS(shader);
    return cls->type(shader, type);
}

int psy_shader_compile(PsyShader* shader, const char* src, SeeError** error)
{
    const PsyShaderClass* cls;
    if (!shader || ! src)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    cls = PSY_SHADER_GET_CLASS(shader);
    return cls->shader_compile(shader, src, error);
}

int psy_shader_compile_file(PsyShader* shader, FILE* file, SeeError** error)
{
    const PsyShaderClass* cls;
    if (!shader || ! file)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    cls = PSY_SHADER_GET_CLASS(shader);
    return cls->shader_compile_file(shader, file, error);
}

int psy_shader_compiled(const PsyShader* shader)
{
    const PsyShaderClass* cls;
    assert(shader != 0);
    if (!shader)
        return 0;

    cls = PSY_SHADER_GET_CLASS(shader);
    return cls->shader_compiled(shader);
}
/* **** initialization of the class **** */

PsyShaderClass* g_PsyShaderClass = NULL;

const PsyShaderClass* psy_shader_class()
{
    return g_PsyShaderClass;
}

static int psy_shader_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    // We might need to free the shader on object destruction.
    new_cls->destroy = shader_destroy;

    /* Set the function pointers of the own class here */
    PsyShaderClass* cls = (PsyShaderClass*) new_cls;

    cls->shader_init         = shader_init;
    cls->type                = shader_type;
    cls->id                  = shader_id;
    cls->shader_compile      = shader_compile;
    cls->shader_compile_file = shader_compile_file;
    cls->shader_compiled     = shader_compiled;
    
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


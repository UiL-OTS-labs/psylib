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
#include "MetaClass.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "gl/GLError.h"


static void
invalidate_program(PsyShaderProgram* program)
{
    if (!program)
        return;

    if(program->program_id) {
        glDeleteProgram(program->program_id);
        program->program_id = 0;
    }
    program->linked = 0;
}

static void
invalidate_vertex_shader(PsyShaderProgram* program)
{
    if (!program)
        return;

    if (program->vertex_shader) {
        see_object_decref(SEE_OBJECT(program->vertex_shader));
        program->vertex_shader = NULL;
    }
    invalidate_program(program);
}

static void invalidate_fragment_shader(PsyShaderProgram* program)
{
    if (!program)
        return;

    if (program->fragment_shader) {
        see_object_decref(SEE_OBJECT(program->fragment_shader));
        program->fragment_shader = NULL;
    }
    invalidate_program(program);
}

/* **** functions that implement PsyShaderProgram or override SeeObject **** */

static int
shader_program_init(
    PsyShaderProgram* shader_program,
    const PsyShaderProgramClass* shader_program_cls,
    PsyShader* vertex_shader,
    PsyShader* fragment_shader,
    SeeError** error
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_CLASS(
        shader_program_cls
        );
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->object_init(
        SEE_OBJECT(shader_program),
        SEE_OBJECT_CLASS(shader_program_cls)
        );

    // Store a reference to the shaders
    if (vertex_shader) {
        ret = shader_program_cls->add_vertex_shader(
            shader_program,
            vertex_shader,
            error
            );
        if (ret)
            return ret;
    }
    if (fragment_shader) {
        ret = shader_program_cls->add_fragment_shader(
            shader_program,
            fragment_shader,
            error
            );
        if (ret)
            return ret;
    }

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const PsyShaderProgramClass* shader_program_cls = PSY_SHADER_PROGRAM_CLASS(cls);
    PsyShaderProgram* shader_program = PSY_SHADER_PROGRAM(obj);
    
    /*Extract parameters here from va_list args here.*/
    PsyShader* vertex_shader    = va_arg(args, PsyShader*);
    PsyShader* fragment_shader  = va_arg(args, PsyShader*);
    SeeError** error            = va_arg(args, SeeError**);

    return shader_program_cls->shader_program_init(
        shader_program,
        shader_program_cls,
        vertex_shader,
        fragment_shader,
        error
        );
}

static void
destroy(SeeObject* object)
{
    PsyShaderProgram* program = PSY_SHADER_PROGRAM(object);
    invalidate_vertex_shader(program);
    invalidate_fragment_shader(program);
    invalidate_program(program);

    see_object_class()->destroy(object);
}

static int
add_shader(PsyShaderProgram* program, PsyShader* shader, SeeError** error)
{
    (void) error;
    psy_shader_t shader_type;
    shader_type =  shader->shader_type;
    if (shader_type == PSY_SHADER_VERTEX) {
        if (program->vertex_shader)
            see_object_decref(SEE_OBJECT(program->vertex_shader));
        program->vertex_shader = shader;
    }
    else if (shader_type == PSY_SHADER_FRAGMENT) {
        if (program->fragment_shader)
            see_object_decref(SEE_OBJECT(program->fragment_shader));
        program->fragment_shader = shader;
    }
    else {
        return SEE_INVALID_ARGUMENT;
    }

    program->linked = 0;
    see_object_ref(SEE_OBJECT(shader));

    return SEE_SUCCESS;
}

static int
add_vertex_shader(
    PsyShaderProgram*   program,
    PsyShader*          vertex_shader,
    SeeError**          error
    )
{
    // TODO handle the case we already have a vertex shader and or program.
    // eg free the old shader and unlink the program.
    if (program->vertex_shader)
        invalidate_vertex_shader(program);

    psy_shader_t type;
    const PsyShaderClass* shader_cls = PSY_SHADER_GET_CLASS(vertex_shader);
    int ret = shader_cls->type(vertex_shader, &type);
    assert(ret == SEE_SUCCESS);

    if (type != PSY_SHADER_VERTEX) {
        PsyGLError* err = NULL;
        psy_glerror_create(&err);
        psy_error_printf(PSY_ERROR(err),
            "add_vertex_shader: the shader is not a vertex shader."
            );
        *error = SEE_ERROR(err);
        return SEE_INVALID_ARGUMENT;
    }

    return PSY_SHADER_PROGRAM_GET_CLASS(program)->add_shader(
        program,
        vertex_shader,
        error
        );
}

static int
add_fragment_shader(
    PsyShaderProgram*   program,
    PsyShader*          fragment_shader,
    SeeError**          error
    )
{
    // TODO handle the case we already have a fragment shader and or program.
    // eg free the old shader and unlink the program.
    if (program->fragment_shader)
        invalidate_fragment_shader(program);

    psy_shader_t type;
    const PsyShaderClass* shader_cls = PSY_SHADER_GET_CLASS(fragment_shader);
    int ret = shader_cls->type(fragment_shader, &type);
    assert(ret == SEE_SUCCESS);

    if (type != PSY_SHADER_FRAGMENT) {
        PsyGLError* err = NULL;
        psy_glerror_create(&err);
        psy_error_printf(PSY_ERROR(err),
                 "add_fragment_shader: the shader is not a fragment shader."
                 );
        *error = SEE_ERROR(err);
        return SEE_INVALID_ARGUMENT;
    }

    return PSY_SHADER_PROGRAM_GET_CLASS(program)->add_shader(
        program,
        fragment_shader,
        error
        );
}

static int
add_vertex_src(
    PsyShaderProgram*   program,
    const char*         src,
    SeeError**          error
    )
{
    int ret;
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);
    PsyShader* vertshader = NULL;

    ret = psy_shader_create(&vertshader, PSY_SHADER_VERTEX, error);
    if (ret != SEE_SUCCESS)
        return ret;

    ret = psy_shader_compile(vertshader, src, error);
    if (ret != SEE_SUCCESS)
        goto add_vertex_src_error;

    ret = cls->add_vertex_shader(program, vertshader, error);

add_vertex_src_error:
    see_object_decref(SEE_OBJECT(vertshader));
    return ret;
}

static int
add_fragment_src (
    PsyShaderProgram*   program,
    const char*         src,
    SeeError**          error
)
{
    int ret;
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);
    PsyShader* fragshader = NULL;

    ret = psy_shader_create(&fragshader, PSY_SHADER_FRAGMENT, error);
    if (ret != SEE_SUCCESS)
        return ret;

    ret = psy_shader_compile(fragshader, src, error);
    if (ret != SEE_SUCCESS)
        goto add_frag_src_error;

    ret = cls->add_fragment_shader(program, fragshader, error);

add_frag_src_error:
    see_object_decref(SEE_OBJECT(fragshader));
    return ret;
}


static const PsyShader*
shader_program_get_vertex_shader(const PsyShaderProgram* program)
{
    if (!program)
        return NULL;
    return program->vertex_shader;
}

static const PsyShader*
shader_program_get_fragment_shader(const PsyShaderProgram* program)
{
    if (!program)
        return NULL;
    return program->fragment_shader;
}

static int
shader_program_link(PsyShaderProgram* program, SeeError** error)
{
    int success;
    PsyGLError* glerror = NULL;
    char log[BUFSIZ];

    if (program->program_id)
        invalidate_program(program);

    program->program_id = glCreateProgram();

    PsyShader *shader = program->vertex_shader;
    if (shader) {
        if (!psy_shader_compiled(shader)) {
            psy_glerror_create(&glerror);
            psy_error_printf(
                PSY_ERROR(glerror),
                "%s: Vertex shader isn't compiled",
                __func__
                );
            *error = SEE_ERROR(glerror);
            return SEE_ERROR_RUNTIME;
        }
        glAttachShader(program->program_id, shader->shader_id);
    }
    else {
        psy_glerror_create(&glerror);
        psy_error_printf(
            PSY_ERROR(glerror),
            "No vertex shader specified"
            );
        *error = SEE_ERROR(glerror);
        return SEE_ERROR_RUNTIME;
    }

    shader = program->fragment_shader;
    if (shader){
        if (!psy_shader_compiled(shader)) {
            psy_glerror_create(&glerror);
            psy_error_printf(
                PSY_ERROR(glerror),
                "%s: Fragment shader isn't compiled",
                __func__
            );
            *error = SEE_ERROR(glerror);
            return SEE_ERROR_RUNTIME;
        }
        glAttachShader(program->program_id, shader->shader_id);
    }
    else {
        psy_glerror_create(&glerror);
        psy_error_printf(
            PSY_ERROR(glerror),
            "No fragment shader specified"
            );
        *error = SEE_ERROR(glerror);
        return SEE_ERROR_RUNTIME;
    }

    glLinkProgram(program->program_id);
    glGetProgramiv(program->program_id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(program->program_id, sizeof(log), NULL, log);
        int status = psy_glerror_create(&glerror);
        assert(status == SEE_SUCCESS);
        psy_error_printf(PSY_ERROR(glerror),"Unable to link program:\n%s", log);
        *error = SEE_ERROR(glerror);
        return SEE_ERROR_RUNTIME;
    }

    /* Free resources as they are contained in the program. */
    if (program->vertex_shader) {
        see_object_decref(SEE_OBJECT(program->vertex_shader));
        program->vertex_shader = NULL;
    }
    if (program->fragment_shader) {
        see_object_decref(SEE_OBJECT(program->fragment_shader));
        program->fragment_shader = NULL;
    }

    program->linked = 1;

    return SEE_SUCCESS;
}

static int
shader_program_linked(const PsyShaderProgram* program)
{
    return program->linked;
}

/* **** implementation of the public API **** */

int
psy_shader_program_create(
    PsyShaderProgram**  out,
    PsyShader*          vertex,
    PsyShader*          fragment,
    SeeError**          error
    )
{
    const PsyShaderProgramClass* cls = psy_shader_program_class();
    const SeeObjectClass* see_cls = SEE_OBJECT_CLASS(cls);

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return see_cls->new_obj(see_cls, 0, (SeeObject**) out, vertex, fragment, error);
}

int psy_shader_program_add_shader(
    PsyShaderProgram*   program,
    PsyShader*          shader,
    SeeError**          error
    )
{
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);

    if (!program)
        return SEE_INVALID_ARGUMENT;

    if (!shader)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return cls->add_shader(program, shader, error);
}

int psy_shader_program_add_vertex_shader(
    PsyShaderProgram*   program,
    PsyShader*          vertex,
    SeeError**          error
    )
{
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);

    if (!program)
        return SEE_INVALID_ARGUMENT;

    if (!vertex)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return cls->add_vertex_shader(program, vertex, error);
}

int psy_shader_program_add_fragment_shader(
    PsyShaderProgram*   program,
    PsyShader*          fragment,
    SeeError**          error
    )
{
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);

    if (!program)
        return SEE_INVALID_ARGUMENT;

    if (!fragment)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return cls->add_fragment_shader(program, fragment, error);
}

int psy_shader_program_add_vertex_src(
    PsyShaderProgram*   program,
    const char*         vertex,
    SeeError**          error
)
{
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);

    if (!program)
        return SEE_INVALID_ARGUMENT;

    if (!vertex)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return cls->add_vertex_src(program, vertex, error);
}

int psy_shader_program_add_fragment_src(
    PsyShaderProgram*   program,
    const char*         fragment,
    SeeError**          error
)
{
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);

    if (!program)
        return SEE_INVALID_ARGUMENT;

    if (!fragment)
        return SEE_INVALID_ARGUMENT;

    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return cls->add_fragment_src(program, fragment, error);
}



int
psy_shader_program_link(PsyShaderProgram* program, SeeError** error)
{
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);
    if (!program)
        return SEE_INVALID_ARGUMENT;
    if (error && *error)
        return SEE_INVALID_ARGUMENT;

    return cls->link(program, error);
}

int
psy_shader_program_linked(const PsyShaderProgram* program)
{
    const PsyShaderProgramClass* cls = PSY_SHADER_PROGRAM_GET_CLASS(program);
    if(!program)
        return 0;

    assert(cls->linked);

    return cls->linked(program);
}

const PsyShader*
psy_shader_program_get_vertex_shader(const PsyShaderProgram* program)
{
    const PsyShaderProgramClass *cls;
    assert(program);
    if (!program)
        return NULL;

    cls = PSY_SHADER_PROGRAM_GET_CLASS(program);

    return cls->get_vertex_shader(program);
}

const PsyShader*
psy_shader_program_get_fragment_shader(const PsyShaderProgram* program)
{
    const PsyShaderProgramClass *cls;
    assert(program);
    if (!program)
        return NULL;

    cls = PSY_SHADER_PROGRAM_GET_CLASS(program);

    return cls->get_fragment_shader(program);
}


/* **** initialization of the class **** */

PsyShaderProgramClass* g_PsyShaderProgramClass = NULL;

static int psy_shader_program_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    new_cls->destroy = destroy;
    
    /* Set the function pointers of the own class here */
    PsyShaderProgramClass* cls = (PsyShaderProgramClass*) new_cls;

    cls->shader_program_init    = shader_program_init;
    cls->add_shader             = add_shader;
    cls->add_vertex_shader      = add_vertex_shader;
    cls->add_fragment_shader    = add_fragment_shader;
    cls->add_vertex_src         = add_vertex_src;
    cls->add_fragment_src       = add_fragment_src;
    cls->link                   = shader_program_link;
    cls->linked                 = shader_program_linked;
    cls->get_fragment_shader    = shader_program_get_fragment_shader;
    cls->get_vertex_shader      = shader_program_get_vertex_shader;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes PsyShaderProgram(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
psy_shader_program_init() {
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_PsyShaderProgramClass,
        sizeof(PsyShaderProgramClass),
        sizeof(PsyShaderProgram),
        see_object_class(),
        sizeof(SeeObjectClass),
        psy_shader_program_class_init
        );

    return ret;
}

void
psy_shader_program_deinit()
{
    if(!g_PsyShaderProgramClass)
        return;

    see_object_decref((SeeObject*) g_PsyShaderProgramClass);
    g_PsyShaderProgramClass = NULL;
}

const PsyShaderProgramClass* psy_shader_program_class()
{
    return g_PsyShaderProgramClass;
}

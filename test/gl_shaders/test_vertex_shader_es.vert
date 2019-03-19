#version 100

precision mediump float;

attribute vec3 a_vertex;

void main()
{
    vec3 aPos = a_vertex;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}


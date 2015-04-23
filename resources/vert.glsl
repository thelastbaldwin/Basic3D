#version 120

varying vec2 vUv;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vUv = gl_TexCoord[0].xy;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
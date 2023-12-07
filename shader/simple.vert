//  Phong lighting
#version 120

void main() {
    //  Use color unchanged
    gl_FrontColor = gl_Color;

    //  Texture
    gl_TexCoord[0] = gl_MultiTexCoord0;

    //  Return fixed transform coordinates for this vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

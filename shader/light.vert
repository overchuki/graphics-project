//  Per Pixel Lighting shader with texture
#version 120

uniform float OuterFlashlightCone;
uniform float FlashLightOn;

varying vec3 SunLight;
varying vec3 FlashLightDirection;
varying vec3 FlashLightVertexDirection;
varying vec3 Normal;
varying float FlashLightDistance;
varying float FlashLightOuterCutoff;
varying float FlashLIghtOnFrag;
vec3 origin = vec3(0.0, 0.0, 0.0);

void main() {
    //  Vertex location in modelview coordinates
    vec4 P = gl_ModelViewMatrix * gl_Vertex;

    //  Sun Light position
    SunLight  = gl_LightSource[0].position.xyz;

    // Flash Light variables
    FlashLightDirection = gl_LightSource[1].spotDirection.xyz;
    FlashLightVertexDirection = -P.xyz;
    FlashLightDistance = distance(origin, P.xyz);
    FlashLightOuterCutoff = OuterFlashlightCone;
    FlashLIghtOnFrag = FlashLightOn;
    
    //  Normal
    Normal = gl_NormalMatrix * gl_Normal;
    
    //  Texture
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    //  Set vertex position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

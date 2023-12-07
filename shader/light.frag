//  Per Pixel Lighting shader with texture
#version 120

varying vec3 SunLight;
varying vec3 FlashLightDirection;
varying vec3 FlashLightVertexDirection;
varying vec3 Normal;
varying float FlashLightDistance;
varying float FlashLightOuterCutoff;
varying float FlashLIghtOnFrag;
uniform sampler2D tex;

void main() {
    // SUN
    vec3 NormNormal = normalize(Normal);     //  N is the object normal
    vec3 L = normalize(SunLight);   //  L is the light vector
    vec3 R = reflect(-L,NormNormal);         //  R is the reflected light vector R = 2(L.N)N - L
    float Id = max(dot(L,NormNormal), 0.0);  //  Diffuse light is cosine of light and normal vectors
    //  Specular is cosine of reflected and view vectors
    // float Is = (Id>0.0) ? pow(max(dot(R,V),0.0) , gl_FrontMaterial.shininess) : 0.0;

    //  Sum color types
    // vec4 color = gl_FrontMaterial.emission
    //             + gl_FrontLightProduct[0].ambient
    //             + Id*gl_FrontLightProduct[0].diffuse;
    //             // + Is*gl_FrontLightProduct[0].specular;
    vec4 color = gl_FrontMaterial.emission
                + gl_LightSource[0].ambient
                + Id*gl_LightSource[0].diffuse;
                // + Is*gl_FrontLightProduct[0].specular;

    // float theta = dot(normalize(FlashLightVertexDirection), normalize(-FlashLightDirection));
    float theta = dot(normalize(FlashLightVertexDirection), normalize(-FlashLightDirection));
    if (FlashLIghtOnFrag > 0.0 && theta > FlashLightOuterCutoff) {
        // vec4 flashLightColor = max(dot(-FlashLightDirection,N)*2, 0.0) * gl_LightSource[1].diffuse;
        float attenuation = gl_LightSource[1].constantAttenuation + gl_LightSource[1].linearAttenuation * FlashLightDistance + gl_LightSource[1].quadraticAttenuation * (FlashLightDistance * FlashLightDistance);
        float epsilon   = gl_LightSource[1].spotCutoff - FlashLightOuterCutoff;
        float intensity = clamp((theta - FlashLightOuterCutoff) / epsilon, 0.0, 1.0); 
        color += (gl_LightSource[1].diffuse / attenuation) * intensity;
    }

    //  Apply texture
    gl_FragColor = color * texture2D(tex, gl_TexCoord[0].xy);
}

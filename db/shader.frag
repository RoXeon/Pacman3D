// Light
varying vec4 diffuse,ambientGlobal, ambient, ecPos;
varying vec3 normal,halfVector;

// Textures
uniform sampler2D samplerName;

// Fog
varying float atten;
varying float fogFactor;
varying vec3 lightVec, viewVec;

void main()
{
    // Light
    vec3 n,halfV,lightDir;
    float NdotL,NdotHV;
    vec4 color = ambientGlobal;
    float att,spotEffect;

    vec3 ct,cf;
    float intensity,at,af;

    /* a fragment shader can't write a verying variable, hence we need
    a new variable to store the normalized interpolated normal */
    n = normalize(normal);

    // Compute the ligt direction
    lightDir = vec3(gl_LightSource[0].position-ecPos);

    /* compute the distance to the light source to a varying variable*/
    float dist = length(lightDir);

    /* compute the dot product between normal and ldir */
    NdotL = max(dot(n,normalize(lightDir)),0.0);

    if (NdotL > 0.0) {

        spotEffect = dot(normalize(gl_LightSource[0].spotDirection), normalize(-lightDir));
        if (spotEffect > gl_LightSource[0].spotCosCutoff) {
            spotEffect = pow(spotEffect, gl_LightSource[0].spotExponent);
            att = spotEffect / (gl_LightSource[0].constantAttenuation +
                    gl_LightSource[0].linearAttenuation * dist +
                    gl_LightSource[0].quadraticAttenuation * dist * dist);

            color += att * (diffuse * NdotL + ambient);


            halfV = normalize(halfVector);
            NdotHV = max(dot(n,halfV),0.0);
            color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV, 0.5);
        }
    }

    // Final color
    vec3 lVec = normalize(lightVec);
    vec3 vVec = normalize(viewVec);
    vec4 finalColor = color * texture2D(samplerName, gl_TexCoord[1].st) * atten;

    gl_FragColor = mix(gl_Fog.color, finalColor, fogFactor );
}

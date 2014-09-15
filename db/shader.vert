// Light
varying vec4 diffuse,ambientGlobal,ambient, ecPos;
varying vec3 normal,halfVector;

// Textures
uniform sampler2D samplerName;

// Fog
varying float atten;
varying float fogFactor;
varying vec3 lightVec, viewVec;
attribute vec3 tangent;

void main()
{
    // Setup... something
    gl_TexCoord[1] = gl_MultiTexCoord1;
    gl_Position = ftransform();


    // Light
    /* first transform the normal into eye space and normalize the result */
    normal = normalize(gl_NormalMatrix * gl_Normal);

    /* compute the vertex position  in camera space. */
    ecPos = gl_ModelViewMatrix * gl_Vertex;

    /* Normalize the halfVector to pass it to the fragment shader */
    halfVector = gl_LightSource[0].halfVector.xyz;

    /* Compute the diffuse, ambient and globalAmbient terms */
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;


    // Fog
    vec3 n = normalize(gl_NormalMatrix * gl_Normal);
    vec3 t = normalize(gl_NormalMatrix * tangent);
    vec3 b = cross(n, t);

    vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
    vec3 vLVec = gl_LightSource[0].position.xyz - vVertex;

    atten = 1.0 / (1.0 + 0.00001 * dot(vLVec, vLVec));

    vec3 vVec = -vVertex;
    viewVec.x = dot(vVec, t);
    viewVec.y = dot(vVec, b);
    viewVec.z = dot(vVec, n);

    lightVec.x = dot(vLVec, t);
    lightVec.y = dot(vLVec, b);
    lightVec.z = dot(vLVec, n);

    const float LOG2 = 1.442695;
    gl_FogFragCoord = length(vVertex);
    fogFactor = exp2( -gl_Fog.density *
                       gl_Fog.density *
                       gl_FogFragCoord *
                       gl_FogFragCoord *
                       LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0);
}

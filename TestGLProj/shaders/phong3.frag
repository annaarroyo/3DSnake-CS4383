#version 440 core
in vec3 N;
in vec3 L;
in vec3 E;
in vec3 H;
in vec3 spotN;
in vec3 spotL;
in vec3 spotE;
in vec3 spotH;
in vec4 eyePosition;

uniform vec4 lightPosition;
uniform mat4 Projection;
uniform mat4 ModelView;

uniform vec4 lightDiffuse;
uniform vec4 lightSpecular; 
uniform vec4 lightAmbient;
uniform vec4 surfaceDiffuse;
uniform vec4 surfaceSpecular;
uniform float shininess;
uniform vec4 surfaceAmbient;
uniform vec4  surfaceEmissive;
uniform vec3 spotDir;
uniform vec4 spotPos;
uniform float cutoff;

void main()
{
	vec3 Normal = normalize(N);
    vec3 Light  = normalize(lightPosition - eyePosition).xyz;
    vec3 Eye    = normalize(E);
    vec3 Half   = normalize(H);

    vec3 SpotNormal = normalize(spotN);
    vec3 SpotLight  = normalize(spotL);
    vec3 SpotEye    = normalize(spotE);
    vec3 SpotHalf   = normalize(spotH);
	
    float Kd = max(dot(Normal, Light), 0.0);
    float KdSpot = max(dot(SpotNormal, SpotLight), 0.0);

    float Ks = pow(max(dot(reflect(-Light, Normal),Eye), 0.0), shininess);
    float KsSpot = pow(max(dot(reflect(-SpotLight, SpotNormal), SpotEye), 0.0), shininess);

    float Ka = 1.0;
    float KaSpot = 1.0;

    vec4 diffuse  = Kd * lightDiffuse*surfaceDiffuse;
    vec4 specular = Ks * lightSpecular*surfaceSpecular;
    vec4 ambient  = Ka * lightAmbient*surfaceAmbient;

    vec4 diffuseSpot  = KdSpot * lightDiffuse*surfaceDiffuse;
    vec4 specularSpot = KsSpot * lightSpecular*surfaceSpecular;
    vec4 ambientSpot  = KaSpot * lightAmbient*surfaceAmbient;

    vec3 spotlightPosition = SpotLight;
    float spotFactor = 1.0;  
    vec3 spotlightDirection = -normalize(spotDir);  
    float spotCosine = dot(spotlightDirection, spotlightPosition);

    if (spotCosine >= cutoff) { 
        spotFactor = pow(spotCosine, spotFactor);
    }
    else { 
        spotFactor = 0.0; // The light will add no color to the point.
    }
    
	gl_FragColor = surfaceEmissive + ambient + ( diffuse + specular);
}

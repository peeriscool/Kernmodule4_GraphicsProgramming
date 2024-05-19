#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
vec3 lerp(vec3 a, vec3 b, float t) {
    return a + (b - a) * t;
}
vec4 lerp(vec4 a, vec4 b, float t) {
    return a + (b - a) * t;
}

void main()
{
    
    vec4 diffuse = texture(texture_diffuse1, TexCoords);
    vec4 specTex = texture(texture_specular1, TexCoords);

    float light = max(dot(-lightDirection, Normals), 0.0);

    vec3 viewDir = normalize(FragPos.rgb - cameraPosition); //diff
    vec3 refl = reflect(lightDirection, Normals); //spec

    float ambientOcclusion = texture(texture_ao1, TexCoords).r; //AO
    
    float roughness = texture(texture_roughness1, TexCoords).r; //shinny
    float spec = pow(max(dot(-viewDir, refl), 0.0), lerp(1, 128, roughness));
    vec3 specular = spec * specTex.rgb; //is grey scale could be color
    
    float dist = length(FragPos.xyz - cameraPosition);

    float fog = pow(clamp((dist -250)/1500,0,1),2); //shouldent be hard coded should be uniform
	//world color to lerp with fog
	vec3 topColor = vec3(68.0/255.0,118.0 / 255.0, 189.0 / 255.0);
	vec3 botColor = vec3(188.0/255.0,214.0 / 255.0, 231.0 / 255.0);
	vec3 fogcolor = lerp(botColor,topColor,max(-viewDir.y,0.0));

    vec4 result = lerp(diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) + vec4(specular, 0),vec4(fogcolor,1),fog);
   // result.a = diffuse.r;
   //clip at threshhold
   if(result.a < 0.5) discard;
    FragColor = result;
}
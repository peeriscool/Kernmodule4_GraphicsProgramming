#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;
in vec3 worldPosition;
in mat3 tbn;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
void main()
{
	//normal
	vec3 normal = texture(normalTex,uv).rgb;
	normal = normalize(normal * 2.0 -1.0);//-1 +1
	normal.rg = normal.rg *.8;
	normal = tbn * normal;
	
	vec3 lightDirection = normalize(worldPosition-lightPosition);

	//specular
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	vec3 reflDir = normalize(reflect(lightDirection,normal));

	//lighting
	float lightValue = max(-dot(normal,lightDirection),0.0); //directional light
	float specular = pow(max(-dot(reflDir,viewDir),0.0),8);//not negative values power of 8 for curvature in light
	//seperate rgb and alhpa calculations
	vec4 result = vec4(color,1.0f) * texture(diffuseTex,uv);
	result.rgb = result.rgb * min(lightValue + 0.3, 1.0) + specular * result.rgb;
	
	FragColor = result ;//vec4(1.0f,0.5f,0.2f,1.0f); //vec4(uv,0.0f,1.0f);
}
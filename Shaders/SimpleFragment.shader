#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D diffuse;
uniform vec3 lightPosition;
void main()
{
	//lighting
	vec3 lightDirection = normalize(worldPosition-lightPosition);
	float lightvalue = max(-dot(normal,lightDirection),0.0); //directional light

	FragColor = vec4 (color,1.0f) * texture(diffuse,uv) * min(lightvalue + 0.3, 1.0);//vec4(1.0f,0.5f,0.2f,1.0f); //vec4(uv,0.0f,1.0f);
}
#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;
in vec3 worldPosition;

uniform vec3 Kd;
uniform vec3 Spec;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform float time;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
void main()
{

	vec3 lightDirection = normalize(worldPosition-lightPosition);
	 float light = max(dot(-lightDirection,worldPosition ), time);
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	//vec4 result = vec4(color,time) * texture(diffuseTex,vec2(time,time)) * time; // *  + texture;
	//vec4 result = vec4(color,time);
	vec4 result = vec4(Kd , 1.0);
	//vec4 result = vec4(color,1.0f);// * texture(diffuseTex,uv) + texture(normalTex,uv);
	FragColor = result ;//vec4(1.0f,0.5f,0.2f,1.0f); //vec4(uv,0.0f,1.0f);
}
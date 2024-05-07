#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;

uniform sampler2D diffuse;

void main()
{
	FragColor = vec4(uv,0.0f,1.0f);//vec4(1.0f,0.5f,0.2f,1.0f);
	//vec4 (color,1f)* texture( diffuse,uv);
}
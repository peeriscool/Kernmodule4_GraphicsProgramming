#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out vec3 color;
out vec2 uv;

out vec3 worldPosition;
//out vec3 normal;
uniform mat4 world,view,projection;
uniform sampler2D diffuseTex;
void main()
{
	vec4 worldPos = world * vec4(aPos,1.0);
	
	worldPos.y += texture(diffuseTex,vUV).r * 100.0f; //displacement

	gl_Position = projection * view * worldPos;

	uv = vUV;
	//normal = vNormal;
	worldPosition = mat3(world) * aPos;
}
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 Vcolor;
layout(location = 2) in vec2 vUV;

out vec3 color;
out vec2 uv;
out vec3 worldPosition;
//out vec3 position;
uniform mat4 world,view,projection;
uniform float time;
void main()
{


	float displacement = sin(time) * 0.5;

	vec3 displacedPosition = aPos + vec3(0.0, displacement, 0.0);

	//gl_Position = * vec4(displacedPosition, 1.0);
	//gl_Position += time;

	gl_Position = projection * view * world * vec4(displacedPosition,1.0);
	color = Vcolor;
	uv = vUV;
	worldPosition = mat3(world) * aPos;
}
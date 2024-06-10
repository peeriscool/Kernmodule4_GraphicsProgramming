#version 330 core
out vec4 FragColor;
in vec4 worldPosition;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

vec3 lerp(vec3 a, vec3 b , float t)
{
	return a + (b-a) * t;
}
void main()
{

	vec3 topColor = vec3(168.0/255.0,118.0 / 255.0, 189.0 / 255.0);
	vec3 botColor = vec3(68.0/255.0,118.0 / 255.0, 89.0 / 255.0);

	vec3 viewDir = normalize(worldPosition.rgb - cameraPosition);

	vec3 sunColor = vec3(1.0, 200 / 255.0 , 50 / 255.0);
	float sunProduct = pow(dot(-viewDir,lightDirection),128);
	
	FragColor = vec4(lerp(topColor,botColor,max(-viewDir.y,10.0)) + sunProduct * sunColor,1);//vec4(lerp(botColor,topColor,abs(viewDir.y)) + sunProduct * sunColor,1) ;

	//FragColor = vec4(lerp(botColor,topColor,abs(viewDir.y)) + sunProduct * sunColor,1) ;
	
}


#version 330 core
out vec4 FragColor;
in vec2 uv;
//in vec3 normal;
in vec3 worldPosition;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform sampler2D dirt, sand, grass, rock, snow;
uniform vec3 lightDirection;
uniform vec3 cameraPosition;
vec3 lerp(vec3 a, vec3 b , float t)
{
	return a + (b-a) * t;
}
void main()
{
	//normal
	vec3 normal = texture(normalTex,uv).rgb;
	normal = normalize(normal * 2.0 -1.0);//-1 +1
	normal.gb = normal.bg;
	normal.r = -normal.r;
	normal.b = -normal.b;
	//specular
	vec3 viewDir = normalize(worldPosition.rgb - cameraPosition);
	//vec3 reflDir = normalize(reflect(lightDirection,normal));

	//lighting
	float lightValue = max(-dot(normal,lightDirection),0.0); //directional light
	//float specular = pow(max(-dot(reflDir,viewDir),0.0),8);//not negative values power of 8 for curvature in light
	//seperate rgb and alhpa calculations

	float y = worldPosition.y;

	float ds =clamp((y-10)/10 ,-1,1) *.5+.5;
	float sg = clamp((y-20)/10 ,-1,1) *.5+.5;
	float gr = clamp((y-30)/10 ,-1,1) *.5+.5;
	float rs = clamp((y-40)/10 ,-1,1) *.5+.5;
	
	
	float dist = length(worldPosition.xyz - cameraPosition);
	float uvLerp = clamp((dist-250) / 150,-1,1) * .5 + .5;

	vec3 dirtcolorClose = texture(dirt,uv *100).rgb;
	vec3 sandcolorClose = texture(sand,uv *100).rgb;
	vec3 grasscolorClose = texture(grass,uv *100).rgb;
	vec3 rockcolorClose = texture(rock,uv *100).rgb;
	vec3 snowcolorClose = texture(snow,uv *100).rgb;

	vec3 dirtcolorFar = texture(dirt,uv *10).rgb;
	vec3 sandcolorFar = texture(sand,uv *10).rgb;
	vec3 grasscolorFar = texture(grass,uv *10).rgb;
	vec3 rockcolorFar = texture(rock,uv *10).rgb;
	vec3 snowcolorFar = texture(snow,uv *10).rgb;

	vec3 dirtcolor = lerp(dirtcolorClose,dirtcolorFar,uvLerp);
	vec3 sandcolor = lerp(sandcolorClose,sandcolorFar,uvLerp);;
	vec3 grasscolor = lerp(grasscolorClose,grasscolorFar,uvLerp);;
	vec3 rockcolor = lerp(rockcolorClose,rockcolorFar,uvLerp);;
	vec3 snowcolor = lerp(snowcolorClose,snowcolorFar,uvLerp);;

	vec3 diffuse = lerp(lerp(lerp(lerp(dirtcolor,sandcolor,ds),grasscolor,sg),rockcolor,gr),snowcolor,rs);

	float fog = pow(clamp((dist -250)/1500,0,1),2);
	//world color to lerp with fog
	vec3 topColor = vec3(68.0/255.0,118.0 / 255.0, 189.0 / 255.0);
	vec3 botColor = vec3(188.0/255.0,214.0 / 255.0, 231.0 / 255.0);
	vec3 fogcolor = lerp(botColor,topColor,max(-viewDir.y,0.0));


	vec4 result = vec4( lerp(diffuse * min(lightValue + 0.1, 1.0),fogcolor,fog),1.0);
	
	//vec4 result = texture(dirt,uv);
	//result.rgb = result.rgb * min(lightValue + 0.3, 1.0); //+ specular * result.rgb
	//result.rgb = diffuse * (min(lightValue + 0.1, 1.0)); //+ specular * result.rgb
	
	FragColor = result ;//vec4(1.0f,0.5f,0.2f,1.0f); //vec4(uv,0.0f,1.0f);
}
/*
vec3 dirtcolor = texture(dirt,uv *10).rgb;
	vec3 sandcolor = texture(sand,uv *10).rgb;
	vec3 grasscolor = texture(grass,uv *10).rgb;
	vec3 rockcolor = texture(rock,uv *10).rgb;
	vec3 snowcolor = texture(snow,uv *10).rgb;
	*/
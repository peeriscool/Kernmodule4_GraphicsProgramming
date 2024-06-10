#version 330 core
out vec4 FragColor;
in vec4 worldPosition;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;
uniform float horizonLevel; // New uniform to control the horizon level

vec3 lerp(vec3 a, vec3 b , float t)
{
	return a + (b-a) * t;
}
void main()
{
    vec3 topColor = vec3(0.0/255.0, 0.0/255.0, 139.0/255.0);
	vec3 midColor = vec3(135.0/255.0, 126.0/255.0, 235.0/255.0); 
	vec3 botColor = min(vec3(0.0), midColor - 0.1); 
	vec3 viewDir = normalize(worldPosition.rgb - cameraPosition);
	// Calculate the mix factor based on view direction
    float t = max(0.0, (viewDir.y + horizonLevel) / (1.0 + horizonLevel));

	 vec3 midLerp = lerp(botColor * 0.5, midColor * 0.7, t);
	//vec3 midLerp = lerp(botColor, midColor, t);
    vec3 skyColor = lerp(midLerp, topColor, t);
	vec3 sunColor = vec3(1.0, 0.78 , 0.2);
	vec3 Stars = vec3(0,0,0);
	// Add stars
    // Randomly generate stars based on world position
   float starDensity = 0.005; // Adjust as needed
	float starThreshold = 0.099; // Adjust as needed
	float starNoise = fract(sin(dot(worldPosition.xyz, vec3(0.20023107, 0.19970112, 0.666))) * 69420.007);
 if (starNoise < starDensity && t > starThreshold)
    {
        // Vary star intensity based on distance
        float starDistance = length(worldPosition.xyz - vec3(0,0,0));
        float starIntensity = 4.0 / pow(starDistance, 0.4); // Adjust power for desired effect
        // Add white color for stars with faded intensity
        Stars += vec3(1.0) * starIntensity;
    }

	float sunIntensity = pow(max(dot(viewDir, lightDirection), 0.0) , 64.0);
	
	vec3 finalColor = lerp (skyColor * 1.5,Stars,t) + sunIntensity * sunColor;
	//vec3 finalColor =skyColor;
    FragColor = vec4(finalColor, 1.0);
	
}


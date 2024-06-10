#version 330 core

out vec4 FragColor;
in vec3 normal;

// Material properties
uniform float Ns;    // Specular exponent
uniform vec3 Ka;     // Ambient color
uniform vec3 Kd;     // Diffuse color
uniform vec3 Ks;     // Specular color
uniform vec3 Ke;     // Emissive color
uniform float Ni;    // Optical density
uniform float d;     // Dissolve (transparency)
uniform int illum;   // Illumination model

uniform float time;
// Light and view direction
uniform vec3 lightDirection;
uniform vec3 viewDirection;

void main()
{
    // Example: Phong lighting model
    // Calculate ambient, diffuse, and specular components based on material properties
    // Note: This is a simplified example, you can implement more advanced lighting models as needed

    vec3 ambient = Ka;
    vec3 diffuse = Kd;
    vec3 specular = Ks;
    
    // Ambient term
    vec3 ambientColor = ambient * vec3(0.1); // Example ambient light color (adjust as needed)

    // Diffuse term (Lambertian reflection)
    // Example: Assuming a single light source at position (0, 0, 1)
    //vec3 lightDir = normalize(vec3(0, 0, 1)); // Example light direction
    float diffuseIntensity = max(dot(lightDirection*4, normalize(normal)), 0.1);
    vec3 diffuseColor = diffuse * diffuseIntensity;

    // Specular term (Phong reflection)
    // Example: Assuming a viewer at position (0, 0, -1)
    //vec3 viewDir = normalize(vec3(0, 0, -1)); // Example view direction
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), Ns);
    vec3 specularColor = specular * specularIntensity;

    // Final color (with transparency)
    vec3 finalColor = ambientColor + diffuseColor + specularColor + Ke;
    FragColor = vec4(finalColor, d); // Include transparency (dissolve) factor
}
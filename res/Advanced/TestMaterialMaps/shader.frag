#version 330

layout (location = 0) out vec4 glColor;

in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 viewPosition;
uniform float specularIntensity;

uniform sampler2D diffuseMap, specularMap;

void main() {
    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPosition - vPosition);
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * texture(diffuseMap, vTexCoord).rgb;

    // specular
    vec3 viewDir = normalize(viewPosition - vPosition);
    vec3 halfway = normalize(viewDir + lightDir);
    float specularStrength = pow(max(dot(norm, halfway), 0.0), 32) * specularIntensity;
    vec3 specular = specularStrength * texture(specularMap, vTexCoord).rgb;

    // add it all together
    vec3 lighting = (ambientStrength + diffuse + specular) * lightColor;
    glColor = vec4(lighting, 1.0);
}
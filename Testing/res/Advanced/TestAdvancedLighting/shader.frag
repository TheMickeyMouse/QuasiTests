#version 330

layout (location = 0) out vec4 glColor;

in vec3 vPosition;
in vec3 vNormal;
flat in int vMatId;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 viewPosition;
uniform float specularIntensity;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

#define MATERIAL_NUM 8
uniform Material materials[MATERIAL_NUM];

void main() {
    Material m = materials[vMatId];

    // ambient
    vec3 ambient = ambientStrength * m.ambient;

    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPosition - vPosition);
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * m.diffuse;

    // specular
    vec3 viewDir = normalize(viewPosition - vPosition);
    vec3 halfway = normalize(viewDir + lightDir);
    float specularStrength = pow(max(dot(norm, halfway), 0.0), m.shininess) * specularIntensity;
    vec3 specular = specularStrength * m.specular;

    // add it all together
    vec3 lighting = (ambient + diffuse + specular) * lightColor;
    glColor = vec4(lighting, 1.0);
}
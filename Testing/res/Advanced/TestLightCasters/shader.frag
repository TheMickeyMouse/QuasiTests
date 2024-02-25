#version 330

layout (location = 0) out vec4 glColor;

in vec3 vPosition;
in vec3 vNormal;
flat in int vMatId;

uniform float ambientStrength;
uniform vec3 viewPosition;
uniform float specularIntensity;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

#define SUNLIGHT_ID 1
#define POINTLIGHT_ID 2
#define FLASHLIGHT_ID 3
struct Light {
    int lightId;
    // union like part
    // data memory: (000 1111 222) (333 4444 555) 6666
    // sunlight:    [ .direction ] -------------------
    // pointlight:  [ .position  ] [.c] [.l] [.q] ----
    // flashlight:  [ .position  ] [.y] [.p] [.i] [.o]
    vec3 d1, d2; float d3;

    vec4 color;
};

#define MATERIAL_NUM 8
uniform Material materials[MATERIAL_NUM];
#define LIGHT_NUM 8
uniform Light lights[LIGHT_NUM];

vec3 Diffuse(vec3 color, vec3 norm, vec3 lightDir) {
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    return diffuseStrength * color;
}

vec3 Specular(vec3 color, float shininess, vec3 norm, vec3 lightDir) {
    vec3 viewDir = normalize(viewPosition - vPosition);
    vec3 halfway = normalize(viewDir + lightDir);
    float specularStrength = pow(max(dot(norm, halfway), 0.0), shininess) * specularIntensity;
    return specularStrength * color;
}

vec3 Phong(Material m, vec4 color, vec3 lightDir) {
    vec3 ambient = ambientStrength * m.ambient;

    vec3 norm = normalize(vNormal);
    vec3 diffuse = Diffuse(m.diffuse, norm, lightDir);
    vec3 specular = Specular(m.specular, m.shininess, norm, lightDir);

    return (ambient + diffuse + specular) * color.rgb * color.a;
}

vec3 PointLight(Material m, Light l) {
    // light attenuation
    float dist = length(l.d1 - vPosition);
    float att = 1.0 / (l.d2.x + l.d2.y * dist + l.d2.z * dist * dist);

    return Phong(m, l.color, normalize(l.d1 - vPosition)) * att;
}

vec3 SunLight(Material m, Light l) {
    return Phong(m, l.color, normalize(-l.d1));
}

vec3 FlashLight(Material m, Light l) {
    vec3 lightDir = normalize(l.d1 - vPosition);
    vec3 dir = vec3(sin(l.d2.x) * cos(l.d2.y), sin(l.d2.y), cos(l.d2.x) * cos(l.d2.y));
    float angle = dot(lightDir, dir);
    vec3 amb = ambientStrength * m.ambient * l.color.rgb * l.color.a;
    return smoothstep(cos(l.d3), cos(l.d2.z), angle) * (Phong(m, l.color, lightDir) - amb) + amb;
}

void main() {
    Material m = materials[vMatId];

    vec3 lighting = vec3(0.0);
    for (int i = 0; i < LIGHT_NUM; ++i) {
        Light l = lights[i];
        switch (l.lightId) {
            case 0:
                break;
            case SUNLIGHT_ID:
                lighting += SunLight(m, l);
                break;
            case POINTLIGHT_ID:
                lighting += PointLight(m, l);
                break;
            case FLASHLIGHT_ID:
                lighting += FlashLight(m, l);
                break;
            default:
                break;
        }
    }
    glColor = vec4(lighting, 1.0);
}
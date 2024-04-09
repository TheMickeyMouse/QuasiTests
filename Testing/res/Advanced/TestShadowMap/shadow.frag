#version 330 core

layout (location = 0) out vec4 glColor;

in VS_OUT {
    vec3 Position;
    vec3 Normal;
    vec4 Color;
    vec4 LightSpace;
} fIn;

uniform sampler2D depthMap;
uniform vec3 lightPos, viewPos;
uniform float ambStrength;
uniform bool useSmoothShadows;

float getShadowFactor(vec4 lightSpace, vec3 lightDir) {
    // shaodws
    vec3 lightProjected = (lightSpace.xyz / lightSpace.w) * 0.5 + 0.5;
    float actualDepth = lightProjected.z;

    if (actualDepth > 1.0) return 1.0;

    if (useSmoothShadows) {
        float shadowFactor = 0;
        vec2 texelSize = 1.0 / textureSize(depthMap, 0);
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float lightCastedDepth = texture(depthMap, lightProjected.xy + vec2(x, y) * texelSize).r;
                shadowFactor += actualDepth > lightCastedDepth ? 0.0 : ((2 - abs(x)) * (2 - abs(y))) / 16.0;
            }
        }
        return shadowFactor;
    } else {
        float lightCastedDepth = texture(depthMap, lightProjected.xy).r;
        float shadowFactor = actualDepth > lightCastedDepth ? 0.0 : 1.0;
        return shadowFactor;
    }
}

// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
void main() {
    // normal blinn-phong
    vec3 lightDir = normalize(lightPos - fIn.Position);
    float diffuse = max(dot(lightDir, fIn.Normal), 0.0);

    vec3 viewDir = normalize(viewPos - fIn.Position);
    vec3 halfway = normalize(lightDir + viewDir);
    float specular = pow(max(dot(halfway, fIn.Normal), 0.0), 32.0);

    glColor = vec4((ambStrength + getShadowFactor(fIn.LightSpace, lightDir) * (diffuse + specular)) * fIn.Color.rgb, fIn.Color.a);
}

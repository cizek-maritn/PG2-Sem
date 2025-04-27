#version 460 core

//point light struct
struct PointLight {
    vec3 Pposition;
    vec3 Pambient;
    vec3 Pdiffuse;
    vec3 Pspecular;

    float Pconstant;
    float Plinear;
    float Pquadratic;
};

uniform PointLight pointLights[3];

// Outputs colors in RGBA
out vec4 FragColor;

// Material properties
uniform vec3 ambient_intensity, diffuse_intensity, specular_intensity;
uniform vec3 ambient_material, diffuse_material, specular_material;
uniform float specular_shinines;
uniform float alpha;

// active texture unit
uniform sampler2D tex0;
uniform bool useTexture;

// spotlight variables
uniform bool useSpotlight;
uniform vec3 spotlight_position, spotlight_direction;
uniform float spotlight_cutOff, spotlight_outerCutOff;
uniform float spotlight_constant, spotlight_linear, spotlight_quadratic;

// Input from vertex shader
in VS_OUT {
    vec3 N;
    vec3 L;
    vec3 V;
    vec2 texCoord;
} fs_in;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.Pposition - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), specular_shinines);

    float distance = length(light.Pposition - fragPos);
    float attenuation = 1.0 / (light.Pconstant + light.Plinear * distance + light.Pquadratic * (distance*distance));

    vec3 ambient = light.Pambient * ambient_material;
    vec3 diffuse = light.Pdiffuse * diff * diffuse_material;
    vec3 specular = light.Pspecular * spec * specular_material;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main(void) {
    // Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);
    vec3 H = normalize(L + V); // halfway vector

    vec3 baseColor;
    if (useTexture) {
        baseColor = texture(tex0, fs_in.texCoord).rgb;
    } else {
        baseColor=diffuse_material;
    }

    // Directiona light, always active
    vec3 ambient = ambient_material * ambient_intensity;
    vec3 diffuse = max(dot(N, L), 0.0) * diffuse_material * diffuse_intensity;
    vec3 specular = pow(max(dot(N, H), 0.0), specular_shinines) * specular_material * specular_intensity;

    vec3 color = (ambient+diffuse) * (baseColor + specular);

    if (useSpotlight) {
        vec3 fragPos = -fs_in.V;
        vec3 lightDir = normalize(spotlight_position-fragPos);
        float theta = dot(lightDir, normalize(-spotlight_direction));

        float epsilon = spotlight_cutOff - spotlight_outerCutOff;
        float intensity = clamp((theta-spotlight_outerCutOff)/epsilon, 0.0, 1.0);

        //attenuation
        float distance = length(spotlight_position - fragPos);
        float attenuation = 1.0 / (spotlight_constant + spotlight_linear * distance + spotlight_quadratic * (distance*distance));

        //light contribution
        float diffs = max(dot(N, lightDir), 0.0);
        vec3 spotDiff = diffs * diffuse_material * diffuse_intensity;
        vec3 spotH = normalize(lightDir + V);
        float specS = pow(max(dot(N, spotH), 0.0), specular_shinines);
        vec3 spotSpec = specS * specular_material * specular_intensity;

        vec3 spotResult = (ambient + spotDiff) * baseColor + spotSpec;
        spotResult *= intensity * attenuation;

        color += spotResult;
    }

    vec3 fragPos = -fs_in.V;

    for (int i=0; i<3; i++) {
        color += CalcPointLight(pointLights[i], N, fragPos, V);
    }

    FragColor = vec4(color, alpha);
}


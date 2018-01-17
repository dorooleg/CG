#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform int lightCount;
uniform int typeRender;

#define ALL 0
#define POSITION 1
#define NORMAL 2
#define DIFFUSE 3
#define SPECULAR 4

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 512;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform float gammaCorrection;

void main()
{             
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = pow(texture(gAlbedoSpec, TexCoords).rgb, vec3(gammaCorrection));
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    vec3 lighting  = Diffuse * 0.1;
    vec3 viewDir  = normalize(viewPos - FragPos);

    for(int i = 0; i < lightCount; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        if (typeRender == ALL)
            lighting += diffuse + specular;    
            
        if (typeRender == DIFFUSE)
            lighting += diffuse;  

        if (typeRender == SPECULAR)
            lighting += specular;  
    }

    lighting = pow(lighting, vec3(1.0 / gammaCorrection));
    if (typeRender == ALL)
        FragColor = vec4(lighting, 1.0);
    if (typeRender == POSITION)
        FragColor = vec4(FragPos, 1.0);
    if (typeRender == NORMAL)
        FragColor = vec4(Normal, 1.0);
    if (typeRender == DIFFUSE)
        FragColor = vec4(lighting, 1.0);
    if (typeRender == SPECULAR)
        FragColor = vec4(lighting, 1.0);
}

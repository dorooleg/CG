#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform Material material;
uniform Light light;

vec3 BlinnPhong(Light light, Material material, vec3 viewPos, vec3 normal, vec3 frag_pos)
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - frag_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
    return ambient + diffuse + specular;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            // Bilinear filtering
            vec2 start = projCoords.xy + vec2(x, y) * texelSize;

            float pcfDepth = texture(shadowMap, start).r;
            float r00 = currentDepth - bias > pcfDepth  ? 1.0 : 0.0;

            pcfDepth = texture(shadowMap, start + vec2(texelSize.x, 0.0)).r;
            float r10 = currentDepth - bias > pcfDepth  ? 1.0 : 0.0;

            pcfDepth = texture(shadowMap, start + vec2(0.0, texelSize.y)).r;
            float r01 = currentDepth - bias > pcfDepth  ? 1.0 : 0.0;

            pcfDepth = texture(shadowMap, start + texelSize).r;
            float r11 = currentDepth - bias > pcfDepth  ? 1.0 : 0.0;

            vec2 ratio = start / texelSize - floor(start / texelSize);
            vec2 opposite = 1.0 - ratio;

            shadow += (r00 * opposite.x  + r10 * ratio.x) * opposite.y + (r01 * opposite.x + r11 * ratio.x) * ratio.y;
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec3 normal = normalize(fs_in.Normal);

    vec3 color = vec3(0.3, 0.9, 0.3);

    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    
     vec3 blinn = BlinnPhong(light, material, viewPos, fs_in.Normal, fs_in.FragPos);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * blinn; 

    FragColor = vec4(lighting + blinn, 1.0);
}
#version 330 core
out vec4 fragcolor;

in vec3 normal; 
in vec3 fragpos;
in vec4 fragpos_light;

uniform sampler2D depth_map;

uniform vec3 light_direction; 
uniform vec3 camera_position;

uniform vec3 object_f0;
uniform vec3 object_color;
uniform vec3 light_color;

uniform float metallic;
uniform float roughness;
uniform float ao;

const float PI = 3.1415926535898;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main()
{
    // // BRDF
    vec3 N = normalize(normal);
    vec3 V = normalize(camera_position - fragpos);
    vec3 F0 = mix(object_f0, object_color, metallic);
    vec3 Lo = vec3(0.0);
    vec3 L = normalize(light_direction);
    vec3 H = normalize(V + L);
    // float distance = length(light_position - fragpos);
    // float attenuation = 1.0 / (distance * distance);
    // vec3 radiance = light_color * attenuation;
    vec3 radiance = light_color;
    float NDF = DistributionGGX(N, H, roughness);   
    float G = GeometrySmith(N, V, L, roughness);      
    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    vec3 numerator = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    float NdotL = max(dot(N, L), 0.0); 
    Lo = (kD * object_color / PI + specular) * radiance * NdotL;
    
    

    vec3 proj_coords = fragpos_light.xyz / fragpos_light.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    float closest_depth = texture(depth_map, proj_coords.xy).r; 
    float current_depth = proj_coords.z;
    float shadow = current_depth - 0.005 > closest_depth  ? 1.0 : 0.0;
    shadow = min(shadow, 0.75);
    // shadow = 0.0;

    // vec3 result = (object_color * ao + Lo*(1.0-shadow));
    // vec3 result = (object_color * ao + Lo);
    // fragcolor = vec4(vec3(1.0 - shadow), 1);
    fragcolor = vec4(vec3(1.0 - shadow), 1);

    // result = result / (result + vec3(1.0));
    // fragcolor = vec4(result, 1.0f);
}

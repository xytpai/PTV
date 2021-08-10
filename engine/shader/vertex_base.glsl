#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 fragpos;
out vec3 normal;
out vec4 fragpos_light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 light_view;
uniform mat4 light_projection;

void main()
{
	fragpos = vec3(model * vec4(in_position, 1.0));
	normal = mat3(transpose(inverse(model))) * in_normal;
	gl_Position = projection * view * model * vec4(in_position, 1.0f);
	fragpos_light = light_view * light_projection * vec4(fragpos, 1.0);
}
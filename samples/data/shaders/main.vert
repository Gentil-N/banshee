#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoords;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec2 frag_texcoords;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec4 frag_sun_dir;
layout(location = 3) out vec4 frag_sun_color;
layout(location = 4) out vec4 frag_ambient;

layout(binding = 0, set = 0) uniform Scene {
       mat4 projection;
       mat4 view;
       vec4 sun_dir;
       vec4 sun_color;
       vec4 ambient;
} scene;

layout(binding = 0, set = 1) uniform Object {
       mat4 transform;
} object;

void main() {
       gl_Position = scene.projection * scene.view * object.transform * vec4(in_position, 1.0);
       frag_texcoords = in_texcoords;
       frag_normal = (object.transform * vec4(in_normal, 0.0)).xyz;
       frag_sun_dir = scene.sun_dir;
       frag_sun_color = scene.sun_color;
       frag_ambient = scene.ambient;
}
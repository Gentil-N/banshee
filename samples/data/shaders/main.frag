#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 frag_texcoords;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec4 frag_sun_dir;
layout(location = 3) in vec4 frag_sun_color;
layout(location = 4) in vec4 frag_ambient;

layout(location = 0) out vec4 out_color;

layout(binding = 0, set = 2) uniform sampler2D texture_sampler;

void main() {
       float weight = max(dot(normalize(frag_normal), normalize(frag_sun_dir.xyz)), 0.0);
       out_color = texture(texture_sampler, frag_texcoords) * (frag_sun_color * weight + frag_ambient);
}
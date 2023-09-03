#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 frag_texcoords;
layout(location = 1) in vec4 frag_sun_color;

layout(location = 0) out vec4 out_color;

layout(binding = 1, set = 1) uniform sampler2D texture_sampler;

void main()
{
       out_color = texture(texture_sampler, frag_texcoords) * frag_sun_color;
}
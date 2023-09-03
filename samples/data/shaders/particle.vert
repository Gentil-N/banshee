#version 450
#extension GL_ARB_separate_shader_objects : enable

// vertex
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoords;
//instance
layout(location = 2) in vec3 in_inst_position;
layout(location = 3) in vec2 in_inst_tex_index;
layout(location = 4) in float in_inst_scale;

layout(location = 0) out vec2 frag_texcoords;
layout(location = 1) out vec4 frag_sun_color;

layout(binding = 0, set = 0) uniform Scene
{
       mat4 projection;
       mat4 view;
       vec4 sun_dir;
       vec4 sun_color;
       vec4 ambient;
} scene;

layout(binding = 0, set = 1) uniform Material
{
       vec2 tex_grid;
} material;

void main()
{
       vec4 c = scene.view * vec4(in_inst_position, 1.0);
       vec4 p = c + vec4(in_position * in_inst_scale, 0.0, 0.0);
       gl_Position = scene.projection * p;
       //gl_Position = scene.projection * billboard * vec4((vec3(in_position, 0.0) + in_inst_position) * in_inst_scale, 1.0);
       frag_texcoords = vec2(1/material.tex_grid.x * (in_inst_tex_index.x + in_texcoords.x), 1/material.tex_grid.y * (in_inst_tex_index.y + in_texcoords.y));
       frag_sun_color = scene.sun_color;
}
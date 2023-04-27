#pragma vertex
#version 330 core

layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec2 aTexture;


out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexture;
}

#pragma fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D tex0;

void main()
{
    FragColor = texture(tex0, TexCoords);
    //FragColor = vec4(240,200,200,255)/255.0;
}
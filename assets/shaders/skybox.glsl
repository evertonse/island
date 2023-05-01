#pragma vertex
#version 330 core 

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
    // 
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    // Septh = 1.0 always because we use pos.w in both w and z coordinate
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
    // We want  flip z axis because apperntly there's a left handed system
    TexCoord = vec3(aPos.x, aPos.y, -aPos.z);
}    

#pragma fragment 
#version 330 core 

out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skybox;

void main() {    
    FragColor = texture(skybox, TexCoord);
}
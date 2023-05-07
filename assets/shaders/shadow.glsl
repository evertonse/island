#pragma vertex
#version 330 core 

// Only need the position of the light
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
	gl_Position = projection * view  * model * vec4(aPos, 1.0f);
}


#pragma fragment 
#version 330 core 

layout(location = 0) out float FragmentDepth;

void main(){
    // Not really needed, OpenGL does it anyway
    //  Could Leave it empty since the depth buffer gets registered automatically
    FragmentDepth = gl_FragCoord.z;
}
//#pragma light
#pragma vertex
#version 330 core 

layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

const vec4 aColor = vec4(230, 198, 199, 255)/255.0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// Outputs the color for the Fragment Shader
out vec4 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 tex_coord;
// Outputs the normal for the Fragment Shader
out vec3 normal;
// Outputs the current position for the Fragment Shader
out vec3 curr_pos;

void main() {
	// calculates current position
	curr_pos = vec3(model * vec4(aPos, 1.0f));
	// Outputs the positions/coordinates of all vertices
	gl_Position = projection * view  * vec4(curr_pos, 1.0);

	// Assigns the colors from the Vertex Data to "color"
	color = aColor;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	tex_coord = aTexture;
	// Assigns the normal from the Vertex Data to "Normal"
	normal = aNormal;
}


#pragma fragment 
#version 330 core 
// Gets the Texture Unit from the main function


in vec4 color;
in vec2 tex_coord;
in vec3 normal;
in vec3 curr_pos;

const vec4 DEUBG_COLOR = vec4(0.1, 1.0, 0.2 , 1.0);
const vec3 light_position = vec3(100.0, 1000.0,10.0);
const vec4 light_color = vec4(1.0, 0.9, 0.9, 1.0);

// Gets the position of the camera from the main function
//uniform vec3 camera_position;
uniform sampler2D tex0;


//The compiler is allowed to optimize away any UNUSED uniforms, which is why you get this error.
//You can safely ignore this. If the uniform isn't found, glGetUniformLocation returns -1. From the OpenGL 4.3 specs, section 7.6.1: 
uniform vec3 cam_position;

out vec4 FragColor;

void debug() {
    FragColor = DEUBG_COLOR;
}

void main() {

	// ambient lighting
	float ambient = 0.28;

	// diffuse lighting
	vec3  normal = normalize(normal);
	vec3  light_direction = normalize(light_position - curr_pos);
	float diffuse = max(dot(normal, light_direction), 0.0); // max method
	//float diffuse = abs(dot(normal, light_direction)); // abs method

    //FragColor = texture(tex0, tex_coord) * diffuse; // uncomment this for only diffuse
    //FragColor = texture(tex0, tex_coord) * (diffuse + ambient); // uncomment this for only diffuse + ambient
    //FragColor.w = 1.0;
    //return;

	// specular lighting
	float specular_light = 0.15;
	vec3  view_direction = normalize(cam_position - curr_pos);
	vec3  reflection_direction = reflect(-light_direction, normal);
	float spec_amount = pow(max(dot(view_direction, reflection_direction), 0.0), 7);
	float specular = spec_amount * specular_light;

	// outputs final color
	FragColor = texture(tex0, tex_coord) * light_color * (diffuse + ambient + specular);

    FragColor.w = 1.0;
}
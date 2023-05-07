//#pragma light
#pragma vertex
#version 330 core 
#define M_PI 3.1415926535897932384626433832795
layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

const vec4 aColor = vec4(230, 198, 199, 255)/255.0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

// Outputs the color for the Fragment Shader
out vec4 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 tex_coord;
// Outputs the normal for the Fragment Shader
out vec3 normal;
// Outputs the current position for the Fragment Shader
out vec3 curr_pos;

float rand(vec3 n) {
    return fract(sin(dot(n, vec3(12.9898, 78.233, 45.5432))) * 43758.5453);
}

float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(mix(mix(rand(vec3(n, n + 1.0, n + 113.0)), 
                       rand(vec3(n + 157.0, n + 1.0, n + 113.0)), f.x), 
                   mix(rand(vec3(n, n + 1.0, n + 113.0)), 
                       rand(vec3(n + 157.0, n + 1.0, n + 113.0)), f.x), f.y), 
               mix(mix(rand(vec3(n, n + 1.0, n + 113.0)), 
                       rand(vec3(n + 157.0, n + 1.0, n + 113.0)), f.x), 
                   mix(rand(vec3(n, n + 1.0, n + 113.0)), 
                       rand(vec3(n + 157.0, n + 1.0, n + 113.0)), f.x), f.y), f.z);
}

float height_factor1(vec3 curr_pos) {

    float dim_factor = 0.015;
    float factor = 0;
    float freq1 = 1.7;
    float freq2 = 1.8;
    float freq3 = 0.8;
    float amp1 = 0.15;
    float amp2 = 0.12;
    float amp3 = 0.17;
    float slow = 0.0068;
    
    // calculate terrain deformation using sine and cosine functions
    factor += amp1 * sin(time*slow * freq1 * curr_pos.x + rand(curr_pos.xxx));
    factor += amp2 * cos(time*slow * freq2 * curr_pos.z + rand(curr_pos.zzz));
    factor += amp3 * sin(time*slow * freq3 * (curr_pos.x + curr_pos.z));
    return factor*0.15;
}

float height_factor2(vec3 curr_pos) {
    // calculate current position
    float slow = 0.058;
    // calculate random frequency and amplitude values
    float freq = noise(curr_pos + vec3(time));
    float amplitude = noise(curr_pos + vec3(time));

    float freq3 = 0.5;
    float amp3 = 0.25;
    // calculate final height
    float dim_factor = 0.015;
    float factor = cos(time*slow * curr_pos.z) / 20 + sin(time*slow * curr_pos.x) / 20; 
    factor += amp3 * sin(time*slow * freq3 * (curr_pos.x + curr_pos.z));
    return factor * dim_factor * amplitude * freq;
}

void main() {


	// calculates current position
	curr_pos = vec3(model * vec4(aPos, 1.0f));

    float factor = height_factor1(curr_pos);
    // this one is good, but the before is more varied
    // float factor = cos(0.2*time*curr_pos.z)/10 + sin(0.2*time*curr_pos.x)/10;
    curr_pos.y  *= factor;
	// Outputs the positions/coordinates of all vertices
	gl_Position = projection * view  * vec4(curr_pos, 1.0);

	// Assigns the colors from the Vertex Data to "color"
	color = aColor;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	tex_coord = aTexture;
	// Assigns the normal from the Vertex Data to "Normal"
	normal = aNormal + sin(curr_pos) +factor; 
}


#pragma fragment 
#version 330 core 
// Gets the Texture Unit from the main function


in vec4 color;
in vec2 tex_coord;
in vec3 normal;
in vec3 curr_pos;

const vec4 DEUBG_COLOR = vec4(0.1, 1.0, 0.2 , 1.0);
const vec3 light_position = vec3(0.0, 100.0,10.0);
const vec4 light_color = vec4(1.0, 0.9, 0.9, 1.0);

// Gets the position of the camera from the main function
//uniform vec3 camera_position;
uniform sampler2D tex0;


//The compiler is allowed to optimize away any UNUSED uniforms, which is why you get this error.
//You can safely ignore this. If the uniform isn't found, glGetUniformLocation returns -1. From the OpenGL 4.3 specs, section 7.6.1: 
uniform vec3 cam_position;
uniform float time;

out vec4 FragColor;

void debug() {
    FragColor = DEUBG_COLOR;
}

void main() {
    //debug(); return;

	// ambient lighting
	float ambient = 0.82;

    // facotor for the water
    float dim_factor = 0.0015;
    //float factor = (0.2*sin(time)+1.0)/2.0;
    float factor = (dim_factor*(cos(0.9*time)+ sin(0.95*time))+1.76)/(2.0*1.76);

	vec3  normal = normalize(normal);
	vec3  light_direction = factor*normalize(light_position - curr_pos);
	float diffuse = max(dot(normal, light_direction), 0.0); // max method
	//float diffuse = abs(dot(normal, light_direction)); // abs method

	// specular lighting
	float specular_light = 0.5887;
	vec3  view_direction = normalize(cam_position - curr_pos);
	vec3  reflection_direction = reflect(-light_direction, normal);
	float spec_amount = pow(max(dot(view_direction, reflection_direction), 0.0), 28);
	float specular = spec_amount * specular_light;

    // open gl color
	FragColor = texture(tex0, tex_coord*(factor/2.0)) * light_color * (diffuse + ambient + specular);
    FragColor.w = 0.9209;
}
#pragma once
// Chaging the definition on this file is SAFE mostly
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.291f;
const float SENSITIVITY = 0.25f;
const float FOV = 75.0f;

#define ISLAND_CUBE_VOLUME 2.0
#define ISLAND_ENABLE_SHADOW false 
#define ISLAND_FLAT_TERRAIN false 
#define ISLAND_TERRAIN_OCTAVES 9 
#define ISLAND_TERRAIN_PERSISTENCE 14500.0f 
#define ISLAND_TERRAIN_LACUNARITY 0.655f
//#define ISLAND_TERRAIN_LACUNARITY 0.945f
#define ISLAND_WATER_TENDENCY 0.299f
#define ISLAND_MOVABLE_TENDENCY 5

// Pode trocar esses duas definições abaixo ao descomentar
#define ISLAND_SKYBOX "assets/textures/skybox/sky/"
//#define ISLAND_SKYBOX "assets/textures/skybox/cloud/"

// doenst workd "assets/textures/ocean/ocean2.jpg"
// DONT "assets/textures/ocean/ocean3.jpg"
// DONT "assets/textures/ocean12.jpg"
//"assets/textures/ocean4.jpg"
// DONT"assets/textures/ocean/ocean3.jpg"
// DONT "assets/textures/ocean6.png"
// dont"assets/textures/ocean/ocean4.jpg"
// work but weird with current settings"assets/textures/ocean2.jpg"
// DONT"assets/textures/ocean11.jpg"

//#define ISLAND_OCEAN_TEXTURE "assets/textures/ocean3.jpg"

#define ISLAND_OCEAN_TEXTURE "assets/textures/pexels/ocean1.jpg"
//#define ISLAND_OCEAN_TEXTURE "assets/textures/ocean4.jpg"
        
// NOT THAT GOOD #define ISLAND_OCEAN_TEXTURE "assets/textures/ocean12.jpg"    
// okayge #define ISLAND_OCEAN_TEXTURE "assets/textures/ocean14.jpg"    
// not #define ISLAND_OCEAN_TEXTURE "assets/textures/ocean16.jpg"    
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
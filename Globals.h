#pragma once 

#include "utils.h"
#include "Stars.h"
#include "Hero.h"
#include "Mesh.h"
#include "Text.h"
#include "Enemy.h"
#include "PowerupManager.h"
#include "BulletManager.h"
#include "Framebuffer.h"
#include "UnitSquare.h" 
#include "LaserBlast.h"
#include "Particle.h"

/*
* Globals class for global variable definitions.
*/
class Globals 
{
public:

    // Sound Assets
	Mix_Chunk* pewSound{ Mix_LoadWAV("assets/pew.ogg") };
	Mix_Chunk* bigPewSound{ Mix_LoadWAV("assets/bigpew.ogg") };
	Mix_Chunk* hitSound{ Mix_LoadWAV("assets/hit2.ogg") };
	Mix_Chunk* powerupSound{ Mix_LoadWAV("assets/powerup.ogg") };

    // Star objects.
	Stars stars{ 100 };

    // Various Shader Programs. Used to specify which shaders run for which gameplay objects.
	Program starProg{ "starvs.txt","starfs.txt" };
	Program mainProg{ "vs.txt","fs.txt" };
	Program usqProg{ "usqvs.txt","usqfs.txt" };
	Program skyboxProg{ "skyboxvs.txt","skyboxfs.txt" };

    Sampler sampler;
    Hero hero { vec3(-0.8f,0,0) };
    Mesh skybox { "cube.obj" };
    Text chargeText { "consolefont", 12 };
    Camera camera { vec3(0,0,3), vec3(0,0,0), vec3(0,1,0) };
    Camera hudcamera { vec3(0,0,3), vec3(0,0,0), vec3(0,1,0), 15.0f, 2.0f };
    Mesh asteroidMesh{"asteroid.obj"};
    Framebuffer fbo{256,256,1,GL_RGBA8};
    Framebuffer blurfbo{512,512,1,GL_RGBA32F};
    UnitSquare usq;
    std::vector<std::shared_ptr<LaserBlast>> laserBlasts;
    
    std::set<int> keyset;

    std::vector<vec3> asteroids;
    std::vector<std::shared_ptr<Enemy>> enemies;
    
    PowerupManager powerupManager;
    BulletManager bulletManager;
	std::vector<std::shared_ptr<ParticleSystem>> particles;
    
    bool paused = false;
    int timeToNextSpawn = 1000;
    float bulletCharge = 0.0f;

    static constexpr float hudSize = 0.15f;
    float hudY = 1.0f + hudSize;
    float hudDir = 0;
    
};

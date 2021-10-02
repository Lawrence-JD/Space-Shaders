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

    // Various Shader Programs. Used to compile and specify which shaders run for which gameplay objects.
	Program starProg{ "starvs.txt","starfs.txt" };
	Program mainProg{ "vs.txt","fs.txt" };
	Program usqProg{ "usqvs.txt","usqfs.txt" };
	Program skyboxProg{ "skyboxvs.txt","skyboxfs.txt" };

    // OpenGL sampler.
    Sampler sampler;

    // The player.
    Hero hero { vec3(-0.8f,0,0) };

    // The Skybox.
    Mesh skybox { "cube.obj" };

    // The text for the charge counter.
    Text chargeText { "consolefont", 12 };

    // The player camera.
    Camera camera { vec3(0,0,3), vec3(0,0,0), vec3(0,1,0) };

    // The rear-view camera.
    Camera hudcamera { vec3(0,0,3), vec3(0,0,0), vec3(0,1,0), 15.0f, 2.0f };

    // the Mesh for the asteroids.
    Mesh asteroidMesh{"asteroid.obj"};

    // The standard and blur FBOs
    Framebuffer fbo{256,256,1,GL_RGBA8};
    Framebuffer blurfbo{512,512,1,GL_RGBA32F};

    // The unit square. Used to determine the size of the buffer that gets drawn to the screen.
    UnitSquare usq;

    // The player's lasers.
    std::vector<std::shared_ptr<LaserBlast>> laserBlasts;
    
    // Set of keys for event handling.
    std::set<int> keyset;

    // The asteroid positions.
    std::vector<vec3> asteroids;

    // The enemies.
    std::vector<std::shared_ptr<Enemy>> enemies;
    
    // The managers for the powerups and bullets.
    PowerupManager powerupManager;
    BulletManager bulletManager;

    // The particles.
	std::vector<std::shared_ptr<ParticleSystem>> particles;
    
    // Misc. Gameplay Variables.
    bool paused = false;
    int timeToNextSpawn = 1000;
    float bulletCharge = 0.0f;

    // Rear view mirror values.
    static constexpr float hudSize = 0.15f;
    float hudY = 1.0f + hudSize;
    float hudDir = 0;
    
};

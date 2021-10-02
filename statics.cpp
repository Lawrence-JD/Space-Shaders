#include "utils.h"
#include "Mesh.h"
#include "Program.h"
#include "StraightLineEnemy.h"
#include "Bullet.h"
#include "Hero.h"
#include "Jellyfish.h"
#include "Text.h"
#include "PowerupManager.h"
#include "BulletManager.h"
#include "Framebuffer.h" 
#include "LaserBlast.h"
#include "Particle.h"

/*
* This file contains all static objects used by the game.
*/

const vec3 Ship::deathAxis = normalize(vec3(1,1,1));


std::shared_ptr<Mesh>                       Bullet::            mesh;
std::shared_ptr<Mesh>                       Hero::              mesh;
std::shared_ptr<Mesh>                       Jellyfish::         mesh;
std::map<std::string,std::array<int,4> >    Program::           uniforms;
Program*                                    Program::           current;
std::shared_ptr<Buffer>                     Program::           uboBuffer;
std::vector<char>                           Program::           uboBackingMemory;
std::shared_ptr<Mesh>                       StraightLineEnemy:: mesh;
std::shared_ptr<Program>                    Text::              prog;
std::shared_ptr<Program>                    BillboardManager::  bbprog;
std::shared_ptr<ImageTexture2DArray>        PowerupManager::    tex;
std::shared_ptr<ImageTexture2DArray>        BulletManager::     tex;
Framebuffer*                                Framebuffer::       current;
int                                         Framebuffer::       saved_viewport[4];
std::vector< std::shared_ptr<Buffer> >      LaserBlast::        buffers;
std::vector<bool>                           LaserBlast::        inUse;
std::vector< GLuint >                       LaserBlast::        vaos;
std::shared_ptr<ImageTexture2DArray>        LaserBlast::        tex;
std::shared_ptr<ImageTexture2DArray>        LaserBlast::        etex;
std::map<std::array<int,3>, std::shared_ptr<Framebuffer>>       Framebuffer::blurHelpers;
std::map<int, std::vector<vec4> >           Framebuffer::       blurWeights;
std::shared_ptr<UnitSquare>                 Framebuffer::       blurUsq;
std::shared_ptr<Program>                    Framebuffer::       blurProgram;
std::shared_ptr<Program>					ParticleSystem::	drawprog;
std::shared_ptr<Program>					ParticleSystem::	updateprog;
std::shared_ptr<ImageTexture2DArray>		ParticleSystem::	tex;
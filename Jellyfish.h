#pragma once
#include "glfuncs.h"
#include "math3d.h"
#include "Enemy.h"
#include <cmath>


class Jellyfish : public Enemy 
{
public:
    static std::shared_ptr<Mesh> mesh;
    float initialY;
    float t;
    float amplitude;
    float frequency;
    
    Jellyfish(vec3 pos) : Enemy(pos, 0.075f ) 
    {
        this->initialY = pos.y;
        this->t = uniform(0,3.14159f);
        this->amplitude = uniform(0.05f,0.25f);
        this->frequency = uniform(0.005f,0.02f);
        
        if(mesh == nullptr)
            mesh = std::make_shared<Mesh>("jellyfish.obj");
    }
            
    void draw()
    {
        Enemy::draw(mesh);
    }
        
    void update(int elapsed) override
    {
        Enemy::update(elapsed);
        if(this->state == ALIVE)
        {
            this->t += elapsed * this->frequency;
            this->position.x -= elapsed * 0.001f;
            this->position.y = this->initialY + this->amplitude * sin(this->t);
        }
    }
};

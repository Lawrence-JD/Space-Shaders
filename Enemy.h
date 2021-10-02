#pragma once

#include "Ship.h"

/*
* Class for handling the game's enemies.
*/
class Enemy: public Ship 
{
public:
    // Enemy Gameplay Variables.
    int deathTimer;
    int lifeLeft;
    
    // Base Constructor.
    Enemy(vec3 pos, float radius ) : Ship( pos,radius )
    {
        this->deathTimer = 500;
        this->lifeLeft = 5000;
    }
    
    // Standard Update Function.
    virtual void update(int elapsed)
    {
        if(this->state == DYING)
        {
            this->position.y -= 0.001f*elapsed;
            this->rotAngle += 0.01f*elapsed;
            this->scale -= 0.002f*elapsed;
            this->deathTimer -= elapsed;
            if(this->deathTimer < 0)
                this->state = DEAD;
        }
        else
        {
            this->lifeLeft -= elapsed;
            if( this->lifeLeft < 0 )
                this->state = DEAD;
        }
    }
    
    // Function for setting the state to dying, for animation purposes.
    virtual void hit()
    {
        this->state = DYING;
    }
    
    // Pure virtual draw function.
    virtual void draw() = 0;
    
    // Standard draw function.
    virtual void draw(std::shared_ptr<Mesh> mesh)
    {
        float alphaFactor;
        if(this->state == DYING)
            alphaFactor = this->deathTimer/500.0f;
        else
            alphaFactor = 1.0f;
        // Ship.draw will updateUniforms()
        Ship::draw(mesh,alphaFactor);
    }
};



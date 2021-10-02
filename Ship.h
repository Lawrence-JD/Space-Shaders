#pragma once 

#include "utils.h"
#include "Program.h"
#include "Mesh.h"

/*
* Base class used for the game's actors.
*/
class Ship
{
public:
    
    // Misc. gameplay variables.
    static const vec3 deathAxis;
    vec3 position;
    float radius;
    int state;
    float rotAngle;
    float scale;
    
    // Base constructor. 
    Ship(vec3 pos, float radius )
    {
        this->position = pos;
        this->radius = radius; //for bounding sphere
        this->state = ALIVE;
        this->rotAngle = 0;
        this->scale = 1;
    }

    // Base destructor.
    virtual ~Ship()
    {

    }
    
    // Function for getting the ships projection matrix.
    virtual mat4 getMatrix()
    {
        return scaling( vec3(this->scale,this->scale,this->scale) ) * 
                axisRotation( Ship::deathAxis, this->rotAngle ) *
                translation(this->position);
    }

    // Standard Draw Function.
    void draw(std::shared_ptr<Mesh> mesh, float alphaFactor=1.0)
    {
        Program::setUniform("worldMatrix",getMatrix());
        Program::setUniform("alphaFactor",alphaFactor);
        //mesh will update uniforms
        mesh->draw();
    }
};



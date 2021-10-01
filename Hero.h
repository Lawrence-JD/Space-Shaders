#pragma once

#include "Ship.h"


static const float TWOPI=3.14159265358979323f*2.0f;

class Hero : public Ship
{
public:
    static std::shared_ptr<Mesh> mesh;
    static constexpr float RADIUS = 0.1f;
    int frameTimer = 0;
    float banking=0;
    float pitching=0;
    vec3 right;
    vec3 up; 
    vec3 facing; 
    vec3 laser1Pos,laser2Pos;
    
    Hero(vec3 pos) : Ship(pos,RADIUS) 
    {
        right = vec3(0,0,-1);
        up = vec3(0,1,0);
        facing = vec3(-1,0,0);
        
        if(!Hero::mesh)
            Hero::mesh = std::make_shared< Mesh >("ship1c.obj");
        
        //find laser beam emplacements
        laser1Pos = vec3( 0.032267f, 0.007823f, -0.029361f);
        laser2Pos = vec3( -0.03285f, 0.007823f, -0.029361f);
    }
    
    virtual mat4 getMatrix()
    {
        mat4 M( 
            this->right.x, this->right.y, this->right.z, 0,
            this->up.x, this->up.y, this->up.z, 0,
            -this->facing.x, -this->facing.y, -this->facing.z, 0,
            0,0,0,1
        ) ;
        M = axisRotation( vec3(0,0,-1), this->banking ) * 
                axisRotation( vec3(1,0,0), this->pitching ) *
                M *
                translation(this->position);
        return M;
    }
    
    void draw()
    {
        //updateUniforms is done in Ship.draw
        Ship::draw(Hero::mesh);
    }
    
    void update(int elapsed, std::set<int>& keyset)
    {
        float bankGoal = 0;
        float pitchGoal = 0;
        float spinAmt=0;
        
        if(keyset.find(SDLK_a) != keyset.end())
        {
            spinAmt = elapsed*0.001f;
            bankGoal=-1;
        }
        if(keyset.find(SDLK_d) != keyset.end())
        {
            spinAmt -= elapsed*0.001f;
            bankGoal=1;
        }
            
        if(spinAmt != 0.0f ){
            mat4 M = axisRotation( this->up, spinAmt );
            this->facing = (vec4(this->facing.x, this->facing.y, this->facing.z, 0) * M).xyz();
            this->right = (vec4(this->right.x, this->right.y, this->right.z, 0) * M).xyz();
        }
            
        float pitchAmt = 0;
        if(keyset.find(SDLK_w) != keyset.end() )
        {
            pitchAmt = -elapsed * 0.001f;
            pitchGoal=-1;
        }
        
        if(keyset.find(SDLK_s) != keyset.end() )
        {
            pitchAmt = elapsed * 0.001f;
            pitchGoal=1;
        }

        if(pitchAmt != 0.0f )
        {
            mat4 M = axisRotation( this->right, pitchAmt );
            this->facing = (vec4(this->facing.x, this->facing.y, this->facing.z, 0) * M).xyz();
            this->up = (vec4(this->up.x, this->up.y, this->up.z, 0) * M).xyz();
        }
       
        if(bankGoal == -1 || (bankGoal == 0 && this->banking > 0) )
        {
            this->banking -= 0.01f*elapsed;
            if(bankGoal == 0 && this->banking < 0 ){
                this->banking = 0;
            }
        } 
        else if( bankGoal == 1 || (bankGoal == 0 && this->banking < 0) )
        {
            this->banking += 0.01f*elapsed;
            if( bankGoal == 0 && this->banking > 0){
                this->banking = 0;
            }
        }

        //45 degrees = 0.785 radians
        if(this->banking > 0.785f)
            this->banking = 0.785f;
        if(this->banking < -0.785f)
            this->banking = -0.785f;
            
        if(pitchGoal == -1 || (pitchGoal == 0 && this->pitching > 0) )
        {
            this->pitching -= 0.01f*elapsed;
            if(pitchGoal == 0 && this->pitching < 0)
                this->pitching = 0;
        } 
        else if( pitchGoal == 1 || (pitchGoal == 0 && this->pitching < 0) )
        {
            this->pitching += 0.01f*elapsed;
            if( pitchGoal == 0 && this->pitching > 0 )
                this->pitching = 0;
        }

        if(this->pitching > 0.785f )
            this->pitching = 0.785f;
        if(this->pitching < -0.785f)
            this->pitching = -0.785f;
            
        this->position = elapsed * 0.001f * this->facing + this->position;
    }
};

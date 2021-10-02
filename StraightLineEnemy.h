#pragma once 

#include "Enemy.h"
#include "math3d.h"
#include "Mesh.h"

/*
* Class for handling the basic ship enemies.
*/
class StraightLineEnemy : public Enemy 
{
public:
    // Misc. Gameplay Variables.
    static std::shared_ptr<Mesh> mesh;
    static constexpr float RADIUS = 0.1f;

    // Base Constructor.
    StraightLineEnemy(vec3 pos) : Enemy(pos, RADIUS )
    {
        if( StraightLineEnemy::mesh == nullptr )
            StraightLineEnemy::mesh = std::make_shared<Mesh>("ship2b.obj");
    }
        
    // Standard Update function. Called every frame.
    void update(int elapsed)
    {
        if(state == ALIVE )
            position += elapsed * vec3(-0.001f,0,0);
        Enemy::update(elapsed);
    }
    
    // Standard draw function.
    void draw()
    {
        Enemy::draw(StraightLineEnemy::mesh);
    }
};


#pragma once 

#include "Enemy.h"
#include "math3d.h"
#include "Mesh.h"


class StraightLineEnemy : public Enemy 
{
public:
    static std::shared_ptr<Mesh> mesh;
    static constexpr float RADIUS = 0.1f;
    StraightLineEnemy(vec3 pos) : Enemy(pos, RADIUS )
    {
        if( StraightLineEnemy::mesh == nullptr )
            StraightLineEnemy::mesh = std::make_shared<Mesh>("ship2b.obj");
    }
        

    void update(int elapsed)
    {
        if(state == ALIVE )
            position += elapsed * vec3(-0.001f,0,0);
        Enemy::update(elapsed);
    }
    
    void draw()
    {
        Enemy::draw(StraightLineEnemy::mesh);
    }
};


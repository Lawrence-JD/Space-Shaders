#pragma once

#include "utils.h"
#include "Billboard.h"

/*
* Class for managing all of the Player's Bullets.
* Inherits from Billboard Manager
*/
class BulletManager : public BillboardManager
{
public:
    // The bullet Texture. All of them are the same so it is a shared pointer.
    static std::shared_ptr<ImageTexture2DArray> tex;

    // Texture Frame for texture animations.
    float texFrame = 0.0f;

    // The list of bullets.
    std::vector<std::shared_ptr<Bullet>> bullets;
    
    // Base Constructor.
    BulletManager()
    {
        if(tex == nullptr)
            tex = std::make_shared<ImageTexture2DArray>("redNova.png");
    }
    
    // Add a bullet to the list.
    void add(vec3 p, vec3 v)
    {
        auto b = std::make_shared<Bullet>(p,v);
        BillboardManager::add(b->position);
        bullets.push_back(b);
    }
    
    // Update all bullets. Handles dead bullet removal.
    void update(int elapsed)
    {
        this->texFrame += 0.05f*elapsed;
        while(this->texFrame > tex->slices)
            this->texFrame -= tex->slices;
            
        for(size_t i=0;i<bullets.size();++i){
            bullets[i]->update(elapsed);
            if( bullets[i]->state == DEAD ){
                std::swap(bullets[i], bullets.back() );
                swapWithLastBillboard(i);
                
                bullets.pop_back();
                removeLastBillboard();
                i--;
            }
            else {
                setBillboardPosition( i, bullets[i]->position );
            }
        }
    }
    
    // Templated collision function to check for enemy collision.
    template<typename T>
    bool checkCollisions(std::shared_ptr<T> enemy)
    {
        if( enemy->state != ALIVE )
            return false;
        for( size_t i=0;i<bullets.size();++i)
        {
            if( bullets[i]->state == ALIVE )
            {
                if( bullets[i]->collidesWith( enemy ) )
                {
                    bullets[i]->state = DEAD;
                    return true;
                }
            }
        }
        return false;
    }
    
    // Standard Draw Function.
    void draw()
    {
        // Bind the Texture.
        tex->bind(0);
        // Update the Uniforms to send to the GPU.
        Program::setUniform("billboardSize",vec2(Bullet::BULLET_RADIUS, Bullet::BULLET_RADIUS));
        Program::setUniform("texSlice",(this->texFrame));
        Program::updateUniforms();
        // Draw the bullets.
        BillboardManager::draw();
    }
};

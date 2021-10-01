#pragma once

#include "utils.h"
#include "Billboard.h"

class BulletManager : public BillboardManager
{
public:
    static std::shared_ptr<ImageTexture2DArray> tex;
    float texFrame = 0.0f;
    std::vector<std::shared_ptr<Bullet>> bullets;
    
    BulletManager()
    {
        if(tex == nullptr)
            tex = std::make_shared<ImageTexture2DArray>("redNova.png");
    }
    
    void add(vec3 p, vec3 v)
    {
        auto b = std::make_shared<Bullet>(p,v);
        BillboardManager::add(b->position);
        bullets.push_back(b);
    }
    
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
    
    void draw()
    {
        tex->bind(0);
        Program::setUniform("billboardSize",vec2(Bullet::BULLET_RADIUS, Bullet::BULLET_RADIUS));
        Program::setUniform("texSlice",(this->texFrame));
        Program::updateUniforms();
        BillboardManager::draw();
    }
};

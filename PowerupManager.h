#pragma once

#include "utils.h"
#include "Billboard.h"

class PowerupManager : public BillboardManager
{
public:
    static std::shared_ptr<ImageTexture2DArray> tex;
    float texFrame = 0.0f;
    
    PowerupManager()
    {
        if(tex == nullptr)
            tex = std::make_shared<ImageTexture2DArray>("powerup.ora");
    }
    
    void update(int elapsed)
    {
        this->texFrame += 0.005f*elapsed;

        while(this->texFrame > tex->slices)
            this->texFrame -= tex->slices;
    }
    
      
    bool checkCollisions(vec3 p)
    {
        float radius = 1.0;
        bool rv=false;
        for(size_t i=0;i<positions.size();++i)
        {
            if( length( p-positions[i] ) <= radius )
            {
                swapWithLastBillboard( i );
                removeLastBillboard();
                rv=true;
            }
        }
        return rv;
    }
    
    void draw()
    {
        tex->bind(0);
        Program::setUniform("billboardSize",vec2(0.5f,0.5f));
        Program::setUniform("texSlice",(this->texFrame));
        Program::updateUniforms();
        BillboardManager::draw();
    }
};

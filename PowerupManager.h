#pragma once

#include "utils.h"
#include "Billboard.h"

/*
* Class for managing the game's powerup billboards.
*/
class PowerupManager : public BillboardManager
{
public:
    // The power up texture.
    static std::shared_ptr<ImageTexture2DArray> tex;

    // The texture fram for animation purposes.
    float texFrame = 0.0f;
    
    // Base Constuctor. Sets the texture.
    PowerupManager()
    {
        if(tex == nullptr)
            tex = std::make_shared<ImageTexture2DArray>("powerup.ora");
    }
    
    // Standard update function. Called every frame.
    void update(int elapsed)
    {
        this->texFrame += 0.005f*elapsed;

        while(this->texFrame > tex->slices)
            this->texFrame -= tex->slices;
    }
    
    // Function for checking if the given point collides with any of the billboards using bounding spheres.  
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
    
    // Standard draw function.
    void draw()
    {
        // Bind the texture.
        tex->bind(0);
        // Update the uniforms to send data to the GPU
        Program::setUniform("billboardSize",vec2(0.5f,0.5f));
        Program::setUniform("texSlice",(this->texFrame));
        Program::updateUniforms();
        // Draw to the current FBO
        BillboardManager::draw();
    }
};

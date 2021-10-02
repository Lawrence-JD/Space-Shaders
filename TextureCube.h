#pragma once

#include "Texture.h"

/*
* Base class for Texture cubes.
*/
class TextureCube : public Texture 
{
public:
    // Base Constructor. Sets up the Texture variables.
    TextureCube(unsigned size) : Texture( GL_TEXTURE_CUBE_MAP )
    {
        this->w=size;
        this->h=size;
    }
};

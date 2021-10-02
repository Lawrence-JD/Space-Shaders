#pragma once
#include "glfuncs.h"

/*
* OpenGL texture wrapper class.
*/
class Texture
{
public:
    // Various Texture variables.
    GLenum type;
    GLuint tex=0;
    int w=0;
    int h=0;
    int slices=0;
    
    // Empty Base Constructor.
    Texture(GLenum typ) : type(typ)
    {

    }

    // Binds the texture to a texture unit.
    void bind(int unit)
    {
        assert(tex != 0);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(type,tex);  
    }

    // Unbinds the texture to a texture unit.
    void unbind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(type,0);
    }

    // Wrapper for generating texture Mipmaps.
    void generateMipmap()
    {
        this->bind(0);
        glGenerateMipmap(this->type);
        this->unbind(0);
    }
};

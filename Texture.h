#pragma once
#include "glfuncs.h"

class Texture
{
public:
    GLenum type;
    GLuint tex=0;
    int w=0;
    int h=0;
    int slices=0;
    
    Texture(GLenum typ) : type(typ)
    {

    }

    void bind(int unit)
    {
        assert(tex != 0);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(type,tex);  
    }

    void unbind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(type,0);
    }

    void generateMipmap()
    {
        this->bind(0);
        glGenerateMipmap(this->type);
        this->unbind(0);
    }
};

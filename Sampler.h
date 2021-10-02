#include "glfuncs.h"

/*
* OpenGL Texture Sampler wrapper class.
*/
class Sampler
{
public:
    // The actual Sampler.
    GLuint samp;

    // The base constructor. Sets the default OpenGL sampler attributes for the game.
    Sampler()
    {
        GLuint tmp[1];
        glGenSamplers(1,tmp);
        samp = tmp[0];
        glSamplerParameteri( this->samp, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glSamplerParameteri( this->samp, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glSamplerParameteri( this->samp, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri( this->samp, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    // Function for binding the sampler to a texture unit.
    void bind(unsigned unit)
    {
        glBindSampler(unit, this->samp );
    }
};


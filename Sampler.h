#include "glfuncs.h"


class Sampler
{
public:
    GLuint samp;
    Sampler()
    {
        GLuint tmp[1];
        glGenSamplers(1,tmp);
        samp = tmp[0];
        glSamplerParameteri( this->samp,
            GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glSamplerParameteri( this->samp,
            GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glSamplerParameteri( this->samp, 
            GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri( this->samp, 
            GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    void bind(unsigned unit)
    {
        glBindSampler(unit, this->samp );
    }
};


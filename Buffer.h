#pragma once

#include "glfuncs.h"
#include <vector>

class Buffer
{
public:
    GLuint buffID;
    
    Buffer( int size, GLenum usage = GL_STATIC_DRAW )
    {
        init(nullptr,size,usage);
    }
    
    template<typename T>
    Buffer(const std::vector<T>& initialData, GLenum usage=GL_STATIC_DRAW ){
        init(initialData.data(), initialData.size()*sizeof(initialData[0]), usage);
    }
    
    void init(const void* p, size_t size, GLenum usage)
    {
        GLuint tmp[1];
        glGenBuffers(1,tmp);
        this->buffID = tmp[0];
        setData(p,size,usage);
    }
    

    void bind(GLenum bindingPoint)
    {
        glBindBuffer(bindingPoint, buffID);
    }

    void bindBase(GLenum bindingPoint, int index)
    {
        glBindBufferBase(bindingPoint,index,buffID);
    }
    
    template<typename T>
    void setData(const std::vector<T>& newData, GLenum usage=GL_DYNAMIC_DRAW)
    {
        setData(newData.data(), newData.size()*sizeof(T), usage);
    }
    
    void setData(const void* p, size_t size, GLenum usage)
    {
        bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, size, p, usage);
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
};
        

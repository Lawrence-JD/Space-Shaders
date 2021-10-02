#pragma once

#include "glfuncs.h"
#include <vector>

// Generic class used to handle OpenGL buffers.
class Buffer
{
public:
    // The Buffer's ID.
    GLuint buffID;
    
    // Base Constructor.
    Buffer( int size, GLenum usage = GL_STATIC_DRAW )
    {
        init(nullptr,size,usage);
    }
    
    // Templated Constructor for making buffers of any size for any data type.
    template<typename T>
    Buffer(const std::vector<T>& initialData, GLenum usage=GL_STATIC_DRAW ){
        init(initialData.data(), initialData.size()*sizeof(initialData[0]), usage);
    }
    
    // Buffer initializer, called by the constructors.
    void init(const void* p, size_t size, GLenum usage)
    {
        // Build the buffer
        GLuint tmp[1];
        glGenBuffers(1,tmp);
        this->buffID = tmp[0];
        setData(p,size,usage);
    }
    
    // Binds this buffer to the given OpenGL binding point.
    void bind(GLenum bindingPoint)
    {
        glBindBuffer(bindingPoint, buffID);
    }

    // Binds the buffer to the binding point at index of the array.
    void bindBase(GLenum bindingPoint, int index)
    {
        glBindBufferBase(bindingPoint,index,buffID);
    }
    
    // Templated function to set the data within the buffer.
    template<typename T>
    void setData(const std::vector<T>& newData, GLenum usage=GL_DYNAMIC_DRAW)
    {
        setData(newData.data(), newData.size()*sizeof(T), usage);
    }
    
    // Standard Data setting function.
    void setData(const void* p, size_t size, GLenum usage)
    {
        bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, size, p, usage);
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
};
        

#pragma once
#include "Texture2DArray.h"

class DataTexture2DArray : public Texture2DArray 
{
public:
    DataTexture2DArray(): Texture2DArray(0,0,0)
    {
        GLuint tmp[1];
        glGenTextures(1,tmp);
        this->tex = tmp[0];
    }
       
    DataTexture2DArray(int w, int h, int slices, void* pix ) : Texture2DArray(w,h,slices)
    {
        GLuint tmp[1];
        glGenTextures(1,tmp);
        this->tex = tmp[0];
        this->bind(0);
        glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, GL_RGBA,
            w,h,slices, 0, GL_RGBA, 
            GL_UNSIGNED_BYTE, pix );
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
    
    DataTexture2DArray(int w, int h, int slices, GLenum ifmt, GLenum efmt, GLenum etype) : Texture2DArray(w,h,slices)
    {
        GLuint tmp[1];
        glGenTextures(1,tmp);
        this->tex = tmp[0];
        this->bind(0);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, ifmt, w,h,slices, 0, efmt, etype, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }


    void setData(int w, int h, int slices, void* pix)
    {
        this->bind(0);
        if(w == this->w && h == this->h && slices == this->slices){
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0,0,0,
                w,h,slices, GL_RGBA, GL_UNSIGNED_BYTE, pix);
        } else{
            glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, GL_RGBA,
                w,h,slices, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);
            this->w=w; this->h=h; this->slices=slices;
        }
        
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        this->unbind(0);
    }
};
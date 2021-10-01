#pragma once

#include "utils.h"

//factory function example
class LaserBlast{
public:
        
    //maximum of 10 blasts on screen at once
    static std::vector< std::shared_ptr<Buffer> > buffers;
    static std::vector< GLuint > vaos;
    static std::vector<bool> inUse;
    
    static std::shared_ptr<ImageTexture2DArray> tex;
    static std::shared_ptr<ImageTexture2DArray> etex;
    
    vec3 pos,vel;
    bool dirty;
    std::size_t idx;
    int state;
    int lifeLeft;
    
    static std::shared_ptr<LaserBlast> make(vec3 pos, vec3 vel)
    {
        if( buffers.empty() )
        {
            tex = std::make_shared<ImageTexture2DArray>("black.png");
            etex = std::make_shared<ImageTexture2DArray>("lightblue.png");
            
            for(int i=0;i<10;++i){
                buffers.push_back( std::make_shared<Buffer>(3*2*sizeof(float)) );
                GLuint tmp[1];
                glGenVertexArrays(1,tmp);
                vaos.push_back(tmp[0]);
                glBindVertexArray(vaos.back());
                glEnableVertexAttribArray(0);
                buffers.back()->bind(GL_ARRAY_BUFFER);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, 3*4, 0);
                glBindVertexArray(0);
                inUse.push_back(false);
            }
        }
        for(std::size_t i=0;i<buffers.size();++i)
        {
            if( !inUse[i] )
            {
                inUse[i]=true;
                std::shared_ptr<LaserBlast> bl = std::shared_ptr<LaserBlast>(new LaserBlast(pos,vel,i));
                return bl;
            }
        }
        return nullptr;
    }
    
    ~LaserBlast()
    {
        inUse[idx]=false;
    }
    
    void update(int elapsed)
    {
        lifeLeft -= elapsed;
        if( lifeLeft <= 0 )
            state = DEAD;
        pos = pos + 0.025*vel*elapsed;
        dirty=true;
    }
    
    void makeClean()
    {
        vec3 tmp[2];
        tmp[0] = pos;
        tmp[1] = pos+3.0*vel;
        buffers[idx]->setData(tmp, sizeof(tmp), GL_DYNAMIC_DRAW );
        dirty=false;
    }
    
    void draw()
    {
        if( state == DEAD )
            return;
        if(dirty)
            makeClean();
        glLineWidth(4.0);
        Program::setUniform("diffuse",vec3(0,0,0));
        Program::setUniform("specular",vec3(0,0,0));
        Program::setUniform("worldMatrix",mat4::identity());
        Program::updateUniforms();
        tex->bind(0);
        etex->bind(1);
        glBindVertexArray(vaos[idx]);
        glDrawArrays(GL_LINES,0,2);
    }
        
            
private:
    LaserBlast( vec3 pos, vec3 vel, std::size_t idx)
    {
        this->pos=pos;
        this->vel=vel;
        this->dirty=true;
        this->idx=idx;
        state = ALIVE;
        lifeLeft = 2000;
    }
};
        

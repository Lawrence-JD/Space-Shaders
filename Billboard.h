#pragma once

#include "utils.h"

class BillboardManager 
{
public:
    std::vector<vec3> positions;
    static std::shared_ptr<Program> bbprog;
    bool dirty=false;
    std::shared_ptr<Buffer> pbuff;
    GLuint vao;
    
    BillboardManager()
    {
        if(bbprog == nullptr)
            bbprog = std::make_shared<Program>("billboard.vs","billboard.gs","billboard.fs");

        pbuff = std::make_shared<Buffer>(positions); //dummy
        GLuint tmp[1];
        glGenVertexArrays(1,tmp);
        vao = tmp[0];
        glBindVertexArray(vao);
        pbuff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 3, GL_FLOAT, false, 3*4, 0 );
        glBindVertexArray(0);
        this->vao = vao;
    }
    
    void add(vec3 pos)
    {
        positions.push_back(pos);
        dirty=true;
    }
  
    void swapWithLastBillboard(size_t idx)
    {
        if( idx != positions.size()-1){
            std::swap(positions[idx],positions.back());
            dirty=true;
        }
    }
    
    void removeLastBillboard()
    {
        positions.pop_back();
        dirty=true;
    }
    
    void setBillboardPosition(size_t idx, vec3 v)
    {
        positions[idx] = v;
        dirty=true;
    }
    
    void draw()
    {
        if( positions.empty() )
            return;
            
        if(dirty){
            pbuff->setData(positions);
            dirty=false;
        }
        auto oldprog = Program::current;
        bbprog->use();
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, positions.size() );
        if(oldprog)
            oldprog->use();
    }
};

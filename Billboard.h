#pragma once

#include "utils.h"

/*
* Class for handling billboard draws and positonal data.
*/
class BillboardManager 
{
public:
    // Billboard Positions
    std::vector<vec3> positions;

    // Billboard Program for Billboard shader compiling and use.
    static std::shared_ptr<Program> bbprog;

    // Flag for position updates.
    bool dirty=false;

    // Positions Buffer
    std::shared_ptr<Buffer> pbuff;

    // OpenGL Vertex Array Object
    GLuint vao;
    
    // Base constructor.
    BillboardManager()
    {
        // Compile shaders if they haven't been already.
        if(bbprog == nullptr)
            bbprog = std::make_shared<Program>("billboard.vs","billboard.gs","billboard.fs");

        // Initialize the dummy positions buffer.
        pbuff = std::make_shared<Buffer>(positions);

        // VAO initialization.
        GLuint tmp[1];
        glGenVertexArrays(1,tmp);
        vao = tmp[0];
        glBindVertexArray(vao);

        // Bind the Positions buffer
        pbuff->bind(GL_ARRAY_BUFFER);

        // Set OpenGL attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 3, GL_FLOAT, false, 3*4, 0 );

        // Finalize the VAO
        glBindVertexArray(0);
        this->vao = vao;
    }
    
    // Adds a billboard to the manager.
    void add(vec3 pos)
    {
        positions.push_back(pos);
        dirty=true;
    }

    // Swaps the given billboard with the last billboard.
    void swapWithLastBillboard(size_t idx)
    {
        if( idx != positions.size()-1){
            std::swap(positions[idx],positions.back());
            dirty=true;
        }
    }
    
    // Removes the last billboard.
    void removeLastBillboard()
    {
        positions.pop_back();
        dirty=true;
    }
    
    // Set billboard with index idx to position v.
    void setBillboardPosition(size_t idx, vec3 v)
    {
        positions[idx] = v;
        dirty=true;
    }
    
    // Draws the billboards. Only updates position data if dirty is true.
    void draw()
    {
        if(positions.empty())
            return;
            
        if(dirty)
        {
            pbuff->setData(positions);
            dirty=false;
        }
        auto oldprog = Program::current;
        bbprog->use();
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, positions.size());
        if(oldprog)
            oldprog->use();
    }
};

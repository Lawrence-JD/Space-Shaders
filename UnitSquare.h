#pragma once

#include "ImageTexture2DArray.h"
#include "Buffer.h"
#include "Program.h"
#include "math3d.h"
#include "glfuncs.h"
#include <cassert>

/*
* Class used to define a Unit Square. I use a unit square for drawing to the screen.
* Everything is drawn to the Unit Square, then the Unit Square is drawn to the screen.
*/
class UnitSquare
{
public:
    // The Vertex Array object.
    GLuint vao;

    // The Base Constuctor.
    UnitSquare()
    {
        // Define the unit square data for the vertex positions, texture mapping, normals, and edges.
        std::vector<float> pos( {-1,1,-1,  -1,-1,-1,  1,1,-1,  1,-1,-1 } );
        std::vector<float> tex( {0,1,  0,0,   1,1,  1,0} );
        std::vector<float> nor( {0,0,1, 0,0,1, 0,0,1, 0,0,1} );
        std::vector<std::uint32_t> ind( {0,1,2,  2,1,3} );
        
        // Unit square buffers.
        Buffer pbuff(pos);
        Buffer tbuff(tex);
        Buffer nbuff(nor);
        Buffer ibuff(ind);
        
        // Setup the VAO using these buffers.
        GLuint tmp[1];
        glGenVertexArrays(1,tmp);
        vao = tmp[0];
        glBindVertexArray(vao);
        ibuff.bind(GL_ELEMENT_ARRAY_BUFFER);
        
        pbuff.bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 3, GL_FLOAT, false, 3*4, 0 );
        
        tbuff.bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer( 1, 2, GL_FLOAT, false, 2*4, 0 );
        
        nbuff.bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer( 2, 3, GL_FLOAT, false, 3*4, 0 );

        glBindVertexArray(0);
    }
    
    // Standard draw function.
    void draw()
    {
        glBindVertexArray(this->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, ((char*)0) );
    }
};

#pragma once 
#include <SDL.h>
#include <map>
#include <array>
#include "glfuncs.h"
#include "Program.h"

/*
* Class for handling and rendering text to the screen. This is used for the charge % text.
*/
class Text
{
public:
    // The shader program.
    static std::shared_ptr<Program> prog;

    // The texture image.
    std::shared_ptr<ImageTexture2DArray> tex;
    
    // Map for storing the x, y and adv attributes.
    std::map<char, std::array<int,3> > metrics;

    //max width and height of any character
    int maxw;   
    int maxh;
    
    // The Vertex Array Object
    GLuint vao;

    // The screen position of the text.
    vec2 pos;

    // What the text says.
    std::string txt;

    // Dirty flag for only updating the texture when the text changes.
    bool dirty = false;

    // OpenGL data buffers.
    std::shared_ptr<Buffer> vbuff;
    std::shared_ptr<Buffer> tbuff;
    std::shared_ptr<Buffer> ibuff;
    
    // Base Constructor.
    // Compiles the shaders, reads in the font files, and sets up the VAO.
    Text(std::string fontname, int size)
    {
        if(Text::prog == nullptr)
            Text::prog = std::make_shared< Program >("textvs.txt","textfs.txt");

        tex = std::make_shared<ImageTexture2DArray>(fontname+std::to_string(size)+".png");
        std::ifstream in("assets/"+fontname+std::to_string(size)+".txt");
        std::string s;
        std::getline(in,s); //info line
        in >> s >> maxw;
        in >> s >> maxh;
        while(1)
        {
            int ascii,x,y,w;
            in >> ascii >> x >> y >> w;
            if(in.fail() )
                break;
            y = tex->h-1-y;
            metrics[ascii] = std::array<int,3>({x,y,w});
        }
        
        //placeholders...
        vbuff = std::make_shared< Buffer >(std::vector<float>({0,0}),GL_DYNAMIC_DRAW);
        tbuff = std::make_shared< Buffer >(std::vector<float>({0,0}),GL_DYNAMIC_DRAW);
        ibuff = std::make_shared< Buffer >(std::vector<uint32_t>({0}),GL_DYNAMIC_DRAW);
        GLuint tmp[1];
        glGenVertexArrays(1,tmp);
        Text::vao = tmp[0];
        glBindVertexArray(Text::vao);
        ibuff->bind(GL_ELEMENT_ARRAY_BUFFER);
        vbuff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 2, GL_FLOAT, false, 2*4, 0 );
        tbuff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer( 1, 2, GL_FLOAT, false, 2*4, 0 );
        glBindVertexArray(0);

        this->pos = vec2(0,0);
        this->txt = "";
        this->dirty = false;
    }
    
    // Setter for the text string. Also handles position.
    void setText(vec2 pos, std::string txt)
    {
        this->pos = pos;
        if(txt != this->txt)
        {
            this->txt = txt;
            this->dirty = true;
        }
    }
    
    // Function to update and render the texture. Only called when the text changes.
    void renderTexture()
    {
        std::vector<float> vdata;
        std::vector<float> tdata;
        std::vector<uint32_t> idata;
        int x=int(pos.x);
        for(size_t i=0;i<txt.length();++i)
        {
            int ch = txt[i];
            auto m = metrics[ch];
            int mx=m[0];
            int my=m[1];
            int adv=m[2];
            unsigned I = vdata.size()/2;
            vdata.push_back( x );
            vdata.push_back( pos.y );
            vdata.push_back( x+maxw );
            vdata.push_back( pos.y );
            vdata.push_back( x+maxw );
            vdata.push_back( pos.y+maxh );
            vdata.push_back( x );
            vdata.push_back( pos.y+maxh );
            tdata.push_back( mx );
            tdata.push_back( my-maxh+1 );
            tdata.push_back( mx+maxw );
            tdata.push_back( my-maxh+1 );
            tdata.push_back( mx+maxw );
            tdata.push_back( my+1 );
            tdata.push_back( mx );
            tdata.push_back( my+1 );
            idata.push_back( I );
            idata.push_back( I+1 );
            idata.push_back( I+2 );
            idata.push_back( I );
            idata.push_back( I+2 );
            idata.push_back( I+3 );
            x += adv;
        }
        
        vbuff->bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, vdata.size()*sizeof(vdata[0]),vdata.data(),GL_DYNAMIC_DRAW);
        tbuff->bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, tdata.size()*sizeof(tdata[0]),tdata.data(),GL_DYNAMIC_DRAW);
        ibuff->bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, idata.size()*sizeof(idata[0]),idata.data(),GL_DYNAMIC_DRAW);
        this->dirty=false;
    }
    
    // Standard draw function.
    void draw()
    {
        if (this->dirty)
            this->renderTexture();
        auto oldprog = Program::current;
        Text::prog->use();
        glBindVertexArray(Text::vao);
        Program::updateUniforms();
        this->tex->bind(0);
        glDrawElements(GL_TRIANGLES,txt.length()*6,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
        if(oldprog)
            oldprog->use();
    }
};


#pragma once


class Stars
{
public:
    
    int numStars;
    GLuint vao;
    
    Stars(int num)
    {
        numStars = num;
        std::vector<float> starPoints;
        for(int i=0;i<3*numStars;++i)
            starPoints.push_back(uniform(-10,10));
        auto buff = std::make_unique<Buffer>( starPoints );
        GLuint tmp[1];
        glGenVertexArrays(1,tmp);
        vao = tmp[0];
        glBindVertexArray(vao);
        buff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 3, GL_FLOAT, false, 3*4, 0 );
        glBindVertexArray(0);
    }
    
    void draw()
    {
        Program::updateUniforms();
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS,0,numStars);
    }
};

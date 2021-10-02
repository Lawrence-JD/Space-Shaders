#pragma once

#include "ImageTexture2DArray.h"
#include "Buffer.h"
#include "Program.h"
#include "math3d.h"
#include "glfuncs.h"
#include <cassert>

/*
* Class for handling various meshes in game.
*/
class Mesh
{
public:

    // Embedded material class to handle mesh texturing, bump maps, and emissives.
    class Material
    {
    public:
        // Material name.
        std::string name;
        // Difffuse color.
        vec3 diffuse;
        // Specualr color.
        vec3 specular;
        // Shineness multiplier.
        float shininess;
        // The textures.
        std::shared_ptr<Texture> tex;
        std::shared_ptr<Texture> etex;
        std::shared_ptr<Texture> btex;
        int start;  //in bytes
        int count;  //in indices
    };
    
    // Map of all the mesh's materials. Keyed by name.
    std::map<std::string,Material> materialDict;
    // Vector of the mesh's materials.
    std::vector<Material> materials;
    // Mesh Vertex Array Object.
    GLuint vao;
    
    // Function for reading mesh files.
    std::string readLine(std::ifstream& in)
    {
        //file is open in binary mode,
        //so we don't know a line ends in \r\n
        std::string rv;
        while(true)
        {
            char c;
            in.read(&c,1);
            if( in.fail() )
                return rv;
            if( c == '\r' )
                ;
            else if( c == '\n' )
                return rv;
            else
                rv += c;
        }
    }
    
    // Base constructor. Opens the mesh files and calls readline() to read in the data.
    Mesh(std::string fname)
    {
        
        std::ifstream in("assets/"+fname+".mesh",std::ios::binary);
        if( !in.good() )
            throw std::runtime_error("Cannot open assets/"+fname);

        std::string line;
        
        line = readLine(in);
        if( line != "mesh 3" )
            throw std::runtime_error("Unexpected mesh format: "+fname);

        readLine(in);   //discard bbox
        line = readLine(in);
        std::istringstream iss(line);
        std::string junk;
        
        int numMaterials;
        iss >> junk >> numMaterials;
        if( junk != "materials" )
            throw std::runtime_error("Unexpected mesh contents");

        for(int i=0;i<numMaterials;++i)
        {
            std::string mname;
            Material mtl;
            line = readLine(in);
            iss.str(line); 
            iss.seekg(0);
            iss >> junk;
            assert(junk == "material");
            iss >> mname;
            line = readLine(in);
            iss.str(line);
            iss.seekg(0);
            iss >> junk;
            assert(junk == "diffuse");
            iss >> mtl.diffuse.x >> mtl.diffuse.y >> mtl.diffuse.z;
            line = readLine(in);
            iss.str(line);
            iss.seekg(0);
            iss >> junk;
            assert( junk == "specular");
            iss >> mtl.specular.x >> mtl.specular.y >> mtl.specular.z;
            line = readLine(in);
            iss.str(line);
            iss.seekg(0);
            iss >> junk;
            assert( junk == "shininess");
            iss >> mtl.shininess;
            line = readLine(in);
            iss.str(line);
            iss.seekg(0);
            iss >> junk;
            assert( junk == "texfile");
            iss >> std::ws;
            std::getline(iss,junk);
            mtl.tex = std::make_shared<ImageTexture2DArray>(junk);
            line = readLine(in);
            iss.str(line);
            iss.seekg(0);
            iss >> junk;
            assert( junk == "etexfile");
            iss >> std::ws;
            std::getline(iss,junk);
            mtl.etex = std::make_shared<ImageTexture2DArray>(junk);
 
            line = readLine(in);
            iss.str(line);
            iss.seekg(0);
            iss >> junk;
            assert( junk == "btexfile");
            iss >> std::ws;
            std::getline(iss,junk);
            mtl.btex = std::make_shared<ImageTexture2DArray>(junk);

           line = readLine(in);
            iss.str(line);
            iss.seekg(0);
            iss >> junk;
            assert( junk == "range");
            iss >> mtl.start >> mtl.count;  //start=bytes, count=ints
            materials.push_back(mtl);
            materialDict[mname]=mtl;
        }

        line = readLine(in);
        iss.str(line);
        iss.seekg(0);
        iss >> junk;
        assert(junk == "positions");
        int num;
        iss >> num;
        std::vector<float> posdata(num/sizeof(float));
        in.read( (char*) posdata.data(), num);
        readLine(in);   //skip trailing newline
        
        line = readLine(in);
        iss.str(line);
        iss.seekg(0);
        iss >> junk;
        assert(junk == "texCoords");
        iss >> num;
        std::vector<float> texdata(num/sizeof(float));
        in.read( (char*) texdata.data(), num);
        readLine(in);   //skip trailing newline
        
        line = readLine(in);
        iss.str(line);
        iss.seekg(0);
        iss >> junk;
        assert(junk == "normals");
        iss >> num;
        std::vector<float> normdata(num/sizeof(float));
        in.read( (char*) normdata.data(), num);
        assert(!in.fail());
        line = readLine(in);   //skip trailing newline
        assert(line == "" );
        
        line = readLine(in);
        iss.str(line);
        iss.seekg(0);
        iss >> junk;
        assert(junk == "tangents");
        iss >> num;
        std::vector<float> tangdata(num/sizeof(float));
        in.read( (char*) tangdata.data(), num);
        assert(!in.fail());
        line = readLine(in);   //skip trailing newline
        assert(line == "" );
        
        line = readLine(in);
        iss.str(line);
        iss.seekg(0);
        iss >> junk;
        assert(junk == "indices");
        iss >> num;
        std::vector<std::uint32_t> inddata(num/sizeof(std::uint32_t));
        in.read( (char*) inddata.data(), num);
        readLine(in);   //skip trailing newline
        
        assert( readLine(in) == "end" );
        
        std::shared_ptr<Buffer> pbuff = std::make_shared< Buffer >( posdata  );
        std::shared_ptr<Buffer> nbuff = std::make_shared< Buffer >( normdata  );
        std::shared_ptr<Buffer> tbuff = std::make_shared< Buffer >( texdata  );
        std::shared_ptr<Buffer> tgbuff = std::make_shared< Buffer >( tangdata  );
        std::shared_ptr<Buffer> ibuff = std::make_shared< Buffer >( inddata  );
        
        GLuint tmp[1];
        glGenVertexArrays(1,tmp);
        vao = tmp[0];
        glBindVertexArray(vao);
        ibuff->bind(GL_ELEMENT_ARRAY_BUFFER);
        
        pbuff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 3, GL_FLOAT, false, 3*4, 0 );
        
        tbuff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer( 1, 2, GL_FLOAT, false, 2*4, 0 );
        
        nbuff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer( 2, 3, GL_FLOAT, false, 3*4, 0 );

        tgbuff->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer( 3, 3, GL_FLOAT, false, 3*4, 0 );

        glBindVertexArray(0);
        this->vao = vao;
    }
    
    // Standard draw function.
    void draw()
    {
        glBindVertexArray(this->vao);
        for(auto& m : materials ){
            Program::setUniform("diffuse",m.diffuse);
            Program::setUniform("specular",m.specular);
            Program::setUniform("shininess",m.shininess);
            Program::updateUniforms();
            m.tex->bind(0);
            m.etex->bind(1);
            m.btex->bind(2);
            glDrawElements(GL_TRIANGLES, m.count, GL_UNSIGNED_INT, ((char*)0) + m.start );
        }
    }
};

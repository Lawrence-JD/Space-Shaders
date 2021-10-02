#pragma once


#include "utils.h"
#include "Buffer.h"

/*
* Class for compiling and activating the game's shaders.
*/
class Program
{
public:
    // Map of uniforms. These are sent from the CPU to the GPU in the UBO.
    static std::map<std::string,std::array<int,4> >uniforms;

    // The current active shader Program.
    static Program* current;

    // The Uniform Buffer Object. This holds the uniform data that is sent to the GPU.
    static std::shared_ptr<Buffer> uboBuffer;

    // The Uniform Buffer Object backing memory.
    static std::vector<char> uboBackingMemory;

    // The GL program.
    GLuint prog;

    // Empty Base Constructor. Used for programs that don't have a geometry shader.
    Program(std::string vsfname,  std::string fsfname) : Program(vsfname,"",fsfname) 
    {

    }
    
    // Constructor used for all non-Compute Shader Program objects.
    Program(std::string vsfname, std::string gsfname, std::string fsfname)
    {
        // Compile the vertex shader.
        GLuint vs = this->compile(vsfname, GL_VERTEX_SHADER);
        // If the geometry shader file name was passed in, compile the geometry shader.
        GLuint gs=0;
        if( !gsfname.empty() )
            gs = this->compile(gsfname, GL_GEOMETRY_SHADER );
        // Compile the fragment shader.
        GLuint fs = this->compile(fsfname, GL_FRAGMENT_SHADER);
        // Call the OpenGL API to create and set the shader program.
        this->prog = glCreateProgram();
        // Attach the vertex shader to the OpenGL program.
        glAttachShader(this->prog, vs);
        // Attach the geometry shader to the OpenGL program, again if we were given a geometry shader file name.
        if( !gsfname.empty() )
            glAttachShader(this->prog, gs);
        // Attach the fragment shader to the OpenGL program.
        glAttachShader(this->prog, fs);
        // Link the program now that all the shaders are attached.
        glLinkProgram(this->prog);
        // Call getLog() for debugging.
        this->getLog("Linking "+vsfname+" "+gsfname+" and "+fsfname,glGetProgramInfoLog, this->prog);
        // If there was an error, throw and exception.
        GLint tmp[1];
        glGetProgramiv( this->prog, GL_LINK_STATUS, tmp );
        if(! tmp[0] )
            throw std::runtime_error("Cannot link "+vsfname+" and "+fsfname);
        // If the UBO doesn't exist, create it.
        if( !Program::uboBuffer )
            Program::setupUniforms(this->prog);
    }

    // Constructor used for Compute shader Program objects.
	Program(std::string csfname) 
    {
        // Compile the compute shader.
		auto cs = this->compile(csfname, GL_COMPUTE_SHADER);
        // Call the API the create the OpenGL program.
		this->prog = glCreateProgram();
        // Attach the shader, link the program.
		glAttachShader(this->prog, cs);
		glLinkProgram(this->prog);
        // Call getLog() for debugging.
		this->getLog("Linking ", glGetProgramInfoLog, this->prog);
        // If there was an error, throw an exception.
		GLint tmp[1];
		glGetProgramiv(this->prog, GL_LINK_STATUS, tmp);
		if (!tmp[0])
			throw std::runtime_error("Cannot link");
        // if the UBO doesn't exist, create it.
		if (!Program::uboBuffer)
			Program::setupUniforms(this->prog);
	}

    // Function called to dispatch, or run, the compute shader. The passed in dimensions determine the number of Work Groups.
	void dispatch(int xs, int ys, int zs) 
    {
		if (current != this)
			this->use();
		glDispatchCompute(xs, ys, zs);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
    
    // Function used to setup the Uniforms. This is used to ensure that the UBO is of the correct size.
    static void setupUniforms(GLuint prog)
    {
        // Get the program's number of active uniforms.
        GLint tmp[1];
        glGetProgramiv(prog,GL_ACTIVE_UNIFORMS,tmp);
        GLint numuniforms = tmp[0];
        // Setup a list of uniforms, these will be examined for their data size.
        std::vector<GLuint> uniformsToQuery(numuniforms);
        for(GLint i=0;i<numuniforms;++i)
            uniformsToQuery[i]=i;
        // Setup lists to keep track of the uniforms' memory offsets, sizes, and types
        std::vector<GLint> offsets(numuniforms);
        std::vector<GLint> sizes(numuniforms);
        std::vector<GLint> types(numuniforms);
        // Populate those lists.
        glGetActiveUniformsiv(prog, numuniforms,
            uniformsToQuery.data(), GL_UNIFORM_OFFSET, offsets.data());
        glGetActiveUniformsiv(prog, numuniforms,
            uniformsToQuery.data(), GL_UNIFORM_SIZE, sizes.data());
        glGetActiveUniformsiv(prog, numuniforms,
            uniformsToQuery.data(), GL_UNIFORM_TYPE, types.data());

        // Setup a map for easy type-to-data-size conversion.
        std::map<GLenum, int> sizeForType( {
            {GL_FLOAT_VEC4, 4*4},
            {GL_FLOAT_VEC3, 3*4},
            {GL_FLOAT_VEC2, 2*4},
            {GL_FLOAT,      1*4},
            {GL_INT,        1*4},
            {GL_FLOAT_MAT2, 2*16},
            {GL_FLOAT_MAT3, 3*16},
            {GL_FLOAT_MAT4, 4*16}
        });

        // Vector for storing Uniform names.
        std::vector<char> nameBytes(256);
        // Int value to keep track of the total number of bytes used by the uniform.
        int totalUniformBytes = 0;
        // Loop through each uniform and tally its total byte size.
        for(int i=0;i<numuniforms;++i)
        {
            glGetActiveUniformName(prog, i,nameBytes.size(),
                tmp, nameBytes.data());
            auto nameLen = tmp[0];
            std::string name(nameBytes.data(),nameLen);
            if(offsets[i] != GLint(-1) )
            {
                assert(sizes[i] == 1 || types[i] == GL_FLOAT_VEC4);
                int numBytes = sizeForType[types[i]] * sizes[i];
                Program::uniforms[name] = {offsets[i], numBytes, types[i], sizes[i] };
                int end = offsets[i] + numBytes;
                if(end > totalUniformBytes)
                    totalUniformBytes = end;
            }
        }
        // resize the UBO to properly fit the uniforms.
        uboBackingMemory.resize(totalUniformBytes);
        Program::uboBuffer = std::make_shared< Buffer >(uboBackingMemory, GL_DYNAMIC_DRAW );
        Program::uboBuffer->bindBase(GL_UNIFORM_BUFFER,0);
    }
    
    // The following are various setUniform() wrappers. Each is used to set the data of a given Uniform.
    // Each override uses a different data type and calls the actual setUniform function.
    static void setUniform(std::string name, float value )
    {
        setUniform(name,&value,sizeof(value), GL_FLOAT, 1);
    }
    static void setUniform(std::string name, int value )
    {
        setUniform(name,&value,sizeof(value), GL_INT, 1);
    }
    static void setUniform(std::string name, unsigned value )
    {
        setUniform(name,&value,sizeof(value), GL_INT, 1);
    }
    static void setUniform(std::string name, vec2 value )
    {
        setUniform(name,&value,sizeof(value), GL_FLOAT_VEC2, 1);
    }
    static void setUniform(std::string name, vec3 value )
    {
        setUniform(name,&value,sizeof(value), GL_FLOAT_VEC3, 1);
    }
    static void setUniform(std::string name, vec4 value )
    {
        setUniform(name,&value,sizeof(value), GL_FLOAT_VEC4, 1);
    }
    static void setUniform(std::string name, mat4 value )
    {
        setUniform(name,&value,sizeof(value), GL_FLOAT_MAT4, 1);
    }
    static void setUniform(std::string name, mat3 value )
    {
        setUniform(name,&value,sizeof(value), GL_FLOAT_MAT3, 1);
    }
    static void setUniform(std::string name, const std::vector<vec4>& value )
    {
        setUniform(name,value.data(),value.size()*sizeof(value[0]), GL_FLOAT_VEC4, value.size());
    }
    // End setUniform() Overrides.

    // Helper function used to quickly get OpenGL type names.
    static std::string getTypeName(int t)
    {
        std::map<GLenum, std::string> M( {
            {GL_FLOAT_VEC4, "GL_FLOAT_VEC4"},
            {GL_FLOAT_VEC3, "GL_FLOAT_VEC3"},
            {GL_FLOAT_VEC2, "GL_FLOAT_VEC2"},
            {GL_FLOAT,      "GL_FLOAT"},
            {GL_INT,        "GL_INT"},
            {GL_FLOAT_MAT2, "GL_FLOAT_MAT2"},
            {GL_FLOAT_MAT3, "GL_FLOAT_MAT3"},
            {GL_FLOAT_MAT4, "GL_FLOAT_MAT4"}
        });
        if( M.find(t) != M.end() )
            return M[t];
        return "<unknown type>";
    }
    
    // The actual setUniform() function. 
    // This function is called by the wrappers and is used to set the data within the given Uniform.
    static void setUniform(std::string name, const void* p, size_t sizeAsSizeT, GLenum typeAsEnum, size_t numElements )
    {
         // If the given uniform does not exist, throw an exception.                       
        if( uniforms.find(name) == uniforms.end() )
            throw std::runtime_error("No such uniform "+name);

        // Setup all the memory data we will need basedo n the given parameters.
        auto A = uniforms[name];
        int _offs = A[0];
        int _numBytes = A[1];
        int _type = A[2];
        int _size = A[3];
        int bytesize = (int)sizeAsSizeT;
        int type = (int)typeAsEnum;

        // If the given type or byte size don't match, throw an exception.
        if( _type != type || _numBytes != bytesize )
        {
            std::cout << "Error when setting uniform " << name << "\n";
            std::cout << "Expected " << _numBytes << " bytes; got " << bytesize << "\n";
            std::cout << "Expected type " << getTypeName(_type) << "; got " << getTypeName(type) << "\n";
            std::cout << "Expected array size " << _size << "; got " << numElements << "\n";
            throw std::runtime_error("Mismatch when setting uniform '"+name+"'");
        }
        
        // Move the data into the UBO.
        void* dst = uboBackingMemory.data() + _offs;
        memmove( dst, p, bytesize );
    }
    
    // Wrapper function for registering UBO changes with OpenGL.
    static void updateUniforms()
    {
        glBufferSubData(GL_UNIFORM_BUFFER, 0,
            Program::uboBackingMemory.size(),
            Program::uboBackingMemory.data());
    }

    // Wrapper function to set this program to the current active program.
    // Meaning this program's shaders will now be used when drawing.
    void use()
    {
        glUseProgram(this->prog);
        Program::current = this;
    }
    
    // Shader compiler function.
    GLuint compile(std::string fname, GLenum shaderType)
    {
        // Create a shader of the given type.
        auto s = glCreateShader(shaderType);
        
        // Open and read the given shader file.
        std::string shaderdata;
        std::ifstream in("shaders/"+fname);
        if(!in.good() )
            throw std::runtime_error("Cannot open "+fname);
        std::getline(in,shaderdata,'\0');
        
        // Open and read the uniforms definitions file.
        std::string uniformdata;
        std::ifstream in2("shaders/uniforms.txt");
        if( !in2.good() )
            throw std::runtime_error("Cannot open shaders/uniforms.txt");
            
        std::getline(in2,uniformdata,'\0');

        std::string txt = "#version 430\n"
                        "layout( std140, row_major ) uniform Uniforms {\n"
                        "#line 1\n" +
                        uniformdata +
                        "};\n"
                        "#line 1\n" +
                        shaderdata;
        
        // Set the shader source in OpenGL.
        const char* X[1];
        X[0] = txt.c_str();
        glShaderSource(s, 1, X, nullptr );
        // Call the API to compile the shader.
        glCompileShader(s);
        // Get the compiler log for debugging purposes.
        this->getLog("Compiling "+fname, glGetShaderInfoLog, s);
        // If there was a compiler error, throw an exception.
        GLint tmp[1];
        glGetShaderiv( s, GL_COMPILE_STATUS, tmp );
        if(! tmp[0] )
            throw std::runtime_error("Cannot compile "+fname);
        return s;
    }
    
    // Templated getLog() function used for debugging purposes.
    template<typename T>
    void getLog( std::string description, const T& func, GLuint identifier )
    {
        std::vector<char> blob(4096);
        GLint tmp[1];
        func( identifier, blob.size(), tmp, blob.data());
        auto length = tmp[0];
        if(length > 0){
            std::string str(blob.data(),length);
            std::cout << description << ":\n";
            std::cout << str << "\n";
        }
    }
};
             

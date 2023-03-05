#include "Shader.h"

#include <fstream>

#include "SDL.h"

namespace {
const GLchar PRECISION_NAME[][8] = { "lowp", "mediump", "highp" };
}

Graphics::SDL2::GLES2::Internal::Shader::Type::Type( PRECISION param_precision, std::basic_string<GLchar> variable_entry ) : precision( param_precision ), variable( variable_entry )
{}

Graphics::SDL2::GLES2::Internal::Shader::Attribute::Attribute( PRECISION param_precision, std::basic_string<GLchar> variable_entry ) : Type( param_precision, variable_entry )
{}

std::basic_string<GLchar> Graphics::SDL2::GLES2::Internal::Shader::Attribute::getEntryES2() const {
    std::basic_string<GLchar> name = "attribute ";
    name += PRECISION_NAME[ precision ];
    name += " ";
    name += variable;
    name += ";\n";
    
    if( precision == HIGH ) {
        std::basic_string<GLchar> guard;
        
        guard += "#ifdef GL_FRAGMENT_PRECISION_HIGH\n";
        guard += "  ";
        guard += name;
        guard += "#else\n";
        guard += "  ";
        guard += Attribute( MEDIUM, variable ).getEntryES2();
        guard += "#endif\n";
        
        return guard;
    }
    else
        return name;
}

std::basic_string<GLchar> Graphics::SDL2::GLES2::Internal::Shader::Attribute::getEntry2() const {
    std::basic_string<GLchar> name = "attribute ";
    name += variable;
    name += ";\n";
    
    return name;
}

std::basic_string<GLchar> Graphics::SDL2::GLES2::Internal::Shader::Varying::getEntryES2() const {
    std::basic_string<GLchar> name = "varying ";
    name += PRECISION_NAME[ precision ];
    name += " ";
    name += variable;
    name += ";\n";
    
    if( precision == HIGH ) {
        std::basic_string<GLchar> guard;
        
        guard += "#ifdef GL_FRAGMENT_PRECISION_HIGH\n";
        guard += "  ";
        guard += name;
        guard += "#else\n";
        guard += "  ";
        guard += Varying( MEDIUM, variable ).getEntryES2();
        guard += "#endif\n";
        
        return guard;
    }
    else
        return name; // Simply return the unmodified variable.
}

std::basic_string<GLchar> Graphics::SDL2::GLES2::Internal::Shader::Varying::getEntry2() const {
    std::basic_string<GLchar> name = "varying ";
    name += variable;
    name += ";\n";
    
    return name;
}

Graphics::SDL2::GLES2::Internal::Shader::Varying::Varying( PRECISION param_precision, std::basic_string<GLchar> variable_entry ) : Type( param_precision, variable_entry )
{}

Graphics::SDL2::GLES2::Internal::Shader::Shader() : shader_type( EMPTY ), shader_id( 0 ) {
}

Graphics::SDL2::GLES2::Internal::Shader::Shader( TYPE type, const GLchar *const shader_source, std::vector<Attribute> attributes_param, std::vector<Varying> varyings_param ) :
        shader_type( EMPTY ), // Set these values to the defaults just in case.
        shader_id( 0 )
{
    setShader( type, shader_source, attributes_param, varyings_param );
}

Graphics::SDL2::GLES2::Internal::Shader::~Shader() {
    // When the shader is being deleted set the shader itself to be deallocated.
    deallocate();
}

void Graphics::SDL2::GLES2::Internal::Shader::setShader( TYPE type, const GLchar *const primary_shader_source, std::vector<Attribute> attributes, std::vector<Varying> varyings ) {
    int opengl_profile;
    
    GLchar glsl_es2_version[] = "#version 100\nprecision mediump float;\n";
    GLchar glsl_2_version[] = "#version 110\n";
    
    this->generated_shader = "";
    
    if( type != EMPTY )
    {
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &opengl_profile );
        
        this->attributes = attributes;
        this->varyings = varyings;
        
        // Deallocate the previous shader.
        deallocate();

        // Set the shader type.
        shader_type = type;

        // Create and compile the shader.
        shader_id = glCreateShader( type );
        
        if( (opengl_profile & SDL_GL_CONTEXT_PROFILE_ES) != 0 ) {
            this->generated_shader += glsl_es2_version;
            for( auto i = attributes.begin(); i != attributes.end(); i++ ) {
                this->generated_shader += (*i).getEntryES2();
            }
            for( auto i = varyings.begin(); i != varyings.end(); i++ ) {
                this->generated_shader += (*i).getEntryES2();
            }
        }
        else {
            this->generated_shader += glsl_2_version;
            for( auto i = attributes.begin(); i != attributes.end(); i++ ) {
                this->generated_shader += (*i).getEntry2();
            }
            for( auto i = varyings.begin(); i != varyings.end(); i++ ) {
                this->generated_shader += (*i).getEntry2();
            }
        }
        
        this->generated_shader += primary_shader_source;
        
        address_generated_shader_r = generated_shader.c_str();
        
        glShaderSource( shader_id, 1, &address_generated_shader_r, nullptr );
        glCompileShader( shader_id );
    }
}

int Graphics::SDL2::GLES2::Internal::Shader::loadShader( TYPE type, const char *const file_path, std::vector<Attribute> attributes, std::vector<Varying> varyings ) {
    std::ifstream shader_file;
    shader_file.open( file_path, std::ios::out );

    if( shader_file )
    {
        // First we need the file size of the file.
        shader_file.seekg( 0, shader_file.end );
        const size_t FILE_SIZE = shader_file.tellg();
        shader_file.seekg( 0, shader_file.beg );

        // Then allocate the shader_buffer which will contain the shader code.
        GLchar *shader_buffer = new GLchar [ FILE_SIZE ];

        // Read the shader file's data and put it into the buffer.
        shader_file.read( shader_buffer, FILE_SIZE );

        // Set this shader with the shader buffer.
        setShader( type, shader_buffer, attributes, varyings );

        // DO NOT FORGET TO DELETE THE BUFFFER WHEN DONE.
        delete [] shader_buffer;

        // Return 1 for success.
        return 1;
    }
    else
        return 0; // The file is not found.
}

void Graphics::SDL2::GLES2::Internal::Shader::deallocate() {
    if( shader_type != EMPTY )
        glDeleteShader( shader_id );

    // Reset the Shader's state to empty.
    shader_type = EMPTY;
    shader_id = 0;
}
std::string Graphics::SDL2::GLES2::Internal::Shader::getInfoLog() const {
    std::string returnry = this->generated_shader;
    GLchar *temporary_string_p;
    GLint info_length;
    GLsizei actual_info_length;
    
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_length);
    
    if( info_length > 0 )
    {
        temporary_string_p = new GLchar [ info_length ];
        
        if( temporary_string_p != nullptr )
        {
            returnry.reserve( info_length );
            
            glGetShaderInfoLog(shader_id, info_length, &actual_info_length, temporary_string_p );
            
            for( GLsizei a = 0; a < actual_info_length; a++ ) {
                returnry.push_back( temporary_string_p[ a ] );
            }
            
            delete [] temporary_string_p;
        }
        else
            returnry += "shader's getInfoLog has ran out of memory?";
    }
    else
        returnry += "This shader has no info log.";
    
    return returnry;
}

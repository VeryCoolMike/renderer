

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Taken from learnOpenGL, thanks ( will probably edit later maybe )

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char *vertexPath, const char *fragmentPath)
    {
        // Step 1: Get the code from the files
        std::string vertexCode; // Create temporary strings for the code
        std::string fragmentCode;
        std::ifstream vShaderFile; // Create an ifstream to read the code
        std::ifstream fShaderFile;
        // Make ifstream throw exceptions to be caught later
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // Open the files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // Copy the file buffer into the streams
            vShaderStream << vShaderFile.rdbuf(); // rdbuf returns a pointer to the buffer
            fShaderStream << fShaderFile.rdbuf();
            // Close the files
            vShaderFile.close();
            fShaderFile.close();
            // Convert the streams into strings
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure &e) // Incase of the exceptions cause error
        {
            std::cerr << error("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: ") << e.what() << std::endl;
        }
        // Convert the code to a const char*
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        // Step 2: Compile the shaders
        unsigned int vertex, fragment;
        // Create and compile the vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // Create and compile the fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // Create the shader program and link them to the shaders
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // Delete the shaders since they're no longer necessary and are linked to the program now
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // OpenGL works in a state so a function to make the shader the current one
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // Functions to easily set uniforms
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat3(const std::string &name, float value1, float value2, float value3) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
    }

    void setMatrix4fv(const std::string &name, GLsizei value1, GLboolean value2, const GLfloat *value3)
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
    }

private:
    // Function to check for errors in compilation
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        // Create error buffer
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") // Check the type of error (program is the shader program)
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << error("ERROR::SHADER_COMPILATION_ERROR of type: ") << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else // Check for fragment and vertex shader issues
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << error("ERROR::PROGRAM_LINKING_ERROR of type: ") << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif

#include "pch.hpp"
#include "Shader.hpp"
#include <glad/glad.h>

namespace Pit::Rendering {

    Shader::Shader(const String& filepath) {
        PIT_PROFILE_FUNCTION();

        ScopedTimer t(filepath + " shader compiling");

        std::string shaderFileContents;
        std::ifstream shaderFile(filepath, std::ios::in | std::ios::binary);
        if (shaderFile) {
            shaderFile.seekg(0, std::ios::end);
            size size = shaderFile.tellg();
            if (size != -1) {
                shaderFileContents.resize(size);
                shaderFile.seekg(0, std::ios::beg);
                shaderFile.read(&shaderFileContents[0], size);
            }
            else PIT_ENGINE_FATAL(Rendering, "Could not open shaderFile {}", filepath);
        }
        
        String vertexCode, fragmentCode;
        
        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = shaderFileContents.find(typeToken, 0); //Start of shader type declaration line
        while (pos != std::string::npos) {
            size_t eol = shaderFileContents.find_first_of("\r\n", pos); //End of shader type declaration line
            PIT_ENGINE_ASSERT(Rendering, eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
            std::string type = shaderFileContents.substr(begin, eol - begin);

            size_t nextLinePos = shaderFileContents.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
            PIT_ENGINE_ASSERT(Rendering, nextLinePos != std::string::npos, "Syntax error");
            pos = shaderFileContents.find(typeToken, nextLinePos); //Start of next shader type declaration line

            if (type == "vertex") vertexCode = (pos == std::string::npos) ? shaderFileContents.substr(nextLinePos) : shaderFileContents.substr(nextLinePos, pos - nextLinePos);
            else if (type == "fragment") fragmentCode = (pos == std::string::npos) ? shaderFileContents.substr(nextLinePos) : shaderFileContents.substr(nextLinePos, pos - nextLinePos);
            else PIT_ENGINE_ERR(Rendering, "Unkown shader type {}", type);
        }
       
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;

        // VertexShader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        _CheckCompileErrors(vertex, "Vertex");

        // FragmentShader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        _CheckCompileErrors(fragment, "Fragment");

        // ShaderProgram
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertex);
        glAttachShader(m_ID, fragment);
        glLinkProgram(m_ID);
        _CheckCompileErrors(m_ID, "Program");


        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Shader::Use() {
        glUseProgram(m_ID);
    }

    void Shader::SetBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
    }

    void Shader::SetInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void Shader::SetFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }

    void Shader::SetVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }

    void Shader::SetVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z);
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }

    void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w);
    }

    void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    void Shader::_CheckCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type == "Program") {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                PIT_ENGINE_ERR(Rendering, "Error while linking the shader program:\n{}\n -- --------------------------------------------------- -- ", infoLog);
            }
        }
        else {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                PIT_ENGINE_ERR(Rendering, "Error while compiling {0}Shader:\n{1}\n -- --------------------------------------------------- -- ", type, infoLog);
            }
        }
    }
}
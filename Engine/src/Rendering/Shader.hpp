#pragma once

#include "Core/CoreInclude.hpp"
#include <glm/glm.hpp>

namespace Pit::Rendering {
    /// <summary>
    /// Manager that handles the compilation, parameterization and activation of
    /// any shader int the game
    /// </summary>
    class Shader {
    public:
        Shader(const String& vertexPath, const String& fragmentPath);

        unsigned int GetID() { return m_ID; }

        void Use();
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2(const std::string& name, const glm::vec2& value) const;
        void SetVec2(const std::string& name, float x, float y) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec3(const std::string& name, float x, float y, float z) const;
        void SetVec4(const std::string& name, const glm::vec4& value) const;
        void SetVec4(const std::string& name, float x, float y, float z, float w) const;
        void SetMat2(const std::string& name, const glm::mat2& mat) const;
        void SetMat3(const std::string& name, const glm::mat3& mat) const;
        void SetMat4(const std::string& name, const glm::mat4& mat) const;

    private:
        unsigned int m_ID;

        void _CheckCompileErrors(unsigned int shader, std::string type);
    };
}
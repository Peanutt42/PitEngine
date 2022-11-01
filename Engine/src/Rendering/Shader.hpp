#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::Rendering {
    class Shader {
    public:
        Shader(const String& vertexPath, const String& fragmentPath);

        unsigned int GetID() { return m_ID; }

        void Use();
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
    private:
        unsigned int m_ID;

        void _CheckCompileErrors(unsigned int shader, std::string type);
    };
}
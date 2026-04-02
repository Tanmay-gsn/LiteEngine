#pragma once
#include <string>
#include <unordered_map>
#include "LiteEngine/Renderer/Shader.h"
#include "LiteEngine/Renderer/Texture.h"

namespace LiteEngine {

    class ResourceManager {
    public:
        static std::unordered_map<std::string, Ref<Shader>>   Shaders;
        static std::unordered_map<std::string, Ref<Texture2D>> Textures;

        static Ref<Shader>   LoadShader(const std::string& filepath, const std::string& name);
        static Ref<Shader>   GetShader(const std::string& name);

        static Ref<Texture2D> LoadTexture(const std::string& filepath, const std::string& name);
        static Ref<Texture2D> GetTexture(const std::string& name);

        static void Clear();

    private:
        ResourceManager() {}
    };

}
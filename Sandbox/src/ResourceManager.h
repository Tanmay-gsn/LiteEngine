#pragma once
#include <string>
#include <unordered_map>
#include "LiteEngine/Renderer/Shader.h"
#include "LiteEngine/Renderer/Texture.h"

class ResourceManager {
public:
    static std::unordered_map<std::string, LiteEngine::Ref<LiteEngine::Shader>>    Shaders;
    static std::unordered_map<std::string, LiteEngine::Ref<LiteEngine::Texture2D>> Textures;

    static LiteEngine::Ref<LiteEngine::Shader>   LoadShader(const std::string& filepath, const std::string& name);
    static LiteEngine::Ref<LiteEngine::Shader>   GetShader(const std::string& name);

    static LiteEngine::Ref<LiteEngine::Texture2D> LoadTexture(const std::string& filepath, const std::string& name);
    static LiteEngine::Ref<LiteEngine::Texture2D> GetTexture(const std::string& name);

    static void Clear();

private:
    ResourceManager() {}
};
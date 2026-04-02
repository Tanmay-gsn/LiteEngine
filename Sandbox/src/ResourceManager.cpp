#include "hzpch.h"
#include "ResourceManager.h"

namespace LiteEngine {

    std::unordered_map<std::string, Ref<Shader>>    ResourceManager::Shaders;
    std::unordered_map<std::string, Ref<Texture2D>> ResourceManager::Textures;

    Ref<Shader> ResourceManager::LoadShader(const std::string& filepath, const std::string& name)
    {
        Shaders[name] = Shader::Create(filepath);
        return Shaders[name];
    }

    Ref<Shader> ResourceManager::GetShader(const std::string& name)
    {
        return Shaders.at(name);
    }

    Ref<Texture2D> ResourceManager::LoadTexture(const std::string& filepath, const std::string& name)
    {
        Textures[name] = Texture2D::Create(filepath);
        return Textures[name];
    }

    Ref<Texture2D> ResourceManager::GetTexture(const std::string& name)
    {
        return Textures.at(name);
    }

    void ResourceManager::Clear()
    {
        Shaders.clear();
        Textures.clear();
    }

}
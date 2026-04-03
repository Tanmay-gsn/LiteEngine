#include "hzpch.h"
#include "ResourceManager.h"

std::unordered_map<std::string, LiteEngine::Ref<LiteEngine::Shader>>    ResourceManager::Shaders;
std::unordered_map<std::string, LiteEngine::Ref<LiteEngine::Texture2D>> ResourceManager::Textures;

LiteEngine::Ref<LiteEngine::Shader> ResourceManager::LoadShader(const std::string& filepath, const std::string& name)
{
    Shaders[name] = LiteEngine::Shader::Create(filepath);
    return Shaders[name];
}

LiteEngine::Ref<LiteEngine::Shader> ResourceManager::GetShader(const std::string& name)
{
    return Shaders.at(name);
}

LiteEngine::Ref<LiteEngine::Texture2D> ResourceManager::LoadTexture(const std::string& filepath, const std::string& name)
{
    Textures[name] = LiteEngine::Texture2D::Create(filepath);
    return Textures[name];
}

LiteEngine::Ref<LiteEngine::Texture2D> ResourceManager::GetTexture(const std::string& name)
{
    return Textures.at(name);
}

void ResourceManager::Clear()
{
    Shaders.clear();
    Textures.clear();
}
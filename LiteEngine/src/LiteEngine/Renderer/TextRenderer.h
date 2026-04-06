#pragma once
#include <string>
#include <map>

#include <glm/glm.hpp>

// Forward declarations so this header stays clean
// (actual FreeType includes live only in TextRenderer.cpp)
struct Character {
    unsigned int TextureID; // OpenGL texture ID for the glyph bitmap
    glm::ivec2   Size;      // Width, height of the glyph in pixels
    glm::ivec2   Bearing;   // Offset from baseline to top-left of glyph
    unsigned int Advance;   // Horizontal advance to next glyph (in 1/64 pixels)
};

class TextRenderer
{
public:
    // Projection width/height should match your game window (e.g. 1280, 720)
    TextRenderer(unsigned int screenWidth, unsigned int screenHeight);
    ~TextRenderer();

    // Load a .ttf font at the given pixel size.
    // Call once in Game::Init() after the OpenGL context is ready.
    void Load(const std::string& fontPath, unsigned int fontSize);

    // Render a string at (x, y) in screen-space pixels (top-left origin).
    // scale = 1.0f for normal size.
    // color is an RGB value in [0,1].
    void RenderText(const std::string& text,
        float x, float y,
        float scale,
        glm::vec3 color = glm::vec3(1.0f));

private:
    std::map<char, Character> m_Characters;

    unsigned int m_VAO, m_VBO;
    unsigned int m_ShaderProgram;

    // Compile a single GLSL shader stage and return its ID.
    unsigned int CompileShader(unsigned int type, const char* src);
};
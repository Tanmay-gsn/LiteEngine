#include "hzpch.h"
#include "TextRenderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// FreeType — included only here so the heavy headers stay out of TextRenderer.h
#include <ft2build.h>
#include FT_FREETYPE_H

// ---------------------------------------------------------------------------
// Embedded GLSL shaders
// (kept here so you don't need extra shader asset files for text)
// ---------------------------------------------------------------------------

static const char* TEXT_VERT_SRC = R"(
#version 330 core
layout(location = 0) in vec4 vertex;   // (x, y,  u, v)

out vec2 TexCoords;

uniform mat4 u_Projection;

void main()
{
    gl_Position = u_Projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords   = vertex.zw;
}
)";

static const char* TEXT_FRAG_SRC = R"(
#version 330 core
in  vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D u_Text;
uniform vec3      u_TextColor;

void main()
{
    // FreeType glyphs are single-channel; sample the red channel as alpha.
    float alpha = texture(u_Text, TexCoords).r;
    FragColor   = vec4(u_TextColor, alpha);
}
)";

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

TextRenderer::TextRenderer(unsigned int screenWidth, unsigned int screenHeight)
{
    // --- Build the text shader program -----------------------------------
    unsigned int vert = CompileShader(GL_VERTEX_SHADER, TEXT_VERT_SRC);
    unsigned int frag = CompileShader(GL_FRAGMENT_SHADER, TEXT_FRAG_SRC);

    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, vert);
    glAttachShader(m_ShaderProgram, frag);
    glLinkProgram(m_ShaderProgram);

    int success;
    glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(m_ShaderProgram, 512, nullptr, log);
        std::cerr << "[TextRenderer] Shader link error:\n" << log << "\n";
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    // Upload an orthographic projection that maps pixels 1:1 to screen coords.
    // Origin is top-left (matches LiteEngine's 2-D coordinate system).
    glm::mat4 proj = glm::ortho(
        0.0f, static_cast<float>(screenWidth),
        static_cast<float>(screenHeight), 0.0f
    );

    glUseProgram(m_ShaderProgram);
    glUniformMatrix4fv(
        glGetUniformLocation(m_ShaderProgram, "u_Projection"),
        1, GL_FALSE, glm::value_ptr(proj)
    );
    glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_Text"), 0);
    glUseProgram(0);

    // --- Set up the VAO / VBO for dynamic glyph quads --------------------
    // Each quad needs 6 vertices, each vertex is (x, y, u, v) = 4 floats.
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // Allocate space for one quad (6 vertices * 4 floats), updated each glyph
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------

TextRenderer::~TextRenderer()
{
    // Release every glyph texture
    for (auto& [c, ch] : m_Characters)
        glDeleteTextures(1, &ch.TextureID);

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_ShaderProgram);
}

// ---------------------------------------------------------------------------
// Load  —  initialise FreeType, rasterise ASCII printable range
// ---------------------------------------------------------------------------

void TextRenderer::Load(const std::string& fontPath, unsigned int fontSize)
{
    // Clear any previously loaded characters
    for (auto& [c, ch] : m_Characters)
        glDeleteTextures(1, &ch.TextureID);
    m_Characters.clear();

    // Init FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "[TextRenderer] Could not init FreeType.\n";
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        std::cerr << "[TextRenderer] Failed to load font: " << fontPath << "\n";
        FT_Done_FreeType(ft);
        return;
    }

    // width = 0 lets FreeType derive it from the height automatically
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // FreeType glyph bitmaps are 1 byte per pixel — disable 4-byte row alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Rasterise the first 128 ASCII characters (covers all printable glyphs)
    for (unsigned char c = 0; c < 128; ++c)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "[TextRenderer] Failed to load glyph: " << c << "\n";
            continue;
        }

        // Upload the glyph bitmap as a single-channel (GL_RED) texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left,  face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)  // in 1/64 px
        };

        m_Characters[c] = character;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore default alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

// ---------------------------------------------------------------------------
// RenderText  —  draw a string in screen-space pixels
// ---------------------------------------------------------------------------

void TextRenderer::RenderText(const std::string& text,
    float x, float y,
    float scale,
    glm::vec3 color)
{
    // Save and set up OpenGL state required for alpha-blended text
    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Activate text shader and upload per-call uniforms
    glUseProgram(m_ShaderProgram);
    glUniform3f(glGetUniformLocation(m_ShaderProgram, "u_TextColor"),
        color.r, color.g, color.b);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    // Iterate over each character in the string
    for (char c : text)
    {
        if (m_Characters.find(c) == m_Characters.end())
            continue;

        const Character& ch = m_Characters[c];

        // Compute the quad's top-left pixel position.
        // Bearing.y is the distance from the baseline to the top of the glyph.
        float xPos = x + ch.Bearing.x * scale;
        float yPos = y + (m_Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Build a quad from two triangles (UV origin is top-left)
        float vertices[6][4] = {
            { xPos,     yPos,     0.0f, 0.0f },
            { xPos,     yPos + h, 0.0f, 1.0f },
            { xPos + w, yPos + h, 1.0f, 1.0f },

            { xPos,     yPos,     0.0f, 0.0f },
            { xPos + w, yPos + h, 1.0f, 1.0f },
            { xPos + w, yPos,     1.0f, 0.0f }
        };

        // Bind this glyph's texture
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Upload the quad vertices and draw
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursor: Advance is stored in 1/64 pixels, so >>6 converts to pixels
        x += (ch.Advance >> 6) * scale;
    }

    // Restore OpenGL state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    if (!blendEnabled)
        glDisable(GL_BLEND);
    else
        glBlendFunc(blendSrc, blendDst);
}

// ---------------------------------------------------------------------------
// CompileShader  —  internal helper
// ---------------------------------------------------------------------------

unsigned int TextRenderer::CompileShader(unsigned int type, const char* src)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "[TextRenderer] Shader compile error ("
            << (type == GL_VERTEX_SHADER ? "vert" : "frag")
            << "):\n" << log << "\n";
    }

    return id;
}
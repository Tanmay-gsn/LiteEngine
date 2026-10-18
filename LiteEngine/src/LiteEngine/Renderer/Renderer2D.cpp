#include "hzpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace LiteEngine {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = WhiteTexture

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DData();
		s_Data->QuadVertexArray = VertexArray::Create();

		s_Data->QuadVertexBuffer.reset(VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex)));
		s_Data->QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float,  "a_TexIndex" }
			});
		s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);

		s_Data->QuadVertexBufferBase = new QuadVertex[s_Data->MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB;
		quadIB.reset(IndexBuffer::Create(quadIndices, s_Data->MaxIndices));
		s_Data->QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Create 1x1 pure white texture for flat-colored quads
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->TextureShader->Bind();
		for (uint32_t i = 0; i < s_Data->MaxTextureSlots; i++)
		{
			s_Data->TextureShader->SetInt("u_Textures[" + std::to_string(i) + "]", i);
		}

		s_Data->TextureSlots[0] = s_Data->WhiteTexture;
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data->QuadVertexBufferBase;
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data->QuadIndexCount = 0;
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;

		s_Data->TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
		s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data->QuadIndexCount == 0)
			return;

		// Bind all active textures for this batch
		for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
			s_Data->TextureSlots[i]->Bind(i);

		s_Data->TextureShader->Bind();
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);
		s_Data->Stats.DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data->QuadIndexCount = 0;
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;

		s_Data->TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const float texIndex = 0.0f; // 0.0f = WhiteTexture
		const float halfWidth = size.x * 0.5f;
		const float halfHeight = size.y * 0.5f;

		s_Data->QuadVertexBufferPtr->Position = { position.x - halfWidth, position.y - halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x + halfWidth, position.y - halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x + halfWidth, position.y + halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x - halfWidth, position.y + halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadIndexCount += 6;
		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, glm::vec4(1.0f));
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad(position, size, texture, glm::vec4(1.0f));
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data->TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				FlushAndReset();

			textureIndex = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;
		}

		const float halfWidth = size.x * 0.5f;
		const float halfHeight = size.y * 0.5f;

		s_Data->QuadVertexBufferPtr->Position = { position.x - halfWidth, position.y - halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x + halfWidth, position.y - halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x + halfWidth, position.y + halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x - halfWidth, position.y + halfHeight, position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadIndexCount += 6;
		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data->Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}

}
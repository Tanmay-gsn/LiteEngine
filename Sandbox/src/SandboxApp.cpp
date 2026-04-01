#include "LiteEngine.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "LiteEngine/Events/Event.h"
#include "LiteEngine/Log.h"
#include "spdlog/fmt/ostr.h"

// Force discrete GPU
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

class ExampleLayer : public LiteEngine::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
		m_VertexArray.reset(LiteEngine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		LiteEngine::Ref<LiteEngine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(LiteEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
		LiteEngine::BufferLayout layout = {
			{ LiteEngine::ShaderDataType::Float3, "a_Position" },
			{ LiteEngine::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		LiteEngine::Ref<LiteEngine::IndexBuffer> indexBuffer;
		indexBuffer.reset(LiteEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(LiteEngine::VertexArray::Create());
		float squareVertices[5 * 4] = {
					-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
					 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
					 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
					-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		LiteEngine::Ref<LiteEngine::VertexBuffer> squareVB;
		squareVB.reset(LiteEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			  { LiteEngine::ShaderDataType::Float3, "a_Position" },
			  { LiteEngine::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		LiteEngine::Ref<LiteEngine::IndexBuffer> squareIB;
		squareIB.reset(LiteEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = LiteEngine::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = LiteEngine::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = LiteEngine::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = LiteEngine::Texture2D::Create("assets/textures/LE_Logo.png");

		std::dynamic_pointer_cast<LiteEngine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<LiteEngine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(LiteEngine::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		LiteEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		LiteEngine::RenderCommand::Clear();

		LiteEngine::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<LiteEngine::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<LiteEngine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				LiteEngine::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		glm::mat4 checkerTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
		LiteEngine::Renderer::Submit(textureShader, m_SquareVA, checkerTransform);

		m_ChernoLogoTexture->Bind();
		glm::mat4 logoTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f / 1.329f, 1.0f));
		LiteEngine::Renderer::Submit(textureShader, m_SquareVA, logoTransform);

		// Triangle
		// LiteEngine::Renderer::Submit(m_Shader, m_VertexArray);

		LiteEngine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(LiteEngine::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	LiteEngine::ShaderLibrary m_ShaderLibrary;
	LiteEngine::Ref<LiteEngine::Shader> m_Shader;
	LiteEngine::Ref<LiteEngine::VertexArray> m_VertexArray;

	LiteEngine::Ref<LiteEngine::Shader> m_FlatColorShader;
	LiteEngine::Ref<LiteEngine::VertexArray> m_SquareVA;

	LiteEngine::Ref<LiteEngine::Texture2D> m_Texture, m_ChernoLogoTexture;

	LiteEngine::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public LiteEngine::Application {
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

LiteEngine::Application* LiteEngine::CreateApplication() {
	return new Sandbox();
}
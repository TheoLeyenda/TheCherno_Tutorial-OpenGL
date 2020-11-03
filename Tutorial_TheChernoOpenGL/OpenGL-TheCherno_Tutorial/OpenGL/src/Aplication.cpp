#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error al inicializar glew" << std::endl;
		std::cin.get();
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		float positions[]
		{
					  // texture data
			-0.5f, -0.5f, 0.0f, 0.0f,//index 0
			 0.5f, -0.5f, 1.0f, 0.0f,//index 1
			 0.5f,  0.5f, 1.0f, 1.0f,//index 2
			-0.5f,  0.5f, 0.0f, 1.0f//index 3
		};

		unsigned int indexArray[] =
		{
			0, 1, 2,
			2, 3, 0
		};
		
		GLCALL(glEnable(GL_BLEND));
		GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		const int countPositions = 4 * 4 * sizeof(float);
		unsigned int countIndexBuffer = 6;

		VertexArray vertexArray;
		VertexBuffer vertexBuffer(positions, countPositions);

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		vertexArray.AddBuffer(vertexBuffer, layout);
		vertexArray.Bind();

		IndexBuffer indexBuffer(indexArray, countIndexBuffer);

		glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

		//std::cout << "VERTEX" << std::endl;
		//std::cout << source.vertexSource << std::endl;
		//std::cout << "FRAGMENT" << std::endl;
		//std::cout << source.fragmentSource << std::endl;
		
		Shader shader("res/shaders/BasicShader.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", proj);

		Texture texture("res/texturas/bokitaElMasGrandePapa.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		vertexArray.Unbind();
		vertexBuffer.Unbind();
		indexBuffer.Unbind();
		shader.Unbind();

		Renderer renderer;

		float r = 0.0f;
		float increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			
			renderer.Draw(vertexArray, indexBuffer, shader);

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;

			r += increment;

			/* Swap front and back buffers */
			GLCALL(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCALL(glfwPollEvents());
		}

	}
	glfwTerminate();
	return 0;
}
//DIBUJAMOS UN TRIANGULO
//glBegin(GL_TRIANGLES);
//glVertex2d(-0.5f, -0.5f);
//glVertex2d(0, 0.5f);
//glVertex2d(0.5f, -0.5f);
//glEnd();
//----------------------//
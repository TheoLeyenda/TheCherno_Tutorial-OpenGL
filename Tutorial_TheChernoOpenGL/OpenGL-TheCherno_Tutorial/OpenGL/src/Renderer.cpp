#include "Renderer.h"
#include <iostream>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char * function, const char * file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ") " << function << " " << file << ":" << "(line: " << line << ")" << std::endl;
		return false;
	}
	return true;
}

void Renderer::Clear() const
{
	GLCALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	ib.Bind();
	va.Bind();
	GLCALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

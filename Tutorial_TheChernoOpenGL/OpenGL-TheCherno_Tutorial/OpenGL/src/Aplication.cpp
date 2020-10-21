#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

struct ShaderProgramSource 
{
	std::string vertexSource;
	std::string fragmentSource;
};

static void GLClearError() 
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) 
{
	while (GLenum error = glGetError()) 
	{
		std::cout << "[OpenGL Error] (" << error << ") " <<function<< " " << file<<":" << "(line: "<<line<<")" << std::endl;
		return false;
	}
	return true;
}

static ShaderProgramSource  ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType 
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) // MIENTRAS SEA VALIDO OBTENER UNA LINEA ENTRARA AL BUCLE.
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
			ss[(int)type] << line << '\n';
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	GLCALL(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCALL(glShaderSource(id, 1, &src, nullptr));
	GLCALL(glCompileShader(id));


	int result;
	GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) 
	{
		int length;
		GLCALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCALL(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile "<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			<< std::endl;
		std::cout << message << std::endl;

		GLCALL(glDeleteShader(id));
		std::cin.get();
		return -1;
	}

	return id;
}

static unsigned int CreatedShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
	GLCALL(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCALL(glAttachShader(program, vs));
	GLCALL(glAttachShader(program, fs));
	GLCALL(glLinkProgram(program));
	GLCALL(glValidateProgram(program));

	GLCALL(glDeleteShader(vs));
	GLCALL(glDeleteShader(fs));

	return program;
}


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

	float positions[]
	{
		-0.5f, -0.5f, //index 0
		 0.5f, -0.5f, //index 1
		 0.5f,  0.5f, //index 2
		-0.5f,  0.5f, //index 3
	};
	
	const int countPositions = 8;
	const int stride = 2;
	const int offset = 0;
	const int sizeVertex = sizeof(float) * 2; 
	const int countVetices = 4;

	unsigned int indexBuffer[] = 
	{
		0, 1, 2,
		2, 3, 0
	};
	const int countIndexBuffer = 6;

	unsigned int vao;
	GLCALL(glGenVertexArrays(1, &vao));
	GLCALL(glBindVertexArray(vao));

	unsigned int buffer;
	GLCALL(glGenBuffers(1, &buffer));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, countPositions * sizeof(float),positions , GL_STATIC_DRAW));

	GLCALL(glEnableVertexAttribArray(0));
	GLCALL(glVertexAttribPointer(0, stride, GL_FLOAT, GL_FALSE, sizeVertex, offset));

	unsigned int indexBufferObject;
	GLCALL(glGenBuffers(1, &indexBufferObject));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, countIndexBuffer * sizeof(unsigned int), indexBuffer, GL_STATIC_DRAW));


	//std::cout << "VERTEX" << std::endl;
	//std::cout << source.vertexSource << std::endl;
	//std::cout << "FRAGMENT" << std::endl;
	//std::cout << source.fragmentSource << std::endl;

	ShaderProgramSource source = ParseShader("res/shaders/BasicShader.shader");
	unsigned int shader = CreatedShader(source.vertexSource, source.fragmentSource);
	GLCALL(glUseProgram(shader));

	GLCALL(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	
	GLCALL(glBindVertexArray(0));
	GLCALL(glUseProgram(0));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	float r = 0.0f;
	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));

		GLCALL(glUseProgram(shader));
		GLCALL(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

		GLCALL(glBindVertexArray(vao));
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));

		GLCALL(glDrawElements(GL_TRIANGLES, countIndexBuffer, GL_UNSIGNED_INT, nullptr));

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

	//glDeleteShader(shader);
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
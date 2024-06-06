#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//forward declaration
void processInput(GLFWwindow* window);
int init(GLFWwindow*& window);
void createShaders();
void createTriangle(GLuint& VAO, GLuint& EBO, int& size, int& numIndices);

void createProgram(GLuint& program, const char* vertex, const char* fragment);

//Util
void LoadFile(const char* filename, char*& output);

//program IDs
GLuint simpleProgram;

int main()
{
	std::cout << "Starting App" << std::endl;
	GLFWwindow* window;
	int res = init(window);
	if (res != 0) return res;

	GLuint triangleVAO, triangleEBO;
	int triangleSize, triangleIndexCount;
	createTriangle(triangleVAO, triangleEBO, triangleSize, triangleIndexCount);
	createShaders();

	//Create viewport
	glViewport(0, 0, 1280, 720);
	//game render loop

	while (!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);
		//rendering
		glClearColor(0.5f, 0.7f, 0.3f, 1.0f); //set background
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(simpleProgram);
		glBindVertexArray(triangleVAO);
		//glDrawArrays(GL_TRIANGLES,0, triangleSize);
		glDrawElements(GL_TRIANGLES, triangleIndexCount, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		//event poll
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}
int init(GLFWwindow*& window)
{

	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(1280, 720, "MyEpicWindow", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "fail to create glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//set context
	glfwMakeContextCurrent(window);

	//Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //give functionpointer to the loader
	{
		std::cout << "falied to load glad" << std::endl;
		return -2;
	}
	return 0;
}
void createTriangle(GLuint& VAO, GLuint& EBO, int& size, int& numIndices)
{
	float vertices[] = {
		//position			//color					//index 
	-0.5f, -0.5f, 0.0f,		1.0f,0.0f,0.0f,1.0f,	//0 
	 0.5f, -0.5f, 0.0f,		0.0f,1.0f,0.0f,1.0f,	//1
	-0.5f,  0.5f, 0.0f,		0.0f,0.0f,1.0f,1.0f,	//2
	 0.5f,  0.5f, 0.0f,		1.0f,1.0f,1.0f,1.0f		//3
	};

	int indices[] = {
		0,1,2,
		2,1,3
	};
	int stride = (3 + 4) * sizeof(float);

	size = sizeof(vertices) / stride;
	numIndices = sizeof(indices) / sizeof(int);
	//vertex array
	//GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//vertex buffer
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}
void createShaders()
{
	createProgram(simpleProgram, "Shaders/SimpleVertex.shader", "Shaders/SimpleFragment.shader");
}
void createProgram(GLuint& programID, const char* vertex, const char* fragment)
{
	char* vertexSrc;
	char* FragSrc;
	LoadFile(vertex, vertexSrc);
	LoadFile(fragment, FragSrc);

	GLuint vertexShaderID, fragmentShadeID;

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSrc, nullptr);
	glCompileShader(vertexShaderID);

	int success;
	char infologbuffer[512];
	//check compile status of vertex
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infologbuffer);
		std::cout << "Error during vertexShader compilation\n" << infologbuffer << std::endl;
	}

	fragmentShadeID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShadeID, 1, &FragSrc, nullptr);
	glCompileShader(fragmentShadeID);

	//check compile status of fragment
	glGetShaderiv(fragmentShadeID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShadeID, 512, nullptr, infologbuffer);
		std::cout << "Error during FragmentShader compilation\n" << infologbuffer << std::endl;

	}
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShadeID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 512, nullptr, infologbuffer);
		std::cout << "Error LinkubgProgram\n" << infologbuffer << std::endl;
	}
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShadeID);
	delete vertexSrc;
	delete FragSrc;

}
void LoadFile(const char* filename, char*& output)
{
	std::ifstream file(filename, std::ios::binary);

	if (file.is_open())
	{
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		output = new char[length + 1];
		file.read(output, length);
		output[length] = '\0';
		file.close();

	}
	else
	{
		output = NULL;
	}
}
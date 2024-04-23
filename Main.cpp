#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//forward declaration
void processInput(GLFWwindow* window);
int init(GLFWwindow* &window);
void createTriangle(GLuint &vao, int &size);
void createShaders();
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

	GLuint TriangleVAO;
	int TriangleSize;

	createTriangle(TriangleVAO, TriangleSize);
	createShaders();

	//Create viewport
	glViewport(0, 0, 1280, 720);
	//game render loop

	while(!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);
		//rendering
		glClearColor(0.5f, 0.7f, 0.3f, 1.0f); //set background
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindVertexArray(TriangleVAO);
		glDrawArrays(GL_TRIANGLES,0, TriangleSize);
		glUseProgram(simpleProgram);
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
int init(GLFWwindow* &window)
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
void createTriangle(GLuint& vao, int& size)
{
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};
	//vertex array
	//GLuint VAO;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//vertex buffer
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	
	size = sizeof(vertices);
}
void createShaders()
{
	createProgram(simpleProgram,"Shaders/SimpleVertex.shader", "Shaders/SimpleFragment.shader");
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
	
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infologbuffer);
		std::cout << "Error at vertexShader\n" << infologbuffer << std::endl;
	}

	fragmentShadeID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShadeID, 1, &FragSrc, nullptr);
	glCompileShader(fragmentShadeID);

	glGetShaderiv(fragmentShadeID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShadeID, 512, nullptr, infologbuffer);
		std::cout << "Error at FragmentShader\n" << infologbuffer << std::endl;

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
	std::ifstream file(filename,std::ios::binary);

	if (file.is_open())
	{
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		output = new char[length + 1];
		file.read(output,length);
		output[length] = '\0';
		file.close();

	}
	else
	{
		output = NULL;
	}
}
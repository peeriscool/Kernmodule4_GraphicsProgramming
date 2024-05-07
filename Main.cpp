#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//forward declaration
void processInput(GLFWwindow* window);
int init(GLFWwindow* &window);
void createShaders();
void createGeometry(GLuint &VAO,GLuint& EBO, int& size, int& numIndices);
void createProgram(GLuint& program, const char* vertex, const char* fragment);
GLuint loadTexture(const char* path);
//Util
void LoadFile(const char* filename, char*& output);

//program IDs
GLuint simpleProgram;
const int WIDTH = 1280, HEIGHT = 720;
int main()
{
	std::cout << "Starting App" << std::endl;
	GLFWwindow* window;
	int res = init(window);
	if (res != 0) return res;

	GLuint triangleVAO, triangleEBO;
	int triangleSize, triangleIndexCount;
	createGeometry(triangleVAO, triangleEBO, triangleSize, triangleIndexCount);
	createShaders();
	GLuint boxTex = loadTexture("textures/container2.png");
	//Create viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glm::mat4 world = glm::mat4(1.0f);
	world = glm::rotate(world,glm::radians(45.f),glm::vec3(0,1,0)); //local / rot
	world = glm::scale(world, glm::vec3(1, 1, 1)); //scale
	world = glm::translate(world, glm::vec3(0, 0, 0));


	glm::mat4 view = glm::lookAt(glm::vec3(0, 2.5f, -5.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); //view /camera 

	glm::mat4 projection = glm::perspective(glm::radians(35.f), WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::vec3 lightPosition = glm::vec3(3,3,1);
	//game render loop
	while(!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);
		//rendering
		glClearColor(0.5f, 0.7f, 0.3f, 1.0f); //set background
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(simpleProgram);

		glUniformMatrix4fv(glGetUniformLocation(simpleProgram,"world"),1,GL_FALSE,glm::value_ptr(world));
		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(glGetUniformLocation(simpleProgram,"lightPosition"),1,glm::value_ptr(lightPosition));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boxTex); //id gets matched with channel of texture using shader program
		 
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
int init(GLFWwindow* &window)
{

	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(WIDTH,HEIGHT, "MyEpicWindow", nullptr, nullptr);

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
void createGeometry(GLuint &VAO, GLuint &EBO, int &size, int& numIndices)
{
	// need 24 vertices for normal/uv-mapped Cube
	float vertices[] = {
		// positions            //colors            // tex coords   // normals
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       1.f, 0.f, 0.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       1.f, 0.f, 0.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 2.f,       0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 2.f,       0.f, 0.f, 1.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   -1.f, 1.f,      -1.f, 0.f, 0.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   -1.f, 0.f,      -1.f, 0.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, -1.f,      0.f, 0.f, -1.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, -1.f,      0.f, 0.f, -1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   3.f, 0.f,       0.f, 1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   3.f, 1.f,       0.f, 1.f, 0.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,

		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,

		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       0.f, 1.f, 0.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       0.f, 1.f, 0.f
	};

	unsigned int indices[] = {  // note that we start from 0!
		// DOWN
		0, 1, 2,   // first triangle
		0, 2, 3,    // second triangle
		// BACK
		14, 6, 7,   // first triangle
		14, 7, 15,    // second triangle
		// RIGHT
		20, 4, 5,   // first triangle
		20, 5, 21,    // second triangle
		// LEFT
		16, 8, 9,   // first triangle
		16, 9, 17,    // second triangle
		// FRONT
		18, 10, 11,   // first triangle
		18, 11, 19,    // second triangle
		// UP
		22, 12, 13,   // first triangle
		22, 13, 23,    // second triangle
	};
	int stride = (3 + 3 + 2 + 3) * sizeof(float);

	size = sizeof(vertices) / stride;
	numIndices = sizeof(indices)/sizeof(int);
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
	//pos
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,stride, (void*)0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);
	//uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//normals
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, stride, (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
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
GLuint loadTexture(const char* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D,textureID);
	
	//				Context			What				how?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//				Context			what	Magnification  How?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, heigth, numChannels;								
	unsigned char* data = stbi_load(path, &width, &heigth, &numChannels, 0);
		if (data)
		{
			
			
			if (numChannels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);		 //rgb	
			else if(numChannels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//rgba
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture loading error" << path << std::endl;
		}
		stbi_image_free(data); //delete pointer 
		glBindTexture(GL_TEXTURE_2D, 0);
		return textureID;
}
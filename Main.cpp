#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//forward declaration
void processInput(GLFWwindow* window);
int init(GLFWwindow* &window);
void createTriangle(GLuint &vao, int &size);
void createShaders();

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
		glClearColor(0.5, 0.7, 03, 1.0); //set background
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindVertexArray(TriangleVAO);
		glDrawArrays(GL_TRIANGLES,0, TriangleSize);

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

}
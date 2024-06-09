#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include "GeoClass.h"
#include "MTLHelper.h"
//int main();
//forward declaration
void processInput(GLFWwindow* window);
void createShaders();
void createGeometry(GLuint &VAO,GLuint& EBO, int& size, int& numIndices);
void createProgram(GLuint& program, const char* vertex, const char* fragment);
void renderSkybox();
void renderTerrain();
void renderModel(Model* model, GLuint program, glm::vec3 pos,glm::vec3 rot, glm::vec3 scale);
void renderModel(Model* model, GLuint program);
void RenderUfo(Model* model, GLuint program, float r, float g, float b, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
int init(GLFWwindow*& window);
unsigned int GeneratePlane(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);
//unsigned int GeneratePlane(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);
unsigned int createPlane(GLuint& VAO, GLuint& VBO, GLuint& EBO, int& size, int& numIndices);
//callbakcs
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool keys[1024];

//Util
void LoadFile(const char* filename, char*& output);
GLuint loadTexture(const char* path, int comp = 0);

//program IDs
GLuint simpleProgram, SkyProgram, terrainProgram,modelProgram, UfoProgram;
GLuint GeometryVAO, GeometryEBO;

//terain data
GLuint terrainVAO, terrainIndeCount, heightmapID,heightNormalID;
unsigned char* heigthMapTexture;

//values
int geoSize, geoIndexCount;
GLuint dirt, sand, grass, rock, snow;
Model* backpack;
//ufo 
Model* Icosphere,* Torus,* legs,* detail;
//, torus, leg, leg2, leg3, detail;
const float cameraSpeed = 0.05f;
const int WIDTH = 1280, HEIGHT = 720;

//vec3
glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.5f, -0.5f, -0.5f));
glm::vec3 cameraPosition = glm::vec3(0, 1250.5f, 5.0f);
glm::mat4 projection, view;
float lastX, lastY;
bool FirstMouse = true;
float camYaw, camPitch;

//cam
glm::quat camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0));
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float FOV = 35.0f;
//mtl
std::vector<Material> UfoMaterials;
int main()
{
	std::cout << "Starting App" << std::endl;
	GLFWwindow* window;
	int res = init(window);
	if (res != 0) return res;

	stbi_set_flip_vertically_on_load(true);

	createShaders();
	createGeometry(GeometryVAO, GeometryEBO, geoSize, geoIndexCount);

	//GeoClass::createSphereGeometry(sphereVAO, SPhereEBO, tempSize, tempNumIndices);
	terrainVAO = GeneratePlane("resources/textures/Heightmap2.png", heigthMapTexture,GL_RGBA,4,100.0f,5.0f,terrainIndeCount, heightmapID);
	heightNormalID = loadTexture("resources/textures/heightnormal.png");
	GLuint boxTex = loadTexture("resources/textures/container2.png");
	GLuint boxNormal = loadTexture("resources/textures/container2_Normal.png");
	
	dirt = loadTexture("resources/textures/dirt.jpg");
	sand = loadTexture("resources/textures/sand.jpg");
	grass = loadTexture("resources/textures/grass.png",4);
	rock = loadTexture("resources/textures/rock.jpg");
	snow = loadTexture("resources/textures/snow.jpg");

	backpack = new Model("resources/models/backpack/backpack.obj");

	//constructing the Ufo
	Icosphere = new Model("resources/models/Ufo/Icosphere.obj");
	Torus = new Model("resources/models/Ufo/Torus.obj");
	legs = new Model("resources/models/Ufo/legs.obj");
	detail = new Model("resources/models/Ufo/Details.obj");

	UfoMaterials = loadMTL("resources/models/Ufo/FullUfo.mtl");
	/*
	for (const auto& material : UfoMaterials)
	{
		std::cout << "Material: " << material.name << std::endl;
		std::cout << "  Ns: " << material.Ns << std::endl;
		std::cout << "  Ka: " << material.Ka[0] << " " << material.Ka[1] << " " << material.Ka[2] << std::endl;
		std::cout << "  Kd: " << material.Kd[0] << " " << material.Kd[1] << " " << material.Kd[2] << std::endl; //diffuse
		std::cout << "  Ks: " << material.Ks[0] << " " << material.Ks[1] << " " << material.Ks[2] << std::endl;
		std::cout << "  Ke: " << material.Ke[0] << " " << material.Ke[1] << " " << material.Ke[2] << std::endl;
		std::cout << "  Ni: " << material.Ni << std::endl;
		std::cout << "  d: " << material.d << std::endl;
		std::cout << "  illum: " << material.illum << std::endl;
	}
	*/
	//Create viewport
	glViewport(0, 0, WIDTH, HEIGHT);

	//game render loop
	while(!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);
		//rendering
		//glClearColor(0.5f, 0.7f, 0.3f, 1.0f); //set background
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//update control values
		projection = glm::perspective(glm::radians(FOV), WIDTH / (float)HEIGHT, 0.1f, 6000.0f);
		
		renderSkybox();
		glUseProgram(terrainProgram);

		renderTerrain();
	//	float t = glfwGetTime();
	//	renderModel(backpack,modelProgram,glm::vec3(1000,100,1000), glm::vec3(0, t, 0), glm::vec3(100, 100, 100));
		RenderUfo(Icosphere, UfoProgram, UfoMaterials[1].Kd[0], UfoMaterials[1].Kd[1], UfoMaterials[1].Kd[2], glm::vec3(10, 100, 10), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)); // blueishx
		RenderUfo(detail, UfoProgram, UfoMaterials[2].Kd[0] * 10, UfoMaterials[2].Kd[1] * 10, UfoMaterials[2].Kd[2] * 10, glm::vec3(10, 100, 10), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)); //black
		RenderUfo(Torus, UfoProgram, UfoMaterials[3].Kd[0], UfoMaterials[3].Kd[1], UfoMaterials[3].Kd[2],glm::vec3(10, 100, 10), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)); //green
		RenderUfo(legs, UfoProgram, UfoMaterials[0].Kd[0], UfoMaterials[0].Kd[1], UfoMaterials[0].Kd[2],glm::vec3(10, 100, 10), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)); //grey
		//renderModel(Torus, simpleProgram, glm::vec3(1500, 150, 1500), glm::vec3(t, t, t), glm::vec3(600, 600, 600));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boxTex); //id gets matched with channel of texture using shader program
		 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, boxNormal); //id gets matched with channel of texture using shader program
		
	
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
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		//change fov +
		FOV += 0.1f;
		std::cout << "Fov" << FOV << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		//change fov -
		if (FOV > 0)
		{
			FOV -= 0.1f;
			std::cout << "Fov" << FOV << std::endl;
		}
	}
	bool camerachange = false;
		//keybord
		if (keys[GLFW_KEY_W])
		{
			cameraPosition += camQuat * glm::vec3(0, 0, 1);
			camerachange = true;
		}
		if (keys[GLFW_KEY_A])
		{
			cameraPosition += camQuat * glm::vec3(1, 0, 0);
			camerachange = true;

		}
		if (keys[GLFW_KEY_S])
		{
			cameraPosition += camQuat * glm::vec3(0, 0, -1);
			camerachange = true;

		}
		if (keys[GLFW_KEY_D])
		{
			cameraPosition += camQuat * glm::vec3(-1, 0, 0);
			camerachange = true;

		}
		if (keys[GLFW_KEY_Q])
		{
			cameraPosition += camQuat * glm::vec3(0, -1, 0);
			camerachange = true;

		}
		if (keys[GLFW_KEY_E])
		{
			cameraPosition += camQuat * glm::vec3(0, 1, 0);
			camerachange = true;

		}
		if (camerachange) //recalculate cam
		{
			glm::vec3 camForward = camQuat * glm::vec3(0, 0, 1);
			glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0);
			view = glm::lookAt(cameraPosition, cameraPosition + camForward, camUp);
		}
		/*if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			cameraPosition += cameraSpeed * cameraFront;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cameraPosition -= cameraSpeed * cameraFront;
		}*/
	
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
	//register callback
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window,Key_callback);
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
void createGeometry(GLuint &VAO, GLuint &EBO, int &size, int& numIndices) //creates plane
{
	// need 24 vertices for normal/uv-mapped Cube
	float vertices[] = {
		// positions            //colors            // tex coords   // normals          //tangents      //bitangents
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f
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
	int stride = (3 + 3 + 2 + 3 + 3 + 3) * sizeof(float);

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
	//tangents 
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_TRUE, stride, (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);
	//bitangents
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_TRUE, stride, (void*)(14 * sizeof(float)));
	glEnableVertexAttribArray(5);
}
void createShaders()
{
	//createProgram(simpleProgram, "Shaders/SimpleVertex.shader", "Shaders/SimpleFragment.shader");
	
	//glUseProgram(simpleProgram);
	//glUniform1i(glGetUniformLocation(simpleProgram, "diffuseTex"), 0);
	//glUniform1i(glGetUniformLocation(simpleProgram, "normalTex"), 1);
	createProgram(SkyProgram,"resources/Shaders/skyVertex.shader","resources/Shaders/skyFragment.shader");

	createProgram(terrainProgram, "resources/Shaders/terrainVertex.shader", "resources/Shaders/terrainFragment.shader");

	glUseProgram(terrainProgram);
	glUniform1i(glGetUniformLocation(terrainProgram, "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(terrainProgram, "normalTex"), 1);

	glUniform1i(glGetUniformLocation(terrainProgram, "dirt"), 2);
	glUniform1i(glGetUniformLocation(terrainProgram, "sand"), 3);
	glUniform1i(glGetUniformLocation(terrainProgram, "grass"), 4);
	glUniform1i(glGetUniformLocation(terrainProgram, "rock"), 5);
	glUniform1i(glGetUniformLocation(terrainProgram, "snow"), 6);

	createProgram(modelProgram, "resources/Shaders/model.vs", "resources/Shaders/model.fs");
	glUseProgram(modelProgram);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_diffuse1"), 0);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_specular1"), 1);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_normal1"), 2);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_roughness1"), 3);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_ao1"), 4);

	createProgram(UfoProgram,"resources/Shaders/UfoVertex.shader", "resources/Shaders/UfoFragment.shader");
	glUseProgram(UfoProgram);
	glUniform1i(glGetUniformLocation(UfoProgram, "Vcolor"), 1);



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
GLuint loadTexture(const char* path, int comp)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D,textureID);
	
	//				Context			What				how?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//				Context			what	Magnification  How?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, heigth, numChannels;								
	unsigned char* data = stbi_load(path, &width, &heigth, &numChannels, comp);
		if (data)
		{
			if (comp != 0) numChannels = comp;
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
void renderSkybox()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(SkyProgram);

	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, cameraPosition);
	world = glm::scale(world, glm::vec3(100, 100, 100)); //scale

	glUniformMatrix4fv(glGetUniformLocation(SkyProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(SkyProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(SkyProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(SkyProgram, "lightDirection"), GL_FALSE, glm::value_ptr(lightDirection));
	//issue with using glunif3fv function
	glUniform3f(glGetUniformLocation(SkyProgram, "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//rendering
	glBindVertexArray(GeometryVAO);
	glDrawElements(GL_TRIANGLES, geoIndexCount, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);  
	glEnable(GL_DEPTH);

}
void renderTerrain()
{
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(terrainProgram);

	glm::mat4 world = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	float t = glfwGetTime();
	lightDirection = glm::normalize( glm::vec3(glm::sin(t), -0.5f, glm::cos(t)));
	glUniform3f(glGetUniformLocation(terrainProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(terrainProgram, "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmapID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightNormalID);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dirt);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sand);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, grass);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rock);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, snow);

	//rendering
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES,terrainIndeCount,GL_UNSIGNED_INT,0);

}
void renderModel(Model* model, GLuint program)
{
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(program);

	glm::mat4 world = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(program, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(program, "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
	//

	model->Draw(program);

}
void RenderUfo(Model* model, GLuint program,float r,float g,float b ,glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(program);

	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, pos);
	world = world * glm::mat4(glm::quat(rot)); //should be to mat4 missing function
	world = glm::scale(world, scale);
	glUniformMatrix4fv(glGetUniformLocation(program, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(program, "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

	glUniform3f(glGetUniformLocation(UfoProgram, "Kd"), r, g, b); // blueish


	model->Draw(program);

	glDisable(GL_BLEND);
}
void renderModel(Model* model , GLuint program, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	glEnable(GL_BLEND);
	//alpha blend
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//additive
	//glBlendFunc(GL_ONE, GL_ONE);
	//soft additive
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
	//multiply
	//glBlendFunc(GL_DST_COLOR,GL_ZERO);
	//double multiply
	//glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(program);

	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, pos);
	world = world * glm::mat4(glm::quat(rot)); //should be to mat4 missing function
	world = glm::scale(world, scale);
	glUniformMatrix4fv(glGetUniformLocation(program, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(program, "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
	


	model->Draw(program);

	glDisable(GL_BLEND);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float x = (float)xpos;
	float y = (float)ypos;

	if (FirstMouse)
	{
		lastX = x;
		lastY = y;
		FirstMouse = false;
	}

	float distanceX = x - lastX;
	float distanceY = y - lastY;

	lastX = x;
	lastY = y;

	camYaw -= distanceX;
	camPitch = glm::clamp(camPitch + distanceY, -90.0f, 90.0f);

	if (camYaw > 180.0f) camYaw -= 360.0f;//loop back to spectrum -180
	if (camYaw - 180.0f) camYaw += 360.0f;//loop back to spectrum 180
	camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw),0));

	glm::vec3 camForward = camQuat * glm::vec3(0, 0, 1) ;
	glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0) ;
	view = glm::lookAt(cameraPosition, cameraPosition + camForward, camUp);
}
void Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
	
}
unsigned int GeneratePlane(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID)
{
	int width, height, channels;
	data = nullptr;
	if (heightmap != nullptr) {
		data = stbi_load(heightmap, &width, &height, &channels, comp); //read texture
		if (data) {
			glGenTextures(1, &heightmapID);
			glBindTexture(GL_TEXTURE_2D, heightmapID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	int stride = 8;
	float* vertices = new float[(width * height) * stride];
	unsigned int* indices = new unsigned int[(width - 1) * (height - 1) * 6];

	int index = 0; //vertex index where u are at within vertices
	for (int i = 0; i < (width * height); i++) {
		// TODO: calculate x/z values
		int x = i % width;
		int z = i / width;

		float texHeight = (float)data[(z * width + x) * comp];
		// TODO: set position
		vertices[index++] = x * xzScale;
		vertices[index++] = (texHeight / 255.0f) * hScale;
		vertices[index++] = z * xzScale;


		// TODO: set normal
		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;

		// TODO: set uv
		vertices[index++] = x / (float)width;
		vertices[index++] = z / (float)height;

	}

	// OPTIONAL TODO: Calculate normal
	// TODO: Set normal

	index = 0;
	for (int i = 0; i < (width - 1) * (height - 1); i++) {
		int x = i % width - 1;
		int z = i / width - 1;

		//get correct vertex position from data
		int vertex = z * width + x;
		indices[index++] = vertex;				//top left
		indices[index++] = vertex + width;		//bottom left
		indices[index++] = vertex + width + 1;  //bottom right
		indices[index++] = vertex;				//top left
		indices[index++] = vertex + width + 1;  //bottom right
		indices[index++] = vertex + 1;			//top right
	}

	unsigned int vertSize = (width * height) * stride * sizeof(float);
	indexCount = ((width - 1) * (height - 1) * 6);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// vertex information!
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, 0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	// uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	delete[] vertices;
	delete[] indices;

	//stbi_image_free(data);

	return VAO;
}
unsigned int createPlane(GLuint& VAO, GLuint& VBO, GLuint& EBO, int& size, int& numIndices)
{
	// Define vertices for a plane (two triangles)
	float vertices[] = {
		// Positions        // Colors          // Texture coordinates
		-0.5f,  0.0f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Top-left
		 0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Top-right
		-0.5f,  0.0f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // Bottom-left
		 0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f  // Bottom-right
	};

	// Define indices for two triangles
	unsigned int indices[] = {
		0, 1, 2, // First triangle
		1, 3, 2  // Second triangle
	};

	// Calculate the sizes
	size = sizeof(vertices);
	numIndices = sizeof(indices) / sizeof(indices[0]);

	// Generate buffers and arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the VAO
	glBindVertexArray(VAO);

	// Bind and fill the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	// Bind and fill the element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position attribute
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Color attribute
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinate attribute
	glEnableVertexAttribArray(2);

	// Unbind the VAO
	glBindVertexArray(0);

	// Return the VAO
	return VAO;
}

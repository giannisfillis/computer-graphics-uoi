//********************************
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1B της OpenGL
//
//ΑΜ:  5380                       Όνομα: IOANNIS FILLIS
//ΑΜ:  5226                       Όνομα: KONSTANTINOS ZOIS

//*********************************

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

//for sleep func
#include <windows.h>

/// <summary>
/// //////////////////////////////////////////////
/// </summary>


bool checkMove(GLfloat map[10][10], GLfloat* player);
void createMap(GLfloat map[10][10]);
GLfloat* upscalePlayer(GLfloat a[]);
void moveSquare(GLfloat* player, GLfloat map[10][10], int move);
void moveToStart(GLfloat* player);
void moveToEnd(GLfloat* player);
bool isAtStart(GLfloat* player);
bool isAtEnd(GLfloat* player);



////////////////////////////////////////////// <summary>
/// Add camera function here
/// </summary>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::mat4 ZoomMatrix;
glm::mat4 tempMatrix;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

void camera_function()
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {//up
		ViewMatrix = glm::rotate(ViewMatrix, 0.025f, glm::vec3(1, 0, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {//down
		ViewMatrix = glm::rotate(ViewMatrix, -0.025f, glm::vec3(1, 0, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { //right
		ViewMatrix = glm::rotate(ViewMatrix, 0.025f, glm::vec3(0, 1, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { //left
		ViewMatrix = glm::rotate(ViewMatrix, -0.025f, glm::vec3(0, 1, 0));
	}
	else if(glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){ // zoom in
		ZoomMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.025f));
		tempMatrix = ZoomMatrix * ViewMatrix;
		if ((0- tempMatrix[3][2]) >=0.1) {
			ViewMatrix = ZoomMatrix * ViewMatrix;
		}
	

	}
	else if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) { // zoom out
			ZoomMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.025f));
			tempMatrix = ZoomMatrix * ViewMatrix;
			if ((0 - tempMatrix[3][2]) <= 20.0) {
				ViewMatrix = ZoomMatrix * ViewMatrix;
			}
	}
}

/////////////////////////////////////////////////

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
///////////////////////////////////////////////////

bool isAtStart(GLfloat* player) {
	//check (xmin,ymax) point of starting block
	if (player[0] == -4.75f && player[1] == 2.75f) {
		return true;
	}
	else {
		return false;
	}
}

void moveToStart(GLfloat* player) {
	//x
	for (int i = 0; i < 108; i = i + 3) {
		player[i] = player[i] - 9.0f;
	}
	//y
	for (int i = 1; i < 108; i = i + 3) {
		player[i] = player[i] + 5.0f;
	}
}

bool isAtEnd(GLfloat* player) {
	//check (xmin,ymax) point of exit block
	if (player[0] == 4.25f && player[1] == -2.25f) {
		return true;
	}
	else {
		return false;
	}
}

void moveToEnd(GLfloat* player) {
	//x
	for (int i = 0; i < 108; i = i + 3) {
		player[i] = player[i] + 9.0f;
	}
	//y
	for (int i = 1; i < 108; i = i + 3) {
		player[i] = player[i] - 5.0f;
	}
}

void moveSquare(GLfloat* player, GLfloat map[10][10], int move) {

	GLfloat temp[108];
	for (int i = 0; i < 108; i++) {
		temp[i] = player[i];
	}
	if (move == 1) { //move right
		//check if player is at exit point
		if (isAtEnd(player)) {
			moveToStart(player);
		}
		else {
			for (int i = 0; i < 108; i = i + 3) {
				temp[i] = temp[i] + 1.0f;
			}
			if (checkMove(map, temp) != false) {
				for (int i = 0; i < 108; i = i + 3) {
					player[i] = player[i] + 1.0f;
				}
			}
		}
	}
	else if (move == 2) { // move left
		//check if player is at starting point
		if (isAtStart(player)) {
			moveToEnd(player);
		}
		else {
			for (int i = 0; i < 108; i = i + 3) {
				temp[i] = temp[i] - 1.0f;
			}
			if (checkMove(map, temp) != false) {
				for (int i = 0; i < 108; i = i + 3) {
					player[i] = player[i] - 1.0f;
				}
			}
		}
	}
	else if (move == 3) { // move up
		for (int i = 1; i < 108; i = i + 3) {
			temp[i] = temp[i] + 1.0f;
		}
		if (checkMove(map, temp) != false) {
			for (int i = 1; i < 108; i = i + 3) {
				player[i] = player[i] + 1.0f;
			}
		}
	}
	else { //move down
		for (int i = 1; i < 108; i = i + 3) {
			temp[i] = temp[i] - 1.0f;
		}
		if (checkMove(map, temp) != false) {
			for (int i = 1; i < 108; i = i + 3) {
				player[i] = player[i] - 1.0f;
			}
		}
	}
}

//upscale player to a full block and store (xmin,ymax)
GLfloat* upscalePlayer(GLfloat a[]) {
	//upscale bottom side
	GLfloat temp[2] = { 0.0f, 0.0f };
	GLfloat x = a[0];
	GLfloat y = a[1];
	//x
	if (a[0] < 0) {
		temp[0] = x - 0.25f;
	}
	else {
		temp[0] = x + 0.25f;
	}
	//y
	if (a[1] < 0) {
		temp[1] = y - 0.25f;
	}
	else {
		temp[1] = y + 0.25f;
	}
	return temp;
}

void createMap(GLfloat map[10][10]) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			map[i][j] = 0.0f;
		}
	}
	for (int k = 0; k < 10; k++) {
		map[0][k] = 1.0f;
	}
	map[1][0] = 1.0f;
	for (int l = 1; l < 7; l++) {
		map[l][9] = 1.0f;
	}
	for (int l = 3; l < 9; l++) {
		map[l][0] = 1.0f;
	}
	for (int l = 0; l < 10; l++) {
		map[9][l] = 1.0f;
	}

	map[2][2] = 1.0f, map[3][2] = 1.0f, map[4][2] = 1.0f;
	map[2][3] = 1.0f, map[2][4] = 1.0f, map[2][5] = 1.0f;
	map[2][7] = 1.0f, map[3][7] = 1.0f, map[4][7] = 1.0f;
	map[4][4] = 1.0f, map[4][5] = 1.0f;
	map[6][2] = 1.0f, map[6][3] = 1.0f;
	map[5][5] = 1.0f, map[6][5] = 1.0f;
	map[6][6] = 1.0f, map[6][7] = 1.0f;
	map[8][4] = 1.0f, map[8][5] = 1.0f;
	map[8][2] = 1.0f;
	map[7][7] = 1.0f;
	map[8][9] = 1.0f;

}

//check if move is valid, compare player block with map block
bool checkMove(GLfloat map[10][10], GLfloat* player) {

	//check bottom side only

	//check if player is out of map
	for (int i = 0; i < 18; i = i + 3) {
		if (player[i] > 5 || player[i] < -5) {
			return false;
		}
	}

	GLfloat* temp = upscalePlayer(player);
	int row = 0;
	int col = 0;

	//transform player coords to map cords

	//2nd quarter
	if (temp[0] < 0 && temp[1] > 0) {
		row = -(temp[1] - 5);
		col = temp[0] + 5;
		if (map[row][col] == 1.0f) {
			return false;
		}
	}
	//1st quarter
	else if (temp[0] > 0 && temp[1] > 0) {
		row = -temp[1] + 5;
		col = temp[0] + 5;
		if (map[row][col] == 1.0f) {
			return false;
		}
	}
	//4th quarter
	else if (temp[0] > 0 && temp[1] < 0) {
		row = -temp[1] + 5;
		col = temp[0] + 5;
		if (map[row][col] == 1.0f) {
			return false;
		}
	}
	//3rd quarter
	else if (temp[0] < 0 && temp[1] < 0) {
		row = -temp[1] + 5;
		col = temp[0] + 5;
		if (map[row][col] == 1.0f) {
			return false;
		}
	}

	return true;

}




int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(950, 950, u8"Άσκηση 1Β - 2024", NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders

	GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 20.0f),
		glm::vec3(0.0f, 0.0f, 0.25f),
		glm::vec3(0.0f, 1.0f, 0.0f));  // Head is up 

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * Model;
	GLfloat len = 5.0f, wid = 2.5f, heig = 2.5f;

	//up side same as bottom with z=1
	//back side same as front with different y
	//left side same as right with different x

	GLfloat map[10][10];
	createMap(map);

	static const GLfloat maze[] =
	{

		// (-5,5) (-5,4) (5,5) (5,4) block
		//bottom
		-5.0f, 5.0f, 0.0f,
		-5.0f, 4.0f, 0.0f,
		5.0f, 5.0f, 0.0f,
		-5.0f, 4.0f, 0.0f,
		5.0f, 5.0f, 0.0f,
		5.0f, 4.0f, 0.0f,
		//up
		-5.0f, 5.0f, 1.0f,
		-5.0f, 4.0f, 1.0f,
		5.0f, 5.0f, 1.0f,
		-5.0f, 4.0f, 1.0f,
		5.0f, 5.0f, 1.0f,
		5.0f, 4.0f, 1.0f,
		//front
		-5.0f, 4.0f, 0.0f,
		-5.0f, 4.0f, 1.0f,
		5.0f, 4.0f, 0.0f,
		5.0f, 4.0f, 0.0f,
		-5.0f, 4.0f, 1.0f,
		5.0f, 4.0f, 1.0f,
		//back
		-5.0f, 5.0f, 0.0f,
		-5.0f, 5.0f, 1.0f,
		5.0f, 5.0f, 0.0f,
		5.0f, 5.0f, 0.0f,
		-5.0f, 5.0f, 1.0f,
		5.0f, 5.0f, 1.0f,
		//right
		5.0f, 4.0f, 0.0f,
		5.0f, 4.0f, 1.0f,
		5.0f, 5.0f, 0.0f,
		5.0f, 5.0f, 0.0f,
		5.0f, 4.0f, 1.0f,
		5.0f, 5.0f, 1.0f,
		//left
		-5.0f, 4.0f, 0.0f,
		-5.0f, 4.0f, 1.0f,
		-5.0f, 5.0f, 0.0f,
		-5.0f, 5.0f, 0.0f,
		-5.0f, 4.0f, 1.0f,
		-5.0f, 5.0f, 1.0f,

		// (-5,4) (-5,3) (-4,4) (-4,3) block
		//bottom
		-5.0f, 4.0f, 0.0f,
		-5.0f, 3.0f, 0.0f,
		-4.0f, 4.0f, 0.0f,
		-5.0f, 3.0f, 0.0f,
		-4.0f, 4.0f, 0.0f,
		-4.0f, 3.0f, 0.0f,
		//up
		-5.0f, 4.0f, 1.0f,
		-5.0f, 3.0f, 1.0f,
		-4.0f, 4.0f, 1.0f,
		-5.0f, 3.0f, 1.0f,
		-4.0f, 4.0f, 1.0f,
		-4.0f, 3.0f, 1.0f,
		//front
		-5.0f, 3.0f, 0.0f,
		-5.0f, 3.0f, 1.0f,
		-4.0f, 3.0f, 0.0f,
		-5.0f, 3.0f, 1.0f,
		-4.0f, 3.0f, 0.0f,
		-4.0f, 3.0f, 1.0f,
		//back
		-5.0f, 4.0f, 0.0f,
		-5.0f, 4.0f, 1.0f,
		-4.0f, 4.0f, 0.0f,
		-5.0f, 4.0f, 1.0f,
		-4.0f, 4.0f, 0.0f,
		-4.0f, 4.0f, 1.0f,
		//right
		-4.0f, 3.0f, 0.0f,
		-4.0f, 3.0f, 1.0f,
		-4.0f, 4.0f, 0.0f,
		-4.0f, 4.0f, 0.0f,
		-4.0f, 3.0f, 1.0f,
		-4.0f, 4.0f, 1.0f,
		//left
		-5.0f, 3.0f, 0.0f,
		-5.0f, 3.0f, 1.0f,
		-5.0f, 4.0f, 0.0f,
		-5.0f, 4.0f, 0.0f,
		-5.0f, 3.0f, 1.0f,
		-5.0f, 4.0f, 1.0f,

		// (4,4) (5,4) (4,-2) (5,-2) block
		//bottom
		4.0f, 4.0f, 0.0f,
		5.0f, 4.0f, 0.0f,
		5.0f, -2.0f, 0.0f,
		4.0f, 4.0f, 0.0f,
		4.0f, -2.0f, 0.0f,
		5.0f, -2.0f, 0.0f,
		//up
		4.0f, 4.0f, 1.0f,
		5.0f, 4.0f, 1.0f,
		5.0f, -2.0f, 1.0f,
		4.0f, 4.0f, 1.0f,
		4.0f, -2.0f, 1.0f,
		5.0f, -2.0f, 1.0f,
		//front
		4.0f, -2.0f, 0.0f,
		4.0f, -2.0f, 1.0f,
		5.0f, -2.0f, 0.0f,
		5.0f, -2.0f, 0.0f,
		4.0f, -2.0f, 1.0f,
		5.0f, -2.0f, 1.0f,
		//back
		4.0f, 4.0f, 0.0f,
		4.0f, 4.0f, 1.0f,
		5.0f, 4.0f, 0.0f,
		5.0f, 4.0f, 0.0f,
		4.0f, 4.0f, 1.0f,
		5.0f, 4.0f, 1.0f,
		//right
		5.0f, -2.0f, 0.0f,
		5.0f, -2.0f, 1.0f,
		5.0f, 4.0f, 0.0f,
		5.0f, -2.0f, 1.0f,
		5.0f, 4.0f, 0.0f,
		5.0f, 4.0f, 1.0f,
		//left
		4.0f, -2.0f, 0.0f,
		4.0f, -2.0f, 1.0f,
		4.0f, 4.0f, 0.0f,
		4.0f, -2.0f, 1.0f,
		4.0f, 4.0f, 0.0f,
		4.0f, 4.0f, 1.0f,

		// (4,-3) (4,-5) (5,-3) (5,-5) block
		//bottom
		4.0f, -3.0f, 0.0f,
		5.0f, -3.0f, 0.0f,
		5.0f, -5.0f, 0.0f,
		4.0f, -3.0f, 0.0f,
		4.0f, -5.0f, 0.0f,
		5.0f, -5.0f, 0.0f,
		//up
		4.0f, -3.0f, 1.0f,
		5.0f, -3.0f, 1.0f,
		5.0f, -5.0f, 1.0f,
		4.0f, -3.0f, 1.0f,
		4.0f, -5.0f, 1.0f,
		5.0f, -5.0f, 1.0f,
		//front
		4.0f, -5.0f, 0.0f,
		4.0f, -5.0f, 1.0f,
		5.0f, -5.0f, 0.0f,
		5.0f, -5.0f, 0.0f,
		4.0f, -5.0f, 1.0f,
		5.0f, -5.0f, 1.0f,
		//back
		4.0f, -3.0f, 0.0f,
		4.0f, -3.0f, 1.0f,
		5.0f, -3.0f, 0.0f,
		5.0f, -3.0f, 0.0f,
		4.0f, -3.0f, 1.0f,
		5.0f, -3.0f, 1.0f,
		//right
		5.0f, -5.0f, 0.0f,
		5.0f, -5.0f, 1.0f,
		5.0f, -3.0f, 0.0f,
		5.0f, -3.0f, 0.0f,
		5.0f, -5.0f, 1.0f,
		5.0f, -3.0f, 1.0f,
		//left
		4.0f, -5.0f, 0.0f,
		4.0f, -5.0f, 1.0f,
		4.0f, -3.0f, 0.0f,
		4.0f, -3.0f, 0.0f,
		4.0f, -5.0f, 1.0f,
		4.0f, -3.0f, 1.0f,

		// (-5,-4) (-5,-5) (4,-4) (4,-5) block
		//bottom
		-5.0f, -4.0f, 0.0f,
		-5.0f, -5.0f, 0.0f,
		4.0f, -5.0f, 0.0f,
		-5.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 0.0f,
		4.0f, -5.0f, 0.0f,
		//up
		-5.0f, -4.0f, 1.0f,
		-5.0f, -5.0f, 1.0f,
		4.0f, -5.0f, 1.0f,
		-5.0f, -4.0f, 1.0f,
		4.0f, -4.0f, 1.0f,
		4.0f, -5.0f, 1.0f,
		//front
		-5.0f, -5.0f, 0.0f,
		-5.0f, -5.0f, 1.0f,
		4.0f, -5.0f, 0.0f,
		4.0f, -5.0f, 0.0f,
		4.0f, -5.0f, 1.0f,
		-5.0f, -5.0f, 1.0f,
		//back
		-5.0f, -4.0f, 0.0f,
		-5.0f, -4.0f, 1.0f,
		4.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 1.0f,
		-5.0f, -4.0f, 1.0f,
		//right
		4.0f, -5.0f, 0.0f,
		4.0f, -5.0f, 1.0f,
		4.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 1.0f,
		4.0f, -5.0f, 1.0f,
		//left
		-5.0f, -5.0f, 0.0f,
		-5.0f, -5.0f, 1.0f,
		-5.0f, -4.0f, 0.0f,
		-5.0f, -4.0f, 0.0f,
		-5.0f, -4.0f, 1.0f,
		-5.0f, -5.0f, 1.0f,

		// (-5,2) (-4,2) (-5,-4) (-4,-4) block
		//bottom
		-5.0f, 2.0f, 0.0f,
		-5.0f, -4.0f, 0.0f,
		-4.0f, -4.0f, 0.0f,
		-5.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 0.0f,
		-4.0f, -4.0f, 0.0f,
		//up
		-5.0f, 2.0f, 1.0f,
		-5.0f, -4.0f, 1.0f,
		-4.0f, -4.0f, 1.0f,
		-5.0f, 2.0f, 1.0f,
		-4.0f, 2.0f, 1.0f,
		-4.0f, -4.0f, 1.0f,
		//front
		-5.0f, -4.0f, 0.0f,
		-5.0f, -4.0f, 1.0f,
		-4.0f, -4.0f, 0.0f,
		-4.0f, -4.0f, 0.0f,
		-4.0f, -4.0f, 1.0f,
		-5.0f, -4.0f, 1.0f,
		//back
		-5.0f, 2.0f, 0.0f,
		-5.0f, 2.0f, 1.0f,
		-4.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 1.0f,
		-5.0f, 2.0f, 1.0f,
		//right
		-4.0f, -4.0f, 0.0f,
		-4.0f, -4.0f, 1.0f,
		-4.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 1.0f,
		-4.0f, -4.0f, 1.0f,
		//left
		-5.0f, -4.0f, 0.0f,
		-5.0f, -4.0f, 1.0f,
		-5.0f, 2.0f, 0.0f,
		-5.0f, 2.0f, 0.0f,
		-5.0f, 2.0f, 1.0f,
		-5.0f, -4.0f, 1.0f,

		// (-3,3) (-2,3) (-3,0) (-2,0) block
		//bottom
		-3.0f, 3.0f, 0.0f,
		-3.0f, 0.0f, 0.0f,
		-2.0f, 0.0f, 0.0f,
		-3.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 0.0f,
		-2.0f, 0.0f, 0.0f,
		//up
		-3.0f, 3.0f, 1.0f,
		-3.0f, 0.0f, 1.0f,
		-2.0f, 0.0f, 1.0f,
		-3.0f, 3.0f, 1.0f,
		-2.0f, 3.0f, 1.0f,
		-2.0f, 0.0f, 1.0f,
		//front
		-3.0f, 0.0f, 0.0f,
		-3.0f, 0.0f, 1.0f,
		-2.0f, 0.0f, 0.0f,
		-2.0f, 0.0f, 0.0f,
		-2.0f, 0.0f, 1.0f,
		-3.0f, 0.0f, 1.0f,
		//back
		-3.0f, 3.0f, 0.0f,
		-3.0f, 3.0f, 1.0f,
		-2.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 1.0f,
		-3.0f, 3.0f, 1.0f,
		//right
		-2.0f, 0.0f, 0.0f,
		-2.0f, 0.0f, 1.0f,
		-2.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 1.0f,
		-2.0f, 0.0f, 1.0f,
		//left
		-3.0f, 0.0f, 0.0f,
		-3.0f, 0.0f, 1.0f,
		-3.0f, 3.0f, 0.0f,
		-3.0f, 3.0f, 0.0f,
		-3.0f, 3.0f, 1.0f,
		-3.0f, 0.0f, 1.0f,

		// (-2,3) (-2,2) (1,3) (1,2) block
		//bottom
		-2.0f, 3.0f, 0.0f,
		-2.0f, 2.0f, 0.0f,
		1.0f, 2.0f, 0.0f,
		-2.0f, 3.0f, 0.0f,
		1.0f, 3.0f, 0.0f,
		1.0f, 2.0f, 0.0f,
		//up
		-2.0f, 3.0f, 1.0f,
		-2.0f, 2.0f, 1.0f,
		1.0f, 2.0f, 1.0f,
		-2.0f, 3.0f, 1.0f,
		1.0f, 3.0f, 1.0f,
		1.0f, 2.0f, 1.0f,
		//front
		-2.0f, 2.0f, 0.0f,
		-2.0f, 2.0f, 1.0f,
		1.0f, 2.0f, 0.0f,
		1.0f, 2.0f, 0.0f,
		1.0f, 2.0f, 1.0f,
		-2.0f, 2.0f, 1.0f,
		//back
		-2.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 1.0f,
		1.0f, 3.0f, 0.0f,
		1.0f, 3.0f, 0.0f,
		1.0f, 3.0f, 1.0f,
		-2.0f, 3.0f, 1.0f,
		//right
		1.0f, 2.0f, 0.0f,
		1.0f, 2.0f, 1.0f,
		1.0f, 3.0f, 0.0f,
		1.0f, 3.0f, 0.0f,
		1.0f, 3.0f, 1.0f,
		1.0f, 2.0f, 1.0f,
		//left
		-2.0f, 2.0f, 0.0f,
		-2.0f, 2.0f, 1.0f,
		-2.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 0.0f,
		-2.0f, 3.0f, 1.0f,
		-2.0f, 2.0f, 1.0f,

		// (2,3) (3,3) (2,0) (3,0) block
		//bottom
		2.0f, 3.0f, 0.0f,
		2.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 0.0f,
		2.0f, 3.0f, 0.0f,
		3.0f, 3.0f, 0.0f,
		3.0f, 0.0f, 0.0f,
		//up
		2.0f, 3.0f, 1.0f,
		2.0f, 0.0f, 1.0f,
		3.0f, 0.0f, 1.0f,
		2.0f, 3.0f, 1.0f,
		3.0f, 3.0f, 1.0f,
		3.0f, 0.0f, 1.0f,
		//front
		2.0f, 0.0f, 0.0f,
		2.0f, 0.0f, 1.0f,
		3.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 1.0f,
		2.0f, 0.0f, 1.0f,
		//back
		2.0f, 3.0f, 0.0f,
		2.0f, 3.0f, 1.0f,
		3.0f, 3.0f, 0.0f,
		3.0f, 3.0f, 0.0f,
		3.0f, 3.0f, 1.0f,
		2.0f, 3.0f, 1.0f,
		//right
		3.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 1.0f,
		3.0f, 3.0f, 0.0f,
		3.0f, 3.0f, 0.0f,
		3.0f, 3.0f, 1.0f,
		3.0f, 0.0f, 1.0f,
		//left
		2.0f, 0.0f, 0.0f,
		2.0f, 0.0f, 1.0f,
		2.0f, 3.0f, 0.0f,
		2.0f, 3.0f, 0.0f,
		2.0f, 3.0f, 1.0f,
		2.0f, 0.0f, 1.0f,

		// (0,0) (1,0) (0,-2) (1,-2) block
		//bottom
		0.0f, 0.0f, 0.0f,
		0.0f, -2.0f, 0.0f,
		1.0f, -2.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, -2.0f, 0.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, -2.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		//front
		0.0f, -2.0f, 0.0f,
		0.0f, -2.0f, 1.0f,
		1.0f, -2.0f, 0.0f,
		1.0f, -2.0f, 0.0f,
		1.0f, -2.0f, 1.0f,
		0.0f, -2.0f, 1.0f,
		//back
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//right
		1.0f, -2.0f, 0.0f,
		1.0f, -2.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		//left
		0.0f, -2.0f, 0.0f,
		0.0f, -2.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, -2.0f, 1.0f,

		// (1,-1) (3,-1) (1,-2) (3,-2) block
		//bottom
		1.0f, -1.0f, 0.0f,
		1.0f, -2.0f, 0.0f,
		3.0f, -1.0f, 0.0f,
		1.0f, -2.0f, 0.0f,
		3.0f, -1.0f, 0.0f,
		3.0f, -2.0f, 0.0f,
		//up
		1.0f, -1.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		3.0f, -1.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		3.0f, -1.0f, 1.0f,
		3.0f, -2.0f, 1.0f,
		//front
		1.0f, -2.0f, 0.0f,
		1.0f, -2.0f, 1.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		//back
		1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		3.0f, -1.0f, 0.0f,
		3.0f, -1.0f, 0.0f,
		3.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		//right
		3.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 1.0f,
		3.0f, -1.0f, 0.0f,
		3.0f, -1.0f, 0.0f,
		3.0f, -1.0f, 1.0f,
		3.0f, -2.0f, 1.0f,
		//left
		1.0f, -2.0f, 0.0f,
		1.0f, -2.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -2.0f, 1.0f,

		// (2,-2) (3,-2) (2,-3) (3,-3) block
		//bottom
		2.0f, -2.0f, 0.0f,
		2.0f, -3.0f, 0.0f,
		3.0f, -3.0f, 0.0f,
		2.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, -3.0f, 0.0f,
		//up
		2.0f, -2.0f, 1.0f,
		2.0f, -3.0f, 1.0f,
		3.0f, -3.0f, 1.0f,
		2.0f, -2.0f, 1.0f,
		3.0f, -2.0f, 1.0f,
		3.0f, -3.0f, 1.0f,
		//front
		2.0f, -3.0f, 0.0f,
		2.0f, -3.0f, 1.0f,
		3.0f, -3.0f, 0.0f,
		3.0f, -3.0f, 0.0f,
		3.0f, -3.0f, 1.0f,
		2.0f, -3.0f, 1.0f,
		//back
		2.0f, -2.0f, 0.0f,
		2.0f, -2.0f, 1.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 1.0f,
		2.0f, -2.0f, 1.0f,
		//right
		3.0f, -3.0f, 0.0f,
		3.0f, -3.0f, 1.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, -2.0f, 1.0f,
		3.0f, -3.0f, 1.0f,
		//left
		2.0f, -3.0f, 0.0f,
		2.0f, -3.0f, 1.0f,
		2.0f, -2.0f, 0.0f,
		2.0f, -2.0f, 0.0f,
		2.0f, -2.0f, 1.0f,
		2.0f, -3.0f, 1.0f,

		// (-3,-1) (-1,-1) (-3,-2) (-1,-2) block
		//bottom
		-3.0f, -1.0f, 0.0f,
		-3.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		-3.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		//up
		-3.0f, -1.0f, 1.0f,
		-3.0f, -2.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		-3.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		//front
		-3.0f, -2.0f, 0.0f,
		-3.0f, -2.0f, 1.0f,
		-1.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 1.0f,
		-3.0f, -2.0f, 1.0f,
		//back
		-3.0f, -1.0f, 0.0f,
		-3.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		-3.0f, -1.0f, 1.0f,
		//right
		-1.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		//left
		-3.0f, -2.0f, 0.0f,
		-3.0f, -2.0f, 1.0f,
		-3.0f, -1.0f, 0.0f,
		-3.0f, -1.0f, 0.0f,
		-3.0f, -1.0f, 1.0f,
		-3.0f, -2.0f, 1.0f,

		// (-3,-3) (-2,-3) (-3,-4) (-2,-4) block
		//bottom
		-3.0f, -3.0f, 0.0f,
		-3.0f, -4.0f, 0.0f,
		-2.0f, -4.0f, 0.0f,
		-3.0f, -3.0f, 0.0f,
		-2.0f, -3.0f, 0.0f,
		-2.0f, -4.0f, 0.0f,
		//up
		-3.0f, -3.0f, 1.0f,
		-3.0f, -4.0f, 1.0f,
		-2.0f, -4.0f, 1.0f,
		-3.0f, -3.0f, 1.0f,
		-2.0f, -3.0f, 1.0f,
		-2.0f, -4.0f, 1.0f,
		//front
		-3.0f, -4.0f, 0.0f,
		-3.0f, -4.0f, 1.0f,
		-2.0f, -4.0f, 0.0f,
		-2.0f, -4.0f, 0.0f,
		-2.0f, -4.0f, 1.0f,
		-3.0f, -4.0f, 1.0f,
		//back
		-3.0f, -3.0f, 0.0f,
		-3.0f, -3.0f, 1.0f,
		-2.0f, -3.0f, 0.0f,
		-2.0f, -3.0f, 0.0f,
		-2.0f, -3.0f, 1.0f,
		-3.0f, -3.0f, 1.0f,
		//right
		-2.0f, -4.0f, 0.0f,
		-2.0f, -4.0f, 1.0f,
		-2.0f, -3.0f, 0.0f,
		-2.0f, -3.0f, 0.0f,
		-2.0f, -3.0f, 1.0f,
		-2.0f, -4.0f, 1.0f,
		//left
		-3.0f, -4.0f, 0.0f,
		-3.0f, -4.0f, 1.0f,
		-3.0f, -3.0f, 0.0f,
		-3.0f, -3.0f, 0.0f,
		-3.0f, -3.0f, 1.0f,
		-3.0f, -4.0f, 1.0f,

		// (-1,-3) (1,-3) (-1,-4) (1,-4) block
		//bottom
		-1.0f, -3.0f, 0.0f,
		-1.0f, -4.0f, 0.0f,
		1.0f, -4.0f, 0.0f,
		-1.0f, -3.0f, 0.0f,
		1.0f, -3.0f, 0.0f,
		1.0f, -4.0f, 0.0f,
		//up
		-1.0f, -3.0f, 1.0f,
		-1.0f, -4.0f, 1.0f,
		1.0f, -4.0f, 1.0f,
		-1.0f, -3.0f, 1.0f,
		1.0f, -3.0f, 1.0f,
		1.0f, -4.0f, 1.0f,
		//front
		-1.0f, -4.0f, 0.0f,
		-1.0f, -4.0f, 1.0f,
		1.0f, -4.0f, 0.0f,
		1.0f, -4.0f, 0.0f,
		1.0f, -4.0f, 1.0f,
		-1.0f, -4.0f, 1.0f,
		//back
		-1.0f, -3.0f, 0.0f,
		-1.0f, -3.0f, 1.0f,
		1.0f, -3.0f, 0.0f,
		1.0f, -3.0f, 0.0f,
		1.0f, -3.0f, 1.0f,
		-1.0f, -3.0f, 1.0f,
		//right
		1.0f, -4.0f, 0.0f,
		1.0f, -4.0f, 1.0f,
		1.0f, -3.0f, 0.0f,
		1.0f, -3.0f, 0.0f,
		1.0f, -3.0f, 1.0f,
		1.0f, -4.0f, 1.0f,
		//left
		-1.0f, -4.0f, 0.0f,
		-1.0f, -4.0f, 1.0f,
		-1.0f, -3.0f, 0.0f,
		-1.0f, -3.0f, 0.0f,
		-1.0f, -3.0f, 1.0f,
		-1.0f, -4.0f, 1.0f,

		// (-1,1) (-1,0) (1,1) (1,0) block
		//bottom
		-1.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//up
		-1.0f, 1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		//front
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,
		//back
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f

	};

	GLfloat a = 0.4f;
	static const GLfloat color[] = {
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,

		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a,
		0.0f, 0.0f, 1.0f, a
	};

	static GLfloat character_buffer[] = {
		// (-4.75,2.25) (-4.25,2.25) (-4.75,2.75) (-4.25,2.75) block
		//bottom
		-4.75f, 2.75f, 0.25f, //xmin,ymax point
		-4.75f, 2.25f, 0.25f,
		-4.25f, 2.75f, 0.25f,
		-4.75f, 2.25f, 0.25f,
		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.25f, 0.25f,
		//up
		-4.75f, 2.75f, 0.75f,
		-4.75f, 2.25f, 0.75f,
		-4.25f, 2.75f, 0.75f,
		-4.75f, 2.25f, 0.75f,
		-4.25f, 2.75f, 0.75f,
		-4.25f, 2.25f, 0.75f,
		//front
		-4.75f, 2.25f, 0.25f,
		-4.75f, 2.25f, 0.75f,
		-4.25f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.75f,
		-4.75f, 2.25f, 0.75f,
		//back
		-4.75f, 2.75f, 0.25f,
		-4.75f, 2.75f, 0.75f,
		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.75f, 0.75f,
		-4.75f, 2.75f, 0.75f,
		//right
		-4.25f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.75f,
		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.75f, 0.75f,
		-4.25f, 2.25f, 0.75f,
		//left
		-4.75f, 2.25f, 0.25f,
		-4.75f, 2.25f, 0.75f,
		-4.75f, 2.75f, 0.25f,
		-4.75f, 2.75f, 0.25f,
		-4.75f, 2.75f, 0.75f,
		-4.75f, 2.25f, 0.75f

	};

	static const GLfloat color2[] = {
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a,
		1.0f, 1.0f, 0.0f, a
	};

	GLuint characterbuffer;
	glGenBuffers(1, &characterbuffer);

	GLuint colorbuffer2;
	glGenBuffers(1, &colorbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color2), color2, GL_STATIC_DRAW);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(maze), maze, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);


	bool keyLpressed = false;
	bool keyKpressed = false;
	bool keyIpressed = false;
	bool keyJpressed = false;

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		camera_function();

		ProjectionMatrix = getProjectionMatrix();
		ViewMatrix = getViewMatrix();
		ZoomMatrix = ViewMatrix;
		glm::mat4 Model = glm::mat4(1.0f);


		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Draw triangles 
		glDrawArrays(GL_TRIANGLES, 0, 16 * 36);


		// character 
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, characterbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(character_buffer), character_buffer, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// color character
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//draw character
		glDrawArrays(GL_TRIANGLES, 0, 36);


		glDisableVertexAttribArray(0);


		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !keyLpressed) { // right move
			Sleep(50);
			moveSquare(character_buffer, map, 1);
			Sleep(50);
			keyLpressed = true;

		}
		else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
			keyLpressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !keyJpressed) { // left move
			Sleep(50);
			moveSquare(character_buffer, map, 2);
			Sleep(50);
			keyJpressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE) {
			keyJpressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !keyIpressed) { // up move
			Sleep(50);
			moveSquare(character_buffer, map, 3);
			Sleep(50);
			keyIpressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
			keyIpressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !keyKpressed) { // down move
			Sleep(50);
			moveSquare(character_buffer, map, 4);
			Sleep(50);
			keyKpressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) {
			keyKpressed = false;
		}


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}




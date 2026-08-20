//********************************
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1Γ της OpenGL
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

//#include <mmsystem.h>

// include stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
void spawnTreasure(GLfloat* treasureConst, GLfloat* treasure, GLfloat map[10][10], GLfloat* player);
bool checkIfPlayerTouched(GLfloat* treasure, GLfloat* player);
void reduceSizeOfTreasure(GLfloat* treasure);
void light_movement_function();


////////////////////////////////////////////// <summary>
/// Add camera function here
/// </summary>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::mat4 ZoomMatrix;
glm::mat4 tempMatrix;
glm::mat4 panningMatrix;

glm::vec3 lightPos = glm::vec3(10.0, 8.0, 4.0);

bool spawning = true;
int chooseTexture = 0;

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
	else if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) { // zoom in
		ZoomMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.025f));
		tempMatrix = ZoomMatrix * ViewMatrix;
		if ((0 - tempMatrix[3][2]) >= 0.1) {
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
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { //pan x left
		panningMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.025f, 0, 0));
		ViewMatrix = panningMatrix * ViewMatrix;
	}
	else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) { //pan x right
		panningMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.025f, 0, 0));
		ViewMatrix = panningMatrix * ViewMatrix;
	}
	else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { //pan y right
		panningMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.025f, 0));
		ViewMatrix = panningMatrix * ViewMatrix;
	}
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) { //pan y left
		panningMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.025f, 0));
		ViewMatrix = panningMatrix * ViewMatrix;
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
	
	GLfloat* temp = (GLfloat*)malloc(2 * sizeof(GLfloat));
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

GLfloat* upscaleTreasure(GLfloat a[]) {
	//upscale bottom side
	
	GLfloat* temp = (GLfloat*)malloc(2 * sizeof(GLfloat));
	GLfloat x = a[0];
	GLfloat y = a[1];
	//x
	if (a[0] < 0) {
		temp[0] = x - 0.1f;
	}
	else {
		temp[0] = x + 0.1f; 
	}
	//y
	if (a[1] < 0) {
		temp[1] = y - 0.1f; 
	}
	else {
		temp[1] = y + 0.1f;
	}
	return temp;
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

	GLfloat* temp;
	temp = upscalePlayer(player);

	int row = 0;
	int col = 0;

	//transform player coords to map cords

	//2nd quarter
	if (temp[0] < 0 && temp[1] > 0) {
		row = -(temp[1] - 5);
		col = temp[0] + 5;
		if (map[row][col] == 1.0f) {
			free(temp);
			return false;
		}
	}
	//1st quarter
	else if (temp[0] > 0 && temp[1] > 0) {
		row = -temp[1] + 5;
		col = temp[0] + 5;
		
		if (map[row][col] == 1.0f) {
			free(temp);
			return false;
		}
	}
	//4th quarter
	else if (temp[0] > 0 && temp[1] < 0) {
		row = -temp[1] + 5;
		col = temp[0] + 5;
		if (map[row][col] == 1.0f) {
			free(temp);
			return false;
		}
	}
	//3rd quarter
	else if (temp[0] < 0 && temp[1] < 0) {
		row = -temp[1] + 5;
		col = temp[0] + 5;
		if (map[row][col] == 1.0f) {
			free(temp);
			return false;
		}
	}
	free(temp);
	return true;

}


bool checkTreasureMove(GLfloat map[10][10], GLfloat* treasureNew, GLfloat* treasureOld, GLfloat* player) {

	//check bottom side only
	

	//check if player is out of map
	for (int i = 0; i < 18; i = i + 3) {
		if (treasureNew[i] > 5 || treasureNew[i] < -5) {
			return false;
		}
	}

	//check if it spawns on the previous position
	if (treasureNew[0] == treasureOld[0] && treasureNew[1] == treasureOld[1]) {
		return false;
	}


	GLfloat* tempPlayer = upscalePlayer(player);
	GLfloat* tempTreasure = upscaleTreasure(treasureNew);

	

		//check if it spawns on the player
	if (abs(tempPlayer[0] - tempTreasure[0]) < 1 && abs(tempPlayer[1] - tempTreasure[1]) < 1) {
		free(tempPlayer);
		free(tempTreasure);
		return false;
	}



	int row = 0;
	int col = 0;


	//transform player coords to map cords

	//2nd quarter
	if (tempTreasure[0] < 0.0f && tempTreasure[1] > 0.0f) {
		
		row = -(tempTreasure[1] - 5);
		col = tempTreasure[0] + 5;
		
		if (map[row][col] == 1.0f) {
			free(tempPlayer);
			free(tempTreasure);
			return false;
		}
	}
	//1st quarter
	else if (tempTreasure[0] > 0.0f && tempTreasure[1] > 0.0f) {
		
		row = -tempTreasure[1] + 5;
		col = tempTreasure[0] + 5;
		
		if (map[row][col] == 1.0f) {
			free(tempPlayer);
			free(tempTreasure);
			return false;
		}
	}
	//4th quarter
	else if (tempTreasure[0] > 0.0f && tempTreasure[1] < 0.0f) {
		
		row = -tempTreasure[1] + 5;
		col = tempTreasure[0] + 5;
		
		if (map[row][col] == 1.0f) {
			free(tempPlayer);
			free(tempTreasure);
			return false;
		}
	}
	//3rd quarter
	else if (tempTreasure[0] < 0.0f && tempTreasure[1] < 0.0f) {
		
		row = -tempTreasure[1] + 5;
		col = tempTreasure[0] + 5;

		if (map[row][col] == 1.0f) {
			free(tempPlayer);
			free(tempTreasure);
			return false;
		}
	}
	free(tempPlayer);
	free(tempTreasure);
	return true;

}


void spawnTreasure(GLfloat* treasureStartingPoint, GLfloat* treasure, GLfloat map[10][10], GLfloat* player) {

	if (spawning == true) {

		srand((unsigned)time(NULL));

		// Toggle between textures
		int chooseRandomTexture = 1 + (rand() % 4);
		chooseTexture = chooseRandomTexture;

		
		bool valid = false;
		int randomX;
		int randomY;

		while (valid == false) {
			randomX = -5 + (rand() % 10);
			
			randomY = -5 + (rand() % 10);
			

			GLfloat temp[108];
			for (int i = 0; i < 108; i++) {
				temp[i] = treasure[i];
			}

			//x
			for (int i = 0; i < 108; i += 3) {

				temp[i] = treasureStartingPoint[i] + (float)randomX;
			}
			//y
			for (int i = 1; i < 108; i += 3) {

				temp[i] = treasureStartingPoint[i] + (float)randomY;
			}

			
			if (checkTreasureMove(map, temp, treasure, player)) {
				valid = true;

				for (int i = 0; i < 108; i++) {
					treasure[i] = temp[i];
				}

			}
		}
	}
}

bool checkIfPlayerTouched(GLfloat* treasure, GLfloat* player) {
	GLfloat* tempPlayer = upscalePlayer(player);
	GLfloat* tempTreasure = upscaleTreasure(treasure);

	bool playersTouch = false;
	if (abs(player[0] - treasure[0]) <= 0.5 && abs(player[1] - treasure[1]) <= 0.5) {//meaning that it touched the treasure
		playersTouch = true;
	}
	free(tempPlayer);
	free(tempTreasure);
	return playersTouch;
}

void reduceSizeOfTreasure(GLfloat* treasure_buffer) {
	// Find the bounds of the treasure
	GLfloat xmin = treasure_buffer[0], xmax = treasure_buffer[0];
	GLfloat ymin = treasure_buffer[1], ymax = treasure_buffer[1];
	GLfloat zmin = treasure_buffer[2], zmax = treasure_buffer[2];

	for (int i = 0; i < 36 * 3; i += 3) {
		if (treasure_buffer[i] < xmin) xmin = treasure_buffer[i];
		if (treasure_buffer[i] > xmax) xmax = treasure_buffer[i];
		if (treasure_buffer[i + 1] < ymin) ymin = treasure_buffer[i + 1];
		if (treasure_buffer[i + 1] > ymax) ymax = treasure_buffer[i + 1];
		if (treasure_buffer[i + 2] < zmin) zmin = treasure_buffer[i + 2];
		if (treasure_buffer[i + 2] > zmax) zmax = treasure_buffer[i + 2];
	}

	// Calculate the center
	GLfloat xcenter = (xmin + xmax) / 2.0f;
	GLfloat ycenter = (ymin + ymax) / 2.0f;
	GLfloat zcenter = (zmin + zmax) / 2.0f;

	// Scale each vertex toward the center
	for (int i = 0; i < 36 * 3; i += 3) {
		treasure_buffer[i] = xcenter + 0.5f * (treasure_buffer[i] - xcenter);
		treasure_buffer[i + 1] = ycenter + 0.5f * (treasure_buffer[i + 1] - ycenter);
		treasure_buffer[i + 2] = zcenter + 0.5f * (treasure_buffer[i + 2] - zcenter);
	}
}

void light_movement_function()
{
	float lightSpeed = 0.1f; // Speed of light movement

	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) { // Move light up
		lightPos.y += lightSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) { // Move light down
		lightPos.y -= lightSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) { // Move light left
		lightPos.x -= lightSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) { // Move light right
		lightPos.x += lightSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) { // Move light forward
		lightPos.z += lightSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) { // Move light backward
		lightPos.z -= lightSpeed;
	}
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

	window = glfwCreateWindow(950, 950, u8"Άσκηση 1Γ - 2024- Κυνήγι Θυσαυρού", NULL, NULL);


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

	GLuint programID = LoadShaders("P1CVertexShader.vertexshader", "P1CFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

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

	static GLfloat maze_normals[] = {
		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f

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

	static GLfloat treasure_const[] = {

		//bottom
		0.1f, 0.9f, 0.10f, //xmin,ymax point
		0.1f, 0.1f, 0.10f,
		0.9f, 0.9f, 0.10f,
		0.1f, 0.1f, 0.10f,
		0.9f, 0.9f, 0.10f,
		0.9f, 0.1f, 0.10f,
		//up
		0.1f, 0.9f, 0.90f,
		0.1f, 0.1f, 0.90f,
		0.9f, 0.9f, 0.90f,
		0.1f, 0.1f, 0.90f,
		0.9f, 0.9f, 0.90f,
		0.9f, 0.1f, 0.90f,
		//front
		0.1f, 0.1f, 0.10f,
		0.1f, 0.1f, 0.90f,
		0.9f, 0.1f, 0.10f,
		0.9f, 0.1f, 0.10f,
		0.9f, 0.1f, 0.90f,
		0.1f, 0.1f, 0.90f,
		//back
		0.1f, 0.9f, 0.10f,
		0.1f, 0.9f, 0.90f,
		0.9f, 0.9f, 0.10f,
		0.9f, 0.9f, 0.10f,
		0.9f, 0.9f, 0.90f,
		0.1f, 0.9f, 0.90f,
		//right
		0.9f, 0.1f, 0.10f,
		0.9f, 0.1f, 0.90f,
		0.9f, 0.9f, 0.10f,
		0.9f, 0.9f, 0.10f,
		0.9f, 0.9f, 0.90f,
		0.9f, 0.1f, 0.90f,
		//left
		0.1f, 0.1f, 0.10f,
		0.1f, 0.1f, 0.90f,
		0.1f, 0.9f, 0.10f,
		0.1f, 0.9f, 0.10f,
		0.1f, 0.9f, 0.90f,
		0.1f, 0.1f, 0.90f

	};

	static GLfloat treasure_buffer[] = {

		//bottom
		0.1f, 3.9f, 0.10f, //xmin,ymax point
		0.1f, 3.1f, 0.10f,
		0.9f, 3.9f, 0.10f,
		0.1f, 3.1f, 0.10f,
		0.9f, 3.9f, 0.10f,
		0.9f, 3.1f, 0.10f,
		//up
		0.1f, 3.9f, 0.90f,
		0.1f, 3.1f, 0.90f,
		0.9f, 3.9f, 0.90f,
		0.1f, 3.1f, 0.90f,
		0.9f, 3.9f, 0.90f,
		0.9f, 3.1f, 0.90f,
		//front
		0.1f, 3.1f, 0.10f,
		0.1f, 3.1f, 0.90f,
		0.9f, 3.1f, 0.10f,
		0.9f, 3.1f, 0.10f,
		0.9f, 3.1f, 0.90f,
		0.1f, 3.1f, 0.90f,
		//back
		0.1f, 3.9f, 0.10f,
		0.1f, 3.9f, 0.90f,
		0.9f, 3.9f, 0.10f,
		0.9f, 3.9f, 0.10f,
		0.9f, 3.9f, 0.90f,
		0.1f, 3.9f, 0.90f,
		//right
		0.9f, 3.1f, 0.10f,
		0.9f, 3.1f, 0.90f,
		0.9f, 3.9f, 0.10f,
		0.9f, 3.9f, 0.10f,
		0.9f, 3.9f, 0.90f,
		0.9f, 3.1f, 0.90f,
		//left
		0.1f, 3.1f, 0.10f,
		0.1f, 3.1f, 0.90f,
		0.1f, 3.9f, 0.10f,
		0.1f, 3.9f, 0.10f,
		0.1f, 3.9f, 0.90f,
		0.1f, 3.1f, 0.90f

	};

	static GLfloat g_uvbuffer_data[] = {
		//αντιστοιχιζουμε το καθε τριγωνο της καθε πλευρας
		//με τα σωστα σημεια της εικονας

		//first block - bottom
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		//second block - up
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		//third block - front
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//fourth block - back
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//fifth block - right
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//sixth block - left
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	static GLfloat normals[] = {

		//bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//up
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//front
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//back
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

	};

	int width;
	int height;
	int numColCh;
	unsigned char* dataImage = stbi_load("coins.jpg", &width, &height, &numColCh, 0);
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	//Bind the newly created texture
	
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	stbi_image_free(dataImage);
	// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	// second photo
	int width2;
	int height2;
	int numColCh2;
	unsigned char* dataImage2 = stbi_load("coins2.jpeg", &width2, &height2, &numColCh2, 0);
	// Create one OpenGL texture
	GLuint textureID2;
	glGenTextures(1, &textureID2);

	//Bind the newly created texture
	
	glBindTexture(GL_TEXTURE_2D, textureID2);

	//Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, dataImage2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	stbi_image_free(dataImage2);
	// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	// third photo
	int width3;
	int height3;
	int numColCh3;
	unsigned char* dataImage3 = stbi_load("coins3.jpg", &width3, &height3, &numColCh3, 0);

	// Create one OpenGL texture
	GLuint textureID3;
	glGenTextures(1, &textureID3);

	//Bind the newly created texture
	
	glBindTexture(GL_TEXTURE_2D, textureID3);

	//Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, dataImage3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	stbi_image_free(dataImage3);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	GLuint mazenormalbuffer;
	glGenBuffers(1, &mazenormalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mazenormalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(maze_normals), maze_normals, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uvbuffer_data), g_uvbuffer_data, GL_STATIC_DRAW);

	GLuint treasurebuffer;
	glGenBuffers(1, &treasurebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, treasurebuffer);

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

	//spawn delay
	static float treasureTimer = 0.0f; // Accumulates elapsed time
	float deltaTime = 1.0f;
	float spawnInterval = 500.0f;

	//movement delay
	static float movementTimer = 0.0f; // Accumulates elapsed time
	//float deltaTime = 1.0f;
	float movementInterval = 500.0f;

	//collect treasure delay
	bool treasureCollected = false;
	float despawnTimer = 0.0f;
	float treasureDespawnInterval = 350.0f;

	
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");


	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);


		camera_function();

		ProjectionMatrix = getProjectionMatrix();
		ViewMatrix = getViewMatrix();
		


		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		
		light_movement_function();
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// maze
		glUniform1i(glGetUniformLocation(programID, "useTexture"), GL_FALSE);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// color maze
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//maze normals
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, mazenormalbuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Draw maze
		glDrawArrays(GL_TRIANGLES, 0, 16 * 36);


		// character 
		glUniform1i(glGetUniformLocation(programID, "useTexture"), GL_FALSE);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, characterbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(character_buffer), character_buffer, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// color character
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//character normals
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//draw character
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//treasure
		glUniform1i(glGetUniformLocation(programID, "useTexture"), GL_TRUE);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, treasurebuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(treasure_buffer), treasure_buffer, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//treasure normals
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		
		// texture
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		if (chooseTexture == 1) {
			// Use the first texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
		}
		else if (chooseTexture == 2) {
			// Use the second texture
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 1);
		}
		else if (chooseTexture == 3) {
			// Use the second texture
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureID3);
			glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 2);
		}
		

		// Draw treasure 


		glDrawArrays(GL_TRIANGLES, 0, 36);



		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);


		if (treasureCollected) {
			despawnTimer += deltaTime;
			if (despawnTimer >= treasureDespawnInterval) {
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				for (int i = 0; i < 108; i++) {
					treasure_buffer[i] = 0.0f;
				}
			}
		}


		treasureTimer += deltaTime;       // Add the time between frames
		if (treasureTimer >= spawnInterval) {
			spawnTreasure(treasure_const, treasure_buffer, map, character_buffer);
			treasureTimer = 0.0f;         // Reset the timer
		}


		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !keyLpressed) { // right move
			while (movementTimer < movementInterval) {
				movementTimer += deltaTime;
			}
			moveSquare(character_buffer, map, 1);
			movementTimer = 0.0f;
			keyLpressed = true;

		}
		else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
			keyLpressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !keyJpressed) { // left move
			while (movementTimer < movementInterval) {
				movementTimer += deltaTime;
			}
			moveSquare(character_buffer, map, 2);
			movementTimer = 0.0f;
			keyJpressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE) {
			keyJpressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !keyIpressed) { // up move

			while (movementTimer < movementInterval) {
				movementTimer += deltaTime;
			}
			moveSquare(character_buffer, map, 3);
			movementTimer = 0.0f;
			keyIpressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
			keyIpressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !keyKpressed) { // down move
			while (movementTimer < movementInterval) {
				movementTimer += deltaTime;
			}
			moveSquare(character_buffer, map, 4);
			movementTimer = 0.0f;
			keyKpressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) {
			keyKpressed = false;
		}


		if (checkIfPlayerTouched(treasure_buffer, character_buffer) == true && treasureCollected == false) {
			spawning = false;
			treasureCollected = true;
			PlaySound(TEXT("coin.wav"), NULL, SND_FILENAME | SND_ASYNC);
			glClearColor(1.0f, 0.5f, 0.0f, 0.0f);
			reduceSizeOfTreasure(treasure_buffer);

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


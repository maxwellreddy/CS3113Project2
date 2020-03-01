// Maxwell Reddy
// CS 3113
// Assignment 2

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window * displayWindow;
bool gameIsRunning = true;
bool gameStarted = false;

GLuint textureID;
GLuint textureID2;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix2, modelMatrix3, projectionMatrix;
//Set position vectors
glm::vec3 paddle1Position = glm::vec3(-4.8f, 0, 0);
glm::vec3 paddle2Position= glm::vec3(4.8f, 0, 0);
glm::vec3 ballPosition = glm::vec3(0, 0, 0);
//Set movement vectors11
glm::vec3 paddle1Movement, paddle2Movement = glm::vec3(0, 0, 0);
glm::vec3 ballMovement = glm::vec3(0.0f, 0.0f, 0.0f);
float ballRotate = 0.0f;

float lastTicks = 0.0f;

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Project 2: Kirby Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix2 = glm::mat4(1.0f);
	modelMatrix3 = glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_BLEND);

	// Good setting for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textureID = LoadTexture("Ball.png");
	textureID2 = LoadTexture("Paddle.png");
}

void ProcessInput() {
	paddle1Movement, paddle2Movement = glm::vec3(0, 0, 0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
				// Move the player left
				break;

			case SDLK_RIGHT:
				// Move the player right
				break;

			case SDLK_SPACE:
				// Some sort of action
				break;
			}
			break; // SDL_KEYDOWN
		}
	}
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	// Wait for the spacebar to be pressed before the game starts
	if (gameStarted == false) {
		if (keys[SDL_SCANCODE_SPACE]) {
			ballMovement = glm::vec3(-2.0f, 1.0f, 0.0f);
			gameStarted = true;
		}
	}

	// Code to move the paddles
	else if ((ballPosition.x < -5.0f || ballPosition.x > 5.0f) == false) {
		if (keys[SDL_SCANCODE_W]) {
			if (paddle1Position.y < 2.8f) {
				paddle1Movement.y = 2.5f;
			}
			else {
				paddle1Movement.y = 0.0f;
			}
		}
		else if (keys[SDL_SCANCODE_S]) {
			if (paddle1Position.y > -2.8f) {
				paddle1Movement.y = -2.5f;
			}
			else {
				paddle1Movement.y = 0.0f;
			}
		}		else {			paddle1Movement.y = 0.0f;		}		if (keys[SDL_SCANCODE_UP]) {
			if (paddle2Position.y < 2.8f) {
				paddle2Movement.y = 2.5f;
			}
			else {
				paddle2Movement.y = 0.0f;
			}
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			if (paddle2Position.y > -2.8f) {
				paddle2Movement.y = -2.5f;
			}
			else {
				paddle2Movement.y = 0.0f;
			}
		}		else {			paddle2Movement.y = 0.0f;		}
	}

	// If someone wins, the game waits for spacebar to be pressed before restarting
	else {
		paddle1Movement.y = 0.0f;
		paddle2Movement.y = 0.0f;

		if (keys[SDL_SCANCODE_SPACE]) {
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			if (ballPosition.x < -5.0f) {
				ballMovement = glm::vec3(2.0f, 1.0f, 0.0f);
			}
			else if (ballPosition.x > 5.0f) {
				ballMovement = glm::vec3(-2.0f, 1.0f, 0.0f);
			}
			ballPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			paddle1Position = glm::vec3(-4.8f, 0, 0);
			paddle2Position = glm::vec3(4.8f, 0, 0);
		}
	}
}

void Update() { 
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	paddle1Position += paddle1Movement * deltaTime;
	paddle2Position += paddle2Movement * deltaTime;
	if ((ballPosition.x < -5.0f || ballPosition.x > 5.0f) == false) {
		ballRotate += 90.0f * deltaTime;
	}
	else {
		ballRotate += 0.0f;
	}

	// Ball transformations

	ballPosition += ballMovement * deltaTime;

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, ballPosition);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(ballRotate), glm::vec3(0.0f, 0.0f, 1.0f));
	
	// Left paddle transformations
	modelMatrix2 = glm::mat4(1.0f);
	modelMatrix2 = glm::translate(modelMatrix2, paddle1Position);
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(2.0f, 2.0f, 1.0f));

	// Right paddle transformations
	modelMatrix3 = glm::mat4(1.0f);
	modelMatrix3 = glm::translate(modelMatrix3, paddle2Position);
	modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(2.0f, 2.0f, 1.0f));

	// Collision detection
	if (ballMovement.x < 0.0f) {
		float xDist = fabs(ballPosition.x - paddle1Position.x) - 0.55f;
		float yDist = fabs(ballPosition.y - paddle1Position.y) - 1.5f;

		if (xDist < 0 && yDist < 0) {
			ballMovement.x = 2.0f;
		}
	}

	else if (ballMovement.x > 0.0f) {
		float xDist = fabs(ballPosition.x - paddle2Position.x) - 0.55f;
		float yDist = fabs(ballPosition.y - paddle2Position.y) - 1.5f;
		
		if (xDist < 0 && yDist < 0) {
			ballMovement.x = -2.0f;
		}
	}

	if (ballPosition.y <= -3.2f) {
		ballMovement.y = 1.0f;
	}
	else if (ballPosition.y >= 3.2f) {
		ballMovement.y = -1.0f;
	}
	
	// Screen turns red and objects freeze when someone wins
	if (ballPosition.x < -5.0f || ballPosition.x > 5.0f) {
		ballMovement.x = 0;
		ballMovement.y = 0;
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	}
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	program.SetModelMatrix(modelMatrix);
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	program.SetModelMatrix(modelMatrix2);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	program.SetModelMatrix(modelMatrix3);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}
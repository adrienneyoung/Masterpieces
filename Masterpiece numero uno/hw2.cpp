#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <iostream>
#include <vector>
#include <string>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}

bool collision(float r[], float b[]){
	//Collision
	//Paddle vs ball
	//A
	if ((r[0] - .25) > (b[2] - .5)){
		//left larger than right
		return false;
	}
	if ((r[2] - .75) < (b[0] - .5)){
		//right smaller than left
		return false;
	}
	if ((r[1] - .25) > (b[5] - .5)){
		//bottom higher than top
		return false;
	}
	if ((r[5] - .75) < (b[1] - .5)){
		//top lower than bottom
		return false;
	}
	return true;
}
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, });         texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size, });
	}     glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void setup(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 600, 360);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);

}

void processEvents(){
	SDL_Event event;



}

void update(){

}
void render(){

}

int main(int argc, char *argv[])
{
	setup();

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glUseProgram(program.programID);

	Matrix modelMatrix;
	Matrix modelMatrixc;
	Matrix viewMatrix;

	GLuint paddle1 = LoadTexture("sword.png"); //bg
	GLuint paddle2 = LoadTexture("sword.png"); //creeper
	GLuint ball = LoadTexture("onigiri.png");//kick
	GLuint font = LoadTexture("font1.png");
	GLuint nada = LoadTexture("bun.png");

	float test[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	float verticesP1[] = { -3.0f, -1.0f, -2.5f, -1.0f, -2.5f, 0.5f, -3.0f, -1.0f, -2.5f, 0.5f, -3.0f, 0.5f };
	float verticesP2[] = { 3.0f, -1.0f, 3.5f, -1.0f, 3.5f, 0.5f, 3.0f, -1.0f, 3.5f, 0.5f, 3.0f, 0.5f };
	float verticesball[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
	float startPos[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	float lastFrameTicks = 0.0f;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	bool done = false;
	float speed = 2.0f;
	float ballSpeed = 5.0f;
	//int angle = 1;
	float angle = rand() % 360;
	bool leftPaddleCollide = false;
	bool rightPaddleCollide = false;


	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	SDL_Event event;
	//int winner = 0;
	while (!done) {

		glClear(GL_COLOR_BUFFER_BIT);

		processEvents();
		update();
		render();

		program.setModelMatrix(modelMatrix);
		program.setViewMatrix(viewMatrix);
		program.setProjectionMatrix(projectionMatrix);

		glBindTexture(GL_TEXTURE_2D, nada);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, test);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);


		glBindTexture(GL_TEXTURE_2D, paddle1);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesP1);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);

		glBindTexture(GL_TEXTURE_2D, paddle2);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesP2);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);

		glBindTexture(GL_TEXTURE_2D, ball);
		program.setModelMatrix(modelMatrixc);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesball);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);


		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		////PADDLE ONE
		//if (keys[SDL_SCANCODE_W]) {   
		//	if (2.5*elapsed + verticesP1[5] <= 2.0){
		//		for (int i = 0; i < 12; i++){
		//			if (i % 2 != 0){
		//				verticesP1[i] += 2.5 * elapsed;
		//			}
		//		}
		//	}
		//}
		//else if (keys[SDL_SCANCODE_S]) {
		//	if (verticesP1[1] - 2.5*elapsed >= -2.0){
		//		for (int i = 0; i < 12; i++){
		//			if (i % 2 != 0){
		//				verticesP1[i] -= 2.5* elapsed;
		//			}
		//		}
		//	}
		//}

		////PADDLE TWO
		//if (keys[SDL_SCANCODE_UP]) {
		//	if (2.5*elapsed + verticesP2[5] <= 2.0){
		//		for (int i = 0; i < 12; i++){
		//			if (i % 2 != 0){
		//				verticesP2[i] += 2.5 * elapsed;
		//			}
		//		}
		//	}
		//}
		//else if (keys[SDL_SCANCODE_DOWN]) {
		//	if (verticesP2[1] - 2.5*elapsed >= -2.0){
		//		for (int i = 0; i < 12; i++){
		//			if (i % 2 != 0){
		//				verticesP2[i] -= 2.5* elapsed;
		//			}
		//		}
		//	}
		//}


		//PADDLE ONE
		if (keys[SDL_SCANCODE_W]) {
			if (speed*elapsed + verticesP1[5] <= 2.0f){
				for (int i = 0; i < 12; i++){
					if (i % 2 != 0){
						verticesP1[i] += speed * elapsed;
					}
				}
			}
		}
		else if (keys[SDL_SCANCODE_S]) {
			if (verticesP1[1] - speed*elapsed >= -2.0f){
				for (int i = 0; i < 12; i++){
					if (i % 2 != 0){
						verticesP1[i] -= speed* elapsed;
					}
				}
			}
		}

		//PADDLE TWO
		if (keys[SDL_SCANCODE_UP]) {
			if (speed*elapsed + verticesP2[5] <= 2.0f){
				for (int i = 0; i < 12; i++){
					if (i % 2 != 0){
						verticesP2[i] += speed * elapsed;
					}
				}
			}
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			if (verticesP2[1] - speed*elapsed >= -2.0){
				for (int i = 0; i < 12; i++){
					if (i % 2 != 0){
						verticesP2[i] -= speed* elapsed;
					}
				}
			}
		}


		//Ball 

		if ((collision(verticesP1, verticesball)) || (collision(verticesP2, verticesball))){
			ballSpeed *= -1;
		}
		for (int i = 0; i < 12; i++){
			if (i % 2 != 0){
				verticesball[i] += ballSpeed * elapsed * sin(angle);
			}
			else{
				verticesball[i] += ballSpeed* cos(angle) *elapsed;
			}
		}
		//X
		//Check Left
		if ((verticesball[0] + ballSpeed* elapsed) <= -6.0){

			//If it reached left, reset ball
			//Give points to right player
			DrawText(&program, font, "Player Two won", 0.25, 0.0);


			//Resets ball
			/*		for (int i = 0; i < 12; i++){
			verticesball[i] = startPos[i];
			}
			angle = rand() % 360;
			*/
		}
		//Check Right
		if ((verticesball[2] + ballSpeed *elapsed) >= 6.0){
			//If it reached right, reset ball
			//Give points to left player
			DrawText(&program, font, "Player One won", 0.25, 0.0);

			/*	for (int i = 0; i < 12; i++){
			verticesball[i] = startPos[i];
			}
			angle = rand() % 360;
			*/
		}

		//Y 
		//Check Ceiling
		if ((verticesball[5] + ballSpeed* elapsed * sin(angle)) >= 2.0){
			angle *= -1;
		}
		//Floor
		if ((verticesball[1] + ballSpeed * elapsed * sin(angle)) <= -2.0){
			angle *= -1;
		}
		//if (winner == 1){
		//	DrawText(&program, font, "Player One won", 0.25, 0.0);
		//}
		//else if (winner == 2){
		//	DrawText(&program, font, "Player Two won", 0.25, 0.0);
		//	time_t a(300);
		//}
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}

		}
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;

}


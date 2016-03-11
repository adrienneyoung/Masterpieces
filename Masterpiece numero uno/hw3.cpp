#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include <string>
#include <math.h>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

using namespace std;
SDL_Window* displayWindow;
#define MAX_BULLETS 30 
#define ENEMY_MIN_X -2.75f
#define ENEMY_MIN_Y -1.90f
#define ENEMY_MAX_Y 1.90f
#define FIXED_TIMESTEP 0.0166666f 
#define MAX_TIMESTEPS 6

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

void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, });         texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size, });
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

class Entity{
public:
	float x, y, height, width, speed, directionX, directionY;
	Matrix matrix;
	bool isAlive;
	bool active; // on screen
	int textureID;
	Entity(){}
	void player(){
		x = -2.9f;
		y = -0.25f;
		height = 1.5f;
		width = 1.8f;
		speed = 1.0f;
		directionX = 0.0f;
		directionY = 0.0f;
		active = true;
		isAlive = true;
		textureID = LoadTexture("largeMagikarp.png");

	}
	void bullet(){
		x = -5.0f;
		y = -0.29;
		height = 0.2f;
		width = 0.4f;
		speed = 1.0f;
		directionX = 1.0f;
		directionY = 0.0f;
		active = false;
		isAlive = true;
		textureID = LoadTexture("sword.png");
	}
	void enemy(){
		x = 2.00f;
		y = 1.75f;
		height = 0.5f;
		width = 0.5f;
		speed = 0.5f;
		directionX = 0.0f;
		directionY = -1.0f;
		isAlive = true;
		active = true;
		textureID = LoadTexture("onigiri.png");
	}

	void updateImage(ShaderProgram * program, float* texVert){
		if (active){
			if (!isAlive){
				//Dead so go off screen
				active = false;
			}
			GLfloat vertices[] =
			{
				x - width / 2, y - height / 2,
				x + width / 2, y - height / 2,
				x + width / 2, y + height / 2,
				x - width / 2, y - height / 2,
				x + width / 2, y + height / 2,
				x - width / 2, y + height / 2 //0,0
			};

			program->setModelMatrix(matrix);
			glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(program->positionAttribute);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texVert);
			glEnableVertexAttribArray(program->texCoordAttribute);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	//Entity(float a, float b, float h, float w, float s, float dX, float dY, bool onScreen, int id) :x(a), y(b), height(h), width(w), speed(s), directionX(dX), directionY(dY), active(onScreen), textureID(id){}
	void died(ShaderProgram * program, float* texVert){
		isAlive = false;
		directionX = 0.0f;
		directionY = 0.0f;
		speed = 0.0f;
		x = -6.0f;
		updateImage(program, texVert);

	}
};

//I was too lazy to make getX and setX methods that I decided to put functions in Entity instead
//class Player : public Entity{
//public:
//	Player(float a = -2.9f, float b = -0.25, float h = 1.5f, float w = 1.8f, float s = 1.0f, float dX = 1.0f, float dY = 1.0f, bool onScreen = true, int id = LoadTexture("largemagikarp.png")) :Entity(a, b, h, w, s, dX, dY, onScreen, id){
//
//	}
//};
//
//class Bullet : public Entity{
//public:
//	Bullet(float a = -5.0f, float b = -0.29f, float h = 0.2f, float w = 0.4f, float s = 1.0f, float dX = 1.0f, float dY = 1.0f, bool onScreen = false, int id = LoadTexture("sword.png")) :Entity(a, b, h, w, s, dX, dY, onScreen, id){
//
//	}
//};
//
//class Enemy: public Entity{
//public:
//	Enemy(float a = 2.25f, float b = 1.75f, float h = 0.5f, float w = 0.5f, float s = 0.5f, float dX = 1.0f, float dY = -1.0f, bool onScreen = true, int id = LoadTexture("onigiri.png")) :Entity(a, b, h, w, s, dX, dY, onScreen, id){
//
//	}
//};


bool collision(Entity* l, Entity* r){
	if (l->y - l->height / 2 < r->y + r->height / 2 && l->y + l->height / 2 > r->y - r->height / 2 &&
		l->x - l->width / 2 < r->x + r->width / 2 && l->x + l->width / 2 > r->x - r->width / 2)
		return true;
	return false;
}

void DrawSpriteSheetSprite(ShaderProgram *program, int index, int spriteCountX, int spriteCountY, float vertices[], GLuint texture) {

	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;

	GLfloat texCoords[] =
	{
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v //0,0
	};

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//	return texCoords[];
	//	float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
	// our regular sprite drawing 

}

void setup(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("PokeInvaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 400, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 400);
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
	Matrix startMenu;

	GLuint mainChar = LoadTexture("largeMagikarp.png");
	GLuint phone = LoadTexture("sword.png");
	GLuint trainers = LoadTexture("trainers1.png");
	GLuint font = LoadTexture("font1.png");
	GLuint onigiri = LoadTexture("onigiri.png");
	//	GLuint bg = LoadTexture("bg.png");

	float verticesP1[] = { -3.8f, -1.0f, -2.0f, -1.0f, -2.0f, 0.5f, -3.8f, -1.0f, -2.0f, 0.5f, -3.8f, 0.5f };
	float bulletVert[] = { -0.5f, -0.5f,
		-0.1f, -0.5f,
		-0.1f, -0.3f,
		-0.5f, -0.5f,
		-0.1f, -0.3f,
		-0.5f, -0.3f
	};

	float verticesP2[] = { 2.0f, 1.5f,
		2.5f, 1.5f,
		2.5f, 2.0f,
		2.0f, 1.5f,
		2.5f, 2.0f,
		2.0f, 2.0f
	};
	float startPos[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	//vector<Entity> bullets;
	//vector<Matrix> bulletMatrixVec;

	//Player
	Entity magikarp;
	magikarp.player();

	//Enemies
	vector<Entity> enemies;
	int rowE = 6;
	int columnE = 6;
	float space = 0.35f; //space between enemies
	float xDiff = 0.0f;
	float yDiff = 0.0f;

	//Set up grid
	for (int i = 0; i < rowE; i++){
		yDiff = space * i;
		for (int j = 0; j < columnE; j++){
			xDiff = space* j;
			Entity e;
			e.enemy();
			e.y -= yDiff;
			e.x += xDiff;
			enemies.push_back(e);
		}
	}

	//Bullets
	vector<Entity>bullets;
	for (int i = 0; i < MAX_BULLETS; i++){
		Entity b;
		b.bullet();
		bullets.push_back(b);
	}
	float lastFrameTicks = 0.0f;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	bool done = false;

	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	SDL_Event event;
	//int winner = 0;
	const int runAnimation[] = { 0, 1, 2, 12, 13, 14, 24, 25, 26, 36, 37, 38 };
	//{3,4,5, 15,16,17, 27,28,29, 39,40,41}
	//{6,7,8, 18,19,20, 30,31,32, 42,43,45}
	const int numFrames = 12;
	float animationElapsed = 0.0f;
	float framesPerSecond = 30.0f;
	int currentIndex = 0;
	Matrix magikarpMatrix;
	Matrix gameMatrix;
	Matrix textMatrix;
	Matrix surprise;
	Matrix gameOver;
	Matrix win;
	int startScreen = 0;


	while (!done) {

		glClear(GL_COLOR_BUFFER_BIT);

		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		if (startScreen == 0)
		{
			program.setModelMatrix(startMenu);
			//DrawText(&program, font, "Ready Player One:", .3f, 0.0f); //Reference to book Ready Player One
			DrawText(&program, font, "Press Space to Start", .2f, 0.0f);
			startMenu.setPosition(-2.50f, -1.0f, 0.0f);

			program.setModelMatrix(textMatrix);
			DrawText(&program, font, "Press B for food", .2f, 0.0f);
			startMenu.setPosition(-2.50f, -1.0f, 0.0f);

			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
				if (keys[SDL_SCANCODE_SPACE]){
					startScreen = 1;
				}
				if (keys[SDL_SCANCODE_B]){
					startScreen = 10;
				}
			}
		}
		else if (startScreen == 10){

			glClear(GL_COLOR_BUFFER_BIT);
			program.setModelMatrix(surprise);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);

			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, startPos);
			glEnableVertexAttribArray(program.positionAttribute);

			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
			glEnableVertexAttribArray(program.texCoordAttribute);
			glBindTexture(GL_TEXTURE_2D, onigiri);
			surprise.setScale(4, 4, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);



			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
				if (keys[SDL_SCANCODE_SPACE]){
					startScreen = 1;
				}
				if (keys[SDL_SCANCODE_B]){
					startScreen = 10;
				}
			}

		}
		else if (startScreen == 1){


			processEvents();
			update();
			render();
			float ticks = (float)SDL_GetTicks() / 7000.0;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;

			float fixedElapsed = elapsed;
			if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
				fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
			}
			while (fixedElapsed >= FIXED_TIMESTEP) {
				fixedElapsed -= FIXED_TIMESTEP;
			}

			animationElapsed += fixedElapsed;
			if (animationElapsed > 1.0 / framesPerSecond) {
				currentIndex++;
				animationElapsed = 0.0;
				if (currentIndex > numFrames - 1) {
					currentIndex = 0;
				}
			}


			//	DrawSpriteSheetSprite(&program, runAnimation[currentIndex], 12, 12, verticesP2, trainers);


			program.setModelMatrix(modelMatrix);
			program.setViewMatrix(viewMatrix);
			program.setProjectionMatrix(projectionMatrix);

			magikarp.updateImage(&program, texCoords);
			int numDead = 0;

			//Enemies
			for (int i = 0; i < rowE*columnE; i++){
				if (enemies[i].isAlive){
					//If enemy is alive ...
					float distance = 0.5f * fixedElapsed * framesPerSecond;
					enemies[i].x -= distance / 5;
					//Constantly move to the left
					if (enemies[i].directionY > 0.0f){
						//If you're moving upward
						if (enemies[i].y + distance < ENEMY_MAX_Y){
							enemies[i].y += distance;
						}
						else{
							//Turn around
							enemies[i].directionY = -1.0f;
						}
					}
					else if (enemies[i].directionY < 0.0f){
						//Moving down
						if (enemies[i].y + distance > ENEMY_MIN_Y){
							enemies[i].y -= distance;
						}
						else{
							//180 noscope
							enemies[i].directionY = 1.0f;
						}
					}
					if (enemies[i].x - distance < ENEMY_MIN_X){
						startScreen = -1; // Game Over
					}
					if (collision(&magikarp, &enemies[i])){
						startScreen = -1;
					}
					for (int j = 0; j < MAX_BULLETS; j++){
						if (collision(&enemies[i], &bullets[j])){
							enemies[i].died(&program, texCoords);
							bullets[j].died(&program, texCoords);
						}
					}
					enemies[i].updateImage(&program, texCoords);
				}
				else{
					numDead++;
				}
				if (numDead == rowE*columnE){
					startScreen = -2; //You win
				}
			}



			//Bullets
			for (int i = 0; i < bullets.size(); i++){
				float distance = fixedElapsed * framesPerSecond; //Bullet speed is 1.0f unless not active
				if (bullets[i].active){
					if (bullets[i].x + distance > 5.0f){
						//Bullet leaves screen
						bullets[i].died(&program, texCoords);
					}
					bullets[i].x += distance;
					bullets[i].updateImage(&program, texCoords);
					for (int j = 0; j < enemies.size(); j++){
						if (collision(&bullets[i], &enemies[j])){
							//Bullet collide with enemy
							enemies[j].died(&program, texCoords);
							bullets[i].died(&program, texCoords);
						}
					}
				}
			}


			/*	for (int i = 0; i < 16; i++){
			enemies[i].updateImage(&program, texCoords);
			}
			*/	//food.updateImage(&program, texCoords);

			program.setModelMatrix(gameMatrix);

			DrawSpriteSheetSprite(&program, runAnimation[currentIndex], 12, 12, verticesP2, trainers);
			//Random moving sprite to fulfill requirement. I wanted to make the enemies moving sprites ._.

			if (keys[SDL_SCANCODE_W]) {
				if (magikarp.y + (framesPerSecond * fixedElapsed) < 1.50f){
					magikarp.y += framesPerSecond * fixedElapsed;
					//	magikarp.matrix.Translate(0, framesPerSecond * elapsed, 0);
				}

			}
			else if (keys[SDL_SCANCODE_S]) {
				if (magikarp.y - (framesPerSecond * fixedElapsed) > -1.50f){
					magikarp.y -= framesPerSecond * fixedElapsed;
					//	magikarp.matrix.Translate(0, -framesPerSecond * elapsed, 0);
				}
			}

			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
				if (keys[SDL_SCANCODE_SPACE]){
					for (int i = 0; i < MAX_BULLETS; i++){
						if (bullets[i].active == false){
							bullets[i].active = true;
							bullets[i].isAlive = true;
							bullets[i].x = magikarp.x + 0.75f;
							bullets[i].y = magikarp.y;
							bullets[i].updateImage(&program, texCoords);
							i += MAX_BULLETS;
						}
					}
				}
			}
		}
		else if (startScreen == -1){

			program.setModelMatrix(gameOver);
			DrawText(&program, font, "You lost!", .3f, 0.0f);
			startMenu.setPosition(-2.50f, -1.0f, 0.0f);
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
			}
		}
		else if (startScreen == -2){

			program.setModelMatrix(win);
			DrawText(&program, font, "You win!", .3f, 0.0f);
			startMenu.setPosition(-2.50f, -1.0f, 0.0f);
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
			}
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;

}


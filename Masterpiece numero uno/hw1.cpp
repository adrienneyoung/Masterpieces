#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <iostream>

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

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Free Chinatsu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	SDL_Event event;
	bool done = false;

	glViewport(0, 0, 640, 360);          
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");          

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(program.programID);
	
	Matrix projectionMatrix;     
	Matrix modelMatrix;  
	Matrix modelMatrixc;
	
	Matrix viewMatrix;     
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);          
	
	GLuint bg = LoadTexture("getherout.png");          
	GLuint creeper = LoadTexture("omfgno.png");
	GLuint kick = LoadTexture("dieho.png");

	
	float verticesbg[] = { -3.0f, -3.0f, 3.0f,-3.0f, 3.0f,3.0f, -3.0f,-3.0f, 3.0f,3.0f, -3.0f,3.0f};
	float verticesc[] = { 0.0f, 0.0f, 2.0f, 0.0f,	2.0f, 1.5f, 0.0f, 0.0f, 2.0f, 1.5f, 0.0f, 1.5f };
	float verticesk[] = { -2.0f, -2.0f, 0.0f, -2.0f, 0.0f, 0.0f, -2.0f, -2.0f, 0.0f, 0.0f, -2.0f, 0.0f };
	float verticesj[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
	
	glUseProgram(program.programID);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	
	float lastFrameTicks = 0.0f;
	
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float x = 0.0f;// , y = 0.0f;
	while (!done) {

		glClear(GL_COLOR_BUFFER_BIT);

		program.setModelMatrix(modelMatrix);        
		program.setProjectionMatrix(projectionMatrix);         
		program.setViewMatrix(viewMatrix);                  
		
		glBindTexture(GL_TEXTURE_2D, bg);                  
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesbg);         
		glEnableVertexAttribArray(program.positionAttribute);                  
		glEnableVertexAttribArray(program.texCoordAttribute);                  
		glDrawArrays(GL_TRIANGLES, 0, 6);         
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);         
		glBindTexture(GL_TEXTURE_2D, kick);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesk);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);

		program.setModelMatrix(modelMatrixc);
		glBindTexture(GL_TEXTURE_2D, creeper);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesc);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);

		glDisableVertexAttribArray(program.positionAttribute);         
		glDisableVertexAttribArray(program.texCoordAttribute);
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		x += elapsed;
		//y += elapsed;
		//modelMatrixc.Rotate(elapsed*7);
		modelMatrixc.setScale(x, x, 0.0f);
		//modelMatrixc.Rotate(x);
		if (x < 20){ modelMatrixc.Translate(-elapsed / 5, -elapsed / 10, 0.0f); }
		if (40 >=x >= 20){

			Matrix modelMatrixj;
			GLuint jumpscare = LoadTexture("omfgno.png");
			program.setModelMatrix(modelMatrixj);

			glBindTexture(GL_TEXTURE_2D, jumpscare);
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesj);
			glEnableVertexAttribArray(program.positionAttribute);
			glEnableVertexAttribArray(program.texCoordAttribute);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);

			modelMatrixj.setScale(10.0f, 10.f, 0.0f);
			
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			//How do you affect the vertices array
			//Also DrawSprite does not work for some reason, need to import another library?
			/*else if (keys[SDL_SCANCODE_LEFT]) {     
				verticesc += {-1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, };
			} else if(keys[SDL_SCANCODE_RIGHT]) {
				verticesc += {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, };
			}
			else if (keys[SDL_SCANCODE_UP]) {
				verticesc += {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};
			}
			else if (keys[SDL_SCANCODE_DOWN]) {
				verticesc += {0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f};
			}*/

		}
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}


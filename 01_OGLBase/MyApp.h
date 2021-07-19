#pragma once

// C++ includes
#include <memory>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "includes/gCamera.h"

#include "includes/ProgramObject.h"
#include "includes/BufferObject.h"
#include "includes/VertexArrayObject.h"
#include "includes/TextureObject.h"

// mesh
#include "includes/ObjParser_OGL3.h"

//skybox
#include "SkyBox.h"

//SDL_Mixer
#include "include/SDL_mixer.h"

class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

protected:
	// shaderekhez szükséges változók
	gCamera		m_camera;

	SkyBox* skybox;

	ProgramObject		planet_program;
	VertexArrayObject	m_planet_vao;		//Parametrikus felület VAO-ja
	IndexBuffer			m_planet_indices;	//Parametrikus felület indextömbje
	ArrayBuffer			m_planet_vbo;		//Parametrikus felület VBO-ja
	Texture2D			m_earth_texture;	//A Föld színtextúrája
	Texture2D			m_earth_height_map;	//A föld domborzati textúrája
	Texture2D			m_moon_texture;		//A Hold színtextúrája
	Texture2D			m_moon_height_map;	//A Hold domborzati textúrája
	Texture2D			m_sun_texture;		//A Hold színtextúrája

	void InitPlanet();			//A parametrikus felületet inicializáló metódus

	int N = 200, M = 200;		//Parametrikus felület felbontása

	// a jobb olvashatóság kedvéért
	void InitShaders();

	bool paused = false;
	float time;
	float last_time;

	Mix_Music* background_music;
};


#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>
#include <imgui/imgui.h>
#include "includes/GLUtils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

CMyApp::CMyApp(void)
{
	m_camera.SetView(glm::vec3(0, 30, 70), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

CMyApp::~CMyApp(void)
{
}

void CMyApp::InitPlanet()
{
	std::vector<glm::vec2> vert((N + 1) * (M + 1));
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;


			vert[i + j * (N + 1)] = glm::vec2(u, v);
		}

	std::vector<int> indices(3 * 2 * (N) * (M));
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1) * (N + 1);
		}

	m_planet_vbo.BufferData(vert);
	m_planet_indices.BufferData(indices);
	m_planet_vao.Init(
		{
			//A vertex- és textúrakoordinátákat is ugyanazzal az uv párossal párral adjuk meg:
			{CreateAttribute<0, glm::vec2, 0, sizeof(glm::vec2)>, m_planet_vbo},
			{CreateAttribute<1, glm::vec2, 0, sizeof(glm::vec2)>, m_planet_vbo}
		}, m_planet_indices);
}


void CMyApp::InitShaders()
{
	// a shadereket tároló program létrehozása az OpenGL-hez hasonló módon:
	planet_program.AttachShaders({
		{ GL_VERTEX_SHADER, "shaders/planet_vert.vert"},
		{ GL_FRAGMENT_SHADER, "shaders/planet_frag.frag"}
	});

	// attributomok osszerendelese a VAO es shader kozt
	planet_program.BindAttribLocations({
		{ 0, "vs_in_uv" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_tex" },				// VAO 1-es csatorna menjen a vs_in_tex-be
	});

	planet_program.LinkProgram();
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	skybox = new SkyBox(
		"assets/right.png",
		"assets/left.png",
		"assets/top.png",
		"assets/bottom.png",
		"assets/front.png",
		"assets/back.png");
	InitPlanet();
	InitShaders();

	m_earth_texture.FromFile("assets/earth.jpg");
	m_earth_height_map.FromFile("assets/earth_height_map.png");
	m_moon_texture.FromFile("assets/moon.jpg");
	m_moon_height_map.FromFile("assets/moon_height_map.jpg");
	m_sun_texture.FromFile("assets/sun.jpg");
	
	// kamera
	m_camera.SetProj(60.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	//Végtelenített háttérzene:
	Mix_Init(MIX_INIT_MOD|MIX_INIT_MP3);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 256);
	background_music = Mix_LoadMUS("assets/2001_space_odysee_theme.wav");
	Mix_PlayMusic(background_music, -1);


	return true;
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!paused) {
		time = SDL_GetTicks() / 1000.0f;
		last_time = time;
	} else {
		time = last_time;
	}
	glm::mat4 viewProj = m_camera.GetViewProj();

	skybox->render(viewProj, m_camera.GetEye());

	//A Nap transzformálása és kirajzolása:
	float sun_size = 20.0f;

	glm::mat4 planet_world =
		glm::rotate<float>(time/2, glm::vec3(0, 1, 0)) *	//2. A föld forgása a tengelye körül.
		glm::scale<float>(glm::vec3(sun_size));				//1. A Föld mérete.
	m_planet_vao.Bind();
	planet_program.Use();
	planet_program.SetTexture("texImage", 1, m_sun_texture);
	//planet_program.SetTexture("heightTexture", 0, m_earth_height_map);
	planet_program.SetUniform("MVP", viewProj * planet_world);
	planet_program.SetUniform("world", planet_world);
	planet_program.SetUniform("worldIT", glm::inverse(glm::transpose(planet_world)));
	planet_program.SetUniform("Ka", glm::vec3(5.0f));
	planet_program.SetUniform("eye_position", m_camera.GetEye());

	glDrawElements(GL_TRIANGLES, N * M * 2 * 3, GL_UNSIGNED_INT, nullptr);	//Négyzetek száma * 6 vertex-et renderelünk

	//A Föld transzformálása és kirajzolása:
	float earth_size = 5.0f;
	float distance_from_sun = 40.0f;

	planet_world =
		glm::rotate<float>(time / 2.0f, glm::vec3(0, 1, 0)) *
		glm::translate<float>(glm::vec3(distance_from_sun, 0.0, 0.0)) *
		glm::rotate<float>((5.0f/6.0f) * M_PI, glm::vec3(0, 0, 1)) *	//3. Ráállítás a forgástengelyre.
		glm::rotate<float>(-time, glm::vec3(0, 1, 0)) *					//2. A föld forgása a tengelye körül.
		glm::scale<float>(glm::vec3(earth_size));						//1. A Föld mérete.
	m_planet_vao.Bind();
	planet_program.SetTexture("texImage", 1, m_earth_texture);
	planet_program.SetTexture("heightTexture", 0, m_earth_height_map);
	planet_program.SetUniform("MVP", viewProj * planet_world);
	planet_program.SetUniform("world", planet_world);
	planet_program.SetUniform("worldIT", glm::inverse(glm::transpose(planet_world)));
	planet_program.SetUniform("Ka", glm::vec3(0.2f));
	planet_program.SetUniform("eye_position", m_camera.GetEye());

	glDrawElements(GL_TRIANGLES, N * M * 2 * 3, GL_UNSIGNED_INT, nullptr);	//Négyzetek száma * 6 vertex-et renderelünk

	//A Hold transzformálása és kirajzolása:
	float moon_to_earth_scale = 0.075f;

	planet_world =
		glm::rotate<float>(time / 2.0f, glm::vec3(0, 1, 0)) *
		glm::translate<float>(glm::vec3(distance_from_sun, 0.0, 0.0)) *
		glm::rotate<float>(time / 2.0f, glm::vec3(0, 1, 0)) *				//A Hold 30-szor lassabban forog, mint a Föld.
		glm::translate<float>(glm::vec3(7.0f, 0.0f, 0.0f)) *				//A föld közelében legyen.
		glm::scale<float>(glm::vec3(moon_to_earth_scale * earth_size));		//1. A Hold mérete.
	m_planet_vao.Bind();
	planet_program.SetTexture("texImage", 1, m_moon_texture);
	planet_program.SetTexture("heightTexture", 0, m_moon_height_map);
	planet_program.SetUniform("MVP", viewProj * planet_world);
	planet_program.SetUniform("world", planet_world);
	planet_program.SetUniform("worldIT", glm::inverse(glm::transpose(planet_world)));
	planet_program.SetUniform("Ka", glm::vec3(0.2f));
	planet_program.SetUniform("eye_position", m_camera.GetEye());
	
	glDrawElements(GL_TRIANGLES, N * M * 2 * 3, GL_UNSIGNED_INT, nullptr);	//Négyzetek száma * 6 vertex-et renderelünk

	planet_program.Unuse();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);

	if (key.keysym.sym == SDLK_SPACE) {
		paused = !paused;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}

void CMyApp::Clean()
{
	Mix_Quit();
}
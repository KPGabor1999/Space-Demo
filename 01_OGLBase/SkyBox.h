#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "includes/gCamera.h"

#include "includes/ProgramObject.h"
#include "includes/BufferObject.h"
#include "includes/VertexArrayObject.h"
#include "includes/TextureObject.h"

class SkyBox
{
private:
	ProgramObject		m_programSkybox;
	VertexArrayObject	m_SkyboxVao;
	IndexBuffer			m_SkyboxIndices;
	ArrayBuffer			m_SkyboxPos;
	TextureCubeMap		m_skyboxTexture;
public:
	SkyBox(
		const std::string& xpos_filepath,
		const std::string& xneg_filepath,
		const std::string& ypos_filepath,
		const std::string& yneg_filepath,
		const std::string& zpos_filepath,
		const std::string& zneg_filepath);	//Paraméterként add meg a képfájlok neveit.
	void render(glm::mat4 viewProj, glm::vec3 eye_position);
};


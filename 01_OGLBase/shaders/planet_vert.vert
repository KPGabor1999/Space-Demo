#version 330 core
#define M_PI 3.1415

// VBO-ból érkezõ változók
in vec2 vs_in_uv;
in vec2 vs_in_tex;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec3 vs_out_norm;
out vec2 vs_out_tex;

// shader külsõ paraméterei
uniform mat4 MVP;
uniform mat4 world;
uniform mat4 worldIT;

uniform float time;

uniform sampler2D heightTexture;


vec3 GetPos(float u, float v){	//Gömb vertexpozíciói
	float r = 1;
	float theta = u * 2 * M_PI;
	float phi   = v * M_PI;

	return r*vec3( sin(phi)*cos(theta),
				   cos(phi),
				   sin(phi)*sin(theta));
}

vec3 GetNorm(float u, float v)
{
	// Képlettel:
	float theta = u * 2 * M_PI;
	float phi   = v * M_PI;
	return vec3(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
}

void main()		//Nem a kért d paramétert alkalmazom, de a heightmap így is illesztve van. Így is jó?
{
	vec3 base_position = GetPos(vs_in_uv.x, vs_in_uv.y);
	vec3 normal = GetNorm(vs_in_uv.x, vs_in_uv.y);
	vec3 height_position = base_position + normal * texture(heightTexture, vs_in_tex).r * 0.03f;
	
	gl_Position = MVP * vec4( height_position, 1 );
	
	vs_out_pos = (world * vec4(height_position, 1)).xyz;
	vs_out_norm = (worldIT * vec4(normal, 0)).xyz;
	vs_out_tex = vs_in_tex;
}
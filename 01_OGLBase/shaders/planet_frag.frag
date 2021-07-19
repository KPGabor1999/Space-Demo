#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

out vec4 fs_out_col;

// irány fényforrás: fény iránya
//uniform vec3 light_dir = vec3(-1,-1,-1);

// fénytulajdonságok: ambiens, diffúz, spekuláris		//Az egyes megvilágítások színei.
vec3 La = vec3(0.4, 0.4, 0.4);
vec3 Ld = vec3(1.0, 1.0, 0.5);
vec3 Ls = vec3(0.9, 0.9, 0.9);

// anyagtulajdonságok: ambiens, diffúz, spekuláris
uniform vec3 Ka;
vec3 Kd = vec3(1.0, 1.0, 1.0);
vec3 Ks = vec3(1.0, 1.0, 1.0);

uniform sampler2D texImage;

vec3 sun_location = vec3(0);
uniform vec3 eye_position;	//A kamera pozíciója.

vec3 pointLight(vec3 La, vec3 Ld, vec3 Ls, vec3 lightPos, float brightness) 
{
	// ambient
	vec3 ambient = La * Ka;

	// diffuse
	vec3 toLight = normalize(lightPos - vs_out_pos);
	
	vec3 normal = normalize(vs_out_norm);
	float di = clamp(dot(toLight, normal), 0.0f, 1.0f);
	vec3 diffuse = Ld * Kd * di;

	// specular
	vec3 specular = vec3(0);

	if (di > 0)
	{
		vec3 e = normalize(eye_position - vs_out_pos);
		vec3 r = reflect(-toLight, normal);
		float si = pow(clamp(dot(e, r), 0.0f, 1.0f), 25);
		specular = Ls * Ks * si;
	}

	// falloff for point light
	float dist = distance(sun_location, vs_out_pos);
	float falloff = brightness / dist / dist;

	return ambient + falloff * (diffuse + specular);
}

void main()
{
	fs_out_col = vec4(pointLight(La, Ld, Ls, sun_location, 1000.0f), 1) * texture(texImage, vs_out_tex);
}
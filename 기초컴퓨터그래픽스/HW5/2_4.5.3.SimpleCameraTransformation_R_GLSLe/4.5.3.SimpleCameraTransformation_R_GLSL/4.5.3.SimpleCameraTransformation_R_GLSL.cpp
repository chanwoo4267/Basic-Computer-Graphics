#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <FreeImage/FreeImage.h>

#define PI 3.1415926535897

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "My_Shading.h"
GLuint h_ShaderProgram; // handle to shader program
GLuint h_ShaderProgram_PS;
GLuint h_ShaderProgram_GS;
GLuint h_ShaderProgram_TXPS;

GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables
GLint loc_ModelViewProjectionMatrix_simple;

#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;

GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;

/* phong and texture */
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_texture, loc_flag_texture_mapping, loc_flag_fog, loc_blind_effect;

#define N_TEXTURES_USED 2
#define TEXTURE_ID_FLOOR 0
#define TEXTURE_ID_TIGER 1
GLuint texture_names[N_TEXTURES_USED];
int flag_texture_mapping;

#define LOC_VERTEX 0
#define LOC_TEXCOORD 2

#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp>
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define LOC_POSITION 0
#define LOC_NORMAL 1

//effect
int flag_draw_screen;
int flag_screen_effect;
int flag_blind_effect;

void My_glTexImage2D_from_file(char* filename) { //texture
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32) {
		FreeImage_Unload(tx_pixmap);
		fprintf(stdout, "Fail 32bit");
	}
}

// fog stuffs
// you could control the fog parameters interactively: FOG_COLOR, FOG_NEAR_DISTANCE, FOG_FAR_DISTANCE   
int flag_fog;

/* material parameter */

Material_Parameters material_tiger;
Material_Parameters material_bus;
Material_Parameters material_floor;

Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];
int use_halfway_vector = 0;

// Begin of geometry setup
#include "4.5.3.GeometrySetup.h"
// End of geometry setup

// Begin of Callback function definitions

// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
typedef struct {
	glm::vec3 prp, vrp, vup; // in this example code, make vup always equal to the v direction.
	float fov_y, aspect_ratio, near_clip, far_clip, zoom_factor;
} CAMERA;

typedef struct {
	int x, y, w, h;
} VIEWPORT;

#define NUMBER_OF_CAMERAS 2
#define MAX_CLIP_DISTANCE 9999999

CAMERA camera[NUMBER_OF_CAMERAS];

//유의사항 : 현재 camera[0]은 2,3,4,5번 카메라가 공통 사용, camera[1]의 vup, vrp, prp는 1번카메라가 사용

glm::vec3 cam5_prp, cam5_vrp, cam5_vup;
float cam5_prp_f[3], cam5_vrp_f[3], cam5_vup_f[3];
float x_angle, y_angle, z_angle;
float cam5_zf;

float object_p[3];

bool camera1_flag = true;
bool camera5_flag = false;

VIEWPORT viewport;

// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when it is ready.
glm::mat4 ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;

void prepare_shader_program(void) {
	
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	h_ShaderProgram = LoadShaders(shader_info);
	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");

	/* GS_shading */
	int i;
	char string[256];
	ShaderInfo shader_info_GS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
		{ GL_NONE, NULL }
	};
	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}
	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");
	/* GS_Shading */

	// phong texture
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");
	
	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");
	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "u_blind_effect");
}
// End of shader setup

// light and material initizlize
void initialize_light_material(void)
{
	glUseProgram(h_ShaderProgram_TXPS);
	int i;

	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUniform1f(loc_blind_effect, 0);

	glUseProgram(0);
}

void update_light(void) {
	glUseProgram(h_ShaderProgram_TXPS);

	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[0].position[0], light[0].position[1],
		light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_light[0].position, 1, &position_EC[0]);
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[0].spot_direction[0],
		light[0].spot_direction[1], light[0].spot_direction[2]);
	glUniform3fv(loc_light[0].spot_direction, 1, &direction_EC[0]);

	position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0],
		light[1].spot_direction[1], light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);

	position_EC = ViewMatrix * glm::vec4(light[2].position[0], light[2].position[1],
		light[2].position[2], light[2].position[3]);
	glUniform4fv(loc_light[2].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[2].spot_direction[0],
		light[2].spot_direction[1], light[2].spot_direction[2]);
	glUniform3fv(loc_light[2].spot_direction, 1, &direction_EC[0]);

	position_EC = ViewMatrix * glm::vec4(light[3].position[0], light[3].position[1],
		light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[3].spot_direction[0],
		light[3].spot_direction[1], light[3].spot_direction[2]);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);

	glUseProgram(0);
	glutPostRedisplay();
}

void setup_scene_light(void) {
	//disabled
	light[0].light_on = 0;
	light[0].position[0] = -100.0f; light[0].position[1] = 1000.0f; 
	light[0].position[2] = 250.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.0f; light[0].ambient_color[1] = 0.0f;
	light[0].ambient_color[2] = 0.0f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.82f; light[0].diffuse_color[1] = 0.82f;
	light[0].diffuse_color[2] = 0.82f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 0.82f; light[0].specular_color[1] = 0.82f;
	light[0].specular_color[2] = 0.82f; light[0].specular_color[3] = 1.0f;

	light[0].spot_direction[0] = 0.0f; light[0].spot_direction[1] = -1.0f; 
	light[0].spot_direction[2] = 0.0f;
	light[0].spot_cutoff_angle = 30.0f;
	light[0].spot_exponent = 20.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 0;
	light[1].position[0] = -100.0f; light[1].position[1] = 1000.0f; // spot light position in WC
	light[1].position[2] = 250.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 5.2f; light[1].ambient_color[1] = 1.2f;
	light[1].ambient_color[2] = 2.2f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 5.82f; light[1].diffuse_color[1] = 5.82f;
	light[1].diffuse_color[2] = 5.82f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 5.82f; light[1].specular_color[1] = 5.82f;
	light[1].specular_color[2] = 5.82f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 0.0f;
	light[1].spot_cutoff_angle = 30.0f;
	light[1].spot_exponent = 20.0f;

	//light 2
	light[2].light_on = 0;
	light[2].spot_direction[0] = -0.3f; light[2].spot_direction[1] = -0.8f; // spot light direction in WC
	light[2].spot_direction[2] = -0.3f;

	light[2].position[0] = cam5_prp[0] - 200;
	light[2].position[1] = cam5_prp[1] + 500;
	light[2].position[2] = cam5_prp[2] - 200;
	light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 5.2f; light[2].ambient_color[1] = 5.9f;
	light[2].ambient_color[2] = 5.2f; light[2].ambient_color[3] = 1.0f;
	light[2].diffuse_color[0] = 5.82f; light[2].diffuse_color[1] = 5.82f;
	light[2].diffuse_color[2] = 5.82f; light[2].diffuse_color[3] = 1.0f;
	light[2].specular_color[0] = 5.82f; light[2].specular_color[1] = 5.82f;
	light[2].specular_color[2] = 5.82f; light[2].specular_color[3] = 1.0f;
	light[2].spot_cutoff_angle = 30.0f;
	light[2].spot_exponent = 20.0f;

	//light3
	light[3].light_on = 0;
	light[3].position[0] = -100.0f; light[3].position[1] = 1000.0f; // spot light position in WC
	light[3].position[2] = 150.0f; light[3].position[3] = 1.0f;

	light[3].ambient_color[0] = 0.2f; light[3].ambient_color[1] = 2.2f;
	light[3].ambient_color[2] = 5.2f; light[3].ambient_color[3] = 1.0f;
	light[3].diffuse_color[0] = 5.82f; light[3].diffuse_color[1] = 5.82f;
	light[3].diffuse_color[2] = 5.82f; light[3].diffuse_color[3] = 1.0f;
	light[3].specular_color[0] = 5.82f; light[3].specular_color[1] = 5.82f;
	light[3].specular_color[2] = 5.82f; light[3].specular_color[3] = 1.0f;

	light[3].spot_direction[0] = 0.0f; light[3].spot_direction[1] = -1.0f; // spot light direction in WC
	light[3].spot_direction[2] = 0.0f;
	light[3].spot_cutoff_angle = 30.0f;
	light[3].spot_exponent = 20.0f;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform1i(loc_light[1].light_on, light[1].light_on);
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);

	//light 0
	glUniform1i(loc_light[0].light_on, light[0].light_on);	
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);
	glUniform1f(loc_light[0].spot_cutoff_angle, light[0].spot_cutoff_angle);
	glUniform1f(loc_light[0].spot_exponent, light[0].spot_exponent);

	//light 2
	glUniform1i(loc_light[2].light_on, light[2].light_on);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);

	//light3
	glUniform1i(loc_light[3].light_on, light[3].light_on);
	glUniform4fv(loc_light[3].ambient_color, 1, light[3].ambient_color);
	glUniform4fv(loc_light[3].diffuse_color, 1, light[3].diffuse_color);
	glUniform4fv(loc_light[3].specular_color, 1, light[3].specular_color);
	glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
	glUniform1f(loc_light[3].spot_exponent, light[3].spot_exponent);

	update_light();
	glUseProgram(0);
}



void update_camera_light(void) {
	light[2].position[0] = cam5_prp[0] - 200;
	light[2].position[1] = cam5_prp[1] + 500;
	light[2].position[2] = cam5_prp[2] - 200;
	light[2].position[3] = 1.0f;
	update_light();
}

void update_object_light(void) {
	light[3].position[0] = -100.0f;
	light[3].position[1] = 1000.0f;
	light[3].position[2] = object_p[2];
	light[3].position[3] = 1.0f;
	update_light();
}

void set_material_tiger(void) {
	glUniform4fv(loc_material.ambient_color, 1, material_tiger.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_tiger.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_tiger.specular_color);
	glUniform1f(loc_material.specular_exponent, material_tiger.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_tiger.emissive_color);
}

void set_material_bus(int ambient, int diffuse, int specular) {
	glUniform4fv(loc_material.ambient_color, 1, material_bus.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_bus.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_bus.specular_color);
	glUniform1f(loc_material.specular_exponent, material_bus.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_bus.emissive_color);
}

void set_material_floor(void) {
	glUniform4fv(loc_material.ambient_color, 1, material_floor.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_floor.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_floor.specular_color);
	glUniform1f(loc_material.specular_exponent, material_floor.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_floor.emissive_color);
}

void print_mat4(const char *string, glm::mat4 M) {
	fprintf(stdout, "\n***** %s ******\n", string);
	for (int i = 0; i < 4; i++)
		fprintf(stdout, "*** COL[%d] (%f, %f, %f, %f)\n", i, M[i].x, M[i].y, M[i].z, M[i].w);
	fprintf(stdout, "**************\n\n");
}

float rotation_angle_cow;
float rotation_angle_bike;
float rotation_angle_bus;
float rotation_angle_dragon;
float movement;


//중요 : 실제 display 함수
void display_camera(int camera_index) {

		glUseProgram(h_ShaderProgram);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
		// At this point, the matrix ViewProjectionMatrix has been properly set up.
		ModelViewProjectionMatrix = glm::scale(ViewProjectionMatrix, glm::vec3(250.0f, 250.0f, 250.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glLineWidth(5.0f);
		draw_axes(); // draw the WC axes.
		glLineWidth(1.0f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(h_ShaderProgram_TXPS);
		set_material_floor();
		glUniform1i(loc_texture, TEXTURE_ID_FLOOR);
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-600.0f, 0.01f, -600.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1000.0f, 1000.0f, 1000.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_object_simple(OBJECT_SQUARE16); //  draw the floor.

		float godzilla_movement;
		godzilla_movement = (float)((10 * (int)rotation_angle_cow) % 180);
		if (godzilla_movement <= 90.0f)
			godzilla_movement = (godzilla_movement - 90.0f) / 10.0f;
		else
			godzilla_movement = -(godzilla_movement - 90.0f) / 10.0f;

		float dragon_movement;
		dragon_movement = (float)((5 * (int)rotation_angle_dragon) % 360);
		if (dragon_movement <= 180.0f)
			dragon_movement = (dragon_movement - 180.0f) / 10.0f;
		else
			dragon_movement = -(dragon_movement - 180.0f) / 10.0f;

		set_material_tiger();
		glUniform1i(loc_texture, TEXTURE_ID_TIGER);
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-150.0f, 0.0f, -100.0f - godzilla_movement*50));
		object_p[0] = -150.0f;
		object_p[2] = -100.0f - godzilla_movement * 50;
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.4f, 0.4f, 0.4f));

		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_object_simple(OBJECT_GODZILLA); // GODZILLA

		set_material_bus(1, 1, 1);
		glUniform1i(loc_texture, TEXTURE_ID_TIGER);
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100.0f, 0.0f, -200.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, rotation_angle_bus * 2 * TO_RADIAN, glm::vec3(0.0, 1.0, 0.0));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(100.0f, 0.0f, -200.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, rotation_angle_bus * 10 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_object_simple(OBJECT_BUS); // BUS

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUseProgram(h_ShaderProgram);

		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, rotation_angle_dragon * TO_RADIAN, glm::vec3(0.0, 1.0, 0.0));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(100.0f, 100.0f, 50.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_DRAGON, 255 / 255.0f, 255 / 255.0f, 0 / 255.0f); //  DRAGON
		
		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(-50.0f, 30.5f, 50.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, rotation_angle_bike * 2 * TO_RADIAN, glm::vec3(0.0, 0.0, 1.0));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(-200.0f, 0.0f, -200.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_BIKE, 255 / 255.0f, 0 / 255.0f, 255 / 255.0f); // BIKE

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(300.0f, 50.0f, 150.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_TIGER, 255 / 255.0f, 0 / 255.0f, 0 / 255.0f); // TIGER

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(h_ShaderProgram_GS);

		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-500.0f, 0.0f, -500.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_object_simple(OBJECT_OPTIMUS); // OPTIMUS

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUseProgram(h_ShaderProgram);

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(200.0f, 0.0f, -300.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_TANK, 25 / 255.0f, 30 / 255.0f, 50 / 255.0f); // TANK

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(150.0f, 50.0f, -160.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_COW, 100 / 255.0f, 100 / 255.0f, 100 / 255.0f); // COW

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(-60.0f, 100.5f, -60.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 30.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_DRAGON, 255 / 255.0f, 100 / 255.0f, 20 / 255.0f); // DRAGON STATIC

		glUseProgram(0);
		update_object_light();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	display_camera(0);
	glutSwapBuffers();
}

unsigned int leftbutton_pressed = 0, rotation_mode_cow = 1, timestamp_cow = 0;
unsigned int rotation_mode_bike = 1, rotation_mode_bus = 1, rotation_mode_dragon = 1, timestamp_bike = 0, timestamp_bus = 0, timestamp_dragon = 0;
int prevx, prevy;

void timer_scene(int value) {
	rotation_angle_cow = (float)(timestamp_cow);
	rotation_angle_bike = (float)(timestamp_bike);
	rotation_angle_bus = (float)(timestamp_bus);
	rotation_angle_dragon = (float)(timestamp_dragon);
	glutPostRedisplay();
	if (rotation_mode_cow)
		timestamp_cow = (timestamp_cow + 1) % 360;
	if (rotation_mode_bike)
		timestamp_bike = (timestamp_bike + 1) % 180;
	if (rotation_mode_bus)
		timestamp_bus = (timestamp_bus + 1) % 180;
	if (rotation_mode_dragon)
		timestamp_dragon = (timestamp_dragon + 1) % 360;

	if (rotation_mode_cow || rotation_mode_bus || rotation_mode_bike || rotation_mode_dragon)
		glutTimerFunc(100, timer_scene, 0);
}



//마우스 입력 제어
void mousepress(int button, int state, int x, int y)  {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		prevx = x, prevy = y;
		leftbutton_pressed = 1;
		glutPostRedisplay();
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
		leftbutton_pressed = 0;
		glutPostRedisplay();
	}
}

#define CAM_ROT_SENSITIVITY 0.15f
void motion_1(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed && camera1_flag) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[1].prp);
		mat4_tmp = glm::rotate(mat4_tmp, -CAM_ROT_SENSITIVITY*delx*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4_tmp = glm::translate(mat4_tmp, -camera[1].prp);

 		camera[1].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[1].vrp, 1.0f));
 		camera[1].vup = glm::vec3(mat4_tmp*glm::vec4(camera[1].vup, 0.0f));

		vec3_tmp = glm::cross(camera[1].vup, camera[1].prp - camera[1].vrp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[1].prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*dely*TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera[1].prp);

 		camera[1].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[1].vrp, 1.0f));
	 	camera[1].vup = glm::vec3(mat4_tmp*glm::vec4(camera[1].vup, 0.0f));

		ViewMatrix = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

		update_light();
	}
}

//중요 : 키 입력 제어부분

void keyboard(unsigned char key, int x, int y) {
	if (key == 27) { // ESC key
		glutLeaveMainLoop(); // incur destuction callback for cleanups.
		return;
	}
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	glm::vec4 position_QC;
	glm::vec3 direction_WC;
	glm::vec3 prp_t;
	float delx = 10.0f;
	float tx, ty;
	switch (key) {
	case '1': // loot at the origin.
		camera1_flag = true;
		camera5_flag = false;
		ViewMatrix = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);
		ProjectionMatrix = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case '2': // loot at the red TIGER.
		camera1_flag = false;
		camera5_flag = false;
		camera[0].prp = glm::vec3(-200.0f, 100.0f, -200.0f);
		ViewMatrix = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
		ProjectionMatrix = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case '3':
		camera1_flag = false;
		camera5_flag = false;
		camera[0].prp = glm::vec3(200.0f, 100.0f, 200.0f);
		ViewMatrix = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
		ProjectionMatrix = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case '4':
		camera1_flag = false;
		camera5_flag = false;
		camera[0].prp = glm::vec3(200.0f, 100.0f, -200.0f);
		ViewMatrix = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
		ProjectionMatrix = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case '5':
		camera1_flag = false;
		camera5_flag = true;
		ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
		ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		update_camera_light();
		glutPostRedisplay();
		break;
	case 'm':
		rotation_mode_cow = 1 - rotation_mode_cow;
		if (rotation_mode_cow && !rotation_mode_bike && !rotation_mode_bus && !rotation_mode_dragon)
			glutTimerFunc(100, timer_scene, 0);
		break;
	case 'n':
		rotation_mode_bike = 1 - rotation_mode_bike;
		if (rotation_mode_bike && !rotation_mode_cow && !rotation_mode_bus && !rotation_mode_dragon)
			glutTimerFunc(100, timer_scene, 0);
		break;
	case 'b':
		rotation_mode_bus = 1 - rotation_mode_bus;
		if (rotation_mode_bus && !rotation_mode_bike && !rotation_mode_cow && !rotation_mode_dragon)
			glutTimerFunc(100, timer_scene, 0);
		break;
	case 'v':
		rotation_mode_dragon = 1 - rotation_mode_dragon;
		if (rotation_mode_dragon && !rotation_mode_bike && !rotation_mode_bus && !rotation_mode_cow)
			glutTimerFunc(100, timer_scene, 0);
		break;
	case 'w':
		if (camera5_flag)
		{
			if (cam5_prp_f[1] >= -1 && cam5_prp_f[1] <= 1) {
				cam5_prp_f[1] += 2.0f;
			}
			else {
				cam5_prp_f[1] += 1.0f;
			}
			cam5_vrp_f[1] += 1.0f;
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);
			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		update_camera_light();
		break;
	case 's':
		if (camera5_flag)
		{
			if (cam5_prp_f[1] >= -1 && cam5_prp_f[1] <= 1) {
				cam5_prp_f[1] -= 2.0f;
			}
			else {
				cam5_prp_f[1] -= 1.0f;
			}
			cam5_vrp_f[1] -= 1.0f;
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);
			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		update_camera_light();
		break;
	case 'a':
		if (camera5_flag)
		{
			if (cam5_prp_f[0] >= -1 && cam5_prp_f[0] <= 1) {
				cam5_prp_f[0] += 2.0f;
			}
			else {
				cam5_prp_f[0] += 1.0f;
			}
			cam5_vrp_f[0] += 1.0f;
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);
			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		update_camera_light();
		break;
	case 'd':
		if (camera5_flag)
		{
			if (cam5_prp_f[0] >= -1 && cam5_prp_f[0] <= 1) {
				cam5_prp_f[0] -= 2.0f;
			}
			else {
				cam5_prp_f[0] -= 1.0f;
			}
			cam5_vrp_f[0] -= 1.0f;
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);
			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		update_camera_light();
		break;
	case 'q':
		if (camera5_flag)
		{
			if (cam5_prp_f[2] >= -1 && cam5_prp_f[2] <= 1) {
				cam5_prp_f[2] += 2.0f;
			}
			else {
				cam5_prp_f[2] += 1.0f;
			}
			cam5_vrp_f[2] += 1.0f;
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);
			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		update_camera_light();
		break;
	case 'e':
		if (camera5_flag)
		{
			if (cam5_prp_f[2] >= -1 && cam5_prp_f[2] <= 1) {
				cam5_prp_f[2] -= 2.0f;
			}
			else {
				cam5_prp_f[2] -= 1.0f;
			}
			cam5_vrp_f[2] -= 1.0f;
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);
			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		update_camera_light();
		break;
	case 'r':
		if (camera5_flag) {
			mat4_tmp = glm::translate(glm::mat4(1.0f), cam5_vrp);
			mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY * delx * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			mat4_tmp = glm::translate(mat4_tmp, -cam5_vrp);

			cam5_prp = glm::vec3(mat4_tmp * glm::vec4(cam5_prp, 1.0f));
			cam5_vup = glm::vec3(mat4_tmp * glm::vec4(cam5_vup, 0.0f));

			cam5_prp_f[0] = cam5_prp.x;
			cam5_prp_f[1] = cam5_prp.y;
			cam5_prp_f[2] = cam5_prp.z;

			cam5_vup_f[0] = cam5_vup.x;
			cam5_vup_f[1] = cam5_vup.y;
			cam5_vup_f[2] = cam5_vup.z;

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 't':
		if (camera5_flag) {
			mat4_tmp = glm::translate(glm::mat4(1.0f), cam5_vrp);
			mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY * delx * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			mat4_tmp = glm::translate(mat4_tmp, -cam5_vrp);

			cam5_prp = glm::vec3(mat4_tmp * glm::vec4(cam5_prp, 1.0f));
			cam5_vup = glm::vec3(mat4_tmp * glm::vec4(cam5_vup, 0.0f));

			cam5_prp_f[0] = cam5_prp.x;
			cam5_prp_f[1] = cam5_prp.y;
			cam5_prp_f[2] = cam5_prp.z;

			cam5_vup_f[0] = cam5_vup.x;
			cam5_vup_f[1] = cam5_vup.y;
			cam5_vup_f[2] = cam5_vup.z;

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'y':
		if (camera5_flag) {
			mat4_tmp = glm::translate(glm::mat4(1.0f), cam5_vrp);
			mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY * delx * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			mat4_tmp = glm::translate(mat4_tmp, -cam5_vrp);

			cam5_prp = glm::vec3(mat4_tmp * glm::vec4(cam5_prp, 1.0f));
			cam5_vup = glm::vec3(mat4_tmp * glm::vec4(cam5_vup, 0.0f));

			cam5_prp_f[0] = cam5_prp.x;
			cam5_prp_f[1] = cam5_prp.y;
			cam5_prp_f[2] = cam5_prp.z;

			cam5_vup_f[0] = cam5_vup.x;
			cam5_vup_f[1] = cam5_vup.y;
			cam5_vup_f[2] = cam5_vup.z;

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'f':
		if (camera5_flag) {
			prp_t = cam5_prp;
			tx = sqrt(cam5_prp_f[1] * cam5_prp_f[1] + cam5_prp_f[2] * cam5_prp_f[2]);
			ty = atan(cam5_prp_f[2] / cam5_prp_f[1]);
			ty += 0.01745329251; // 1' degree

			if ((cam5_prp_f[1] >= 0 && cam5_prp_f[2] >= 0) || (cam5_prp_f[1] >= 0 && cam5_prp_f[2] <= 0)) {
				cam5_prp_f[2] = sin(ty) * tx;
				cam5_prp_f[1] = cos(ty) * tx;
			}
			else
			{
				cam5_prp_f[2] = -sin(ty) * tx;
				cam5_prp_f[1] = -cos(ty) * tx;
			}
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			tx = prp_t.y - cam5_prp_f[1];
			ty = prp_t.z - cam5_prp_f[2];
			cam5_vrp_f[1] -= tx;
			cam5_vrp_f[2] -= ty;
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'g':
		if (camera5_flag) {
			prp_t = cam5_prp;
			tx = sqrt(cam5_prp_f[1] * cam5_prp_f[1] + cam5_prp_f[2] * cam5_prp_f[2]);
			ty = atan(cam5_prp_f[2] / cam5_prp_f[1]);
			ty -= 0.01745329251;
			if ((cam5_prp_f[1] >= 0 && cam5_prp_f[2] >= 0) || (cam5_prp_f[1] >= 0 && cam5_prp_f[2] <= 0)) {
				cam5_prp_f[2] = sin(ty) * tx;
				cam5_prp_f[1] = cos(ty) * tx;
			}
			else
			{
				cam5_prp_f[2] = -sin(ty) * tx;
				cam5_prp_f[1] = -cos(ty) * tx;
			}
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			tx = prp_t.y - cam5_prp_f[1];
			ty = prp_t.z - cam5_prp_f[2];
			cam5_vrp_f[1] -= tx;
			cam5_vrp_f[2] -= ty;
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'h':
		if (camera5_flag) {
			prp_t = cam5_prp;
			tx = sqrt(cam5_prp_f[0] * cam5_prp_f[0] + cam5_prp_f[2] * cam5_prp_f[2]);
			ty = atan(cam5_prp_f[0] / cam5_prp_f[2]);
			ty += 0.01745329251;
			if ((cam5_prp_f[0] >= 0 && cam5_prp_f[2] >= 0) || (cam5_prp_f[2] >= 0 && cam5_prp_f[0] <= 0)) {
				cam5_prp_f[0] = sin(ty) * tx;
				cam5_prp_f[2] = cos(ty) * tx;
			}
			else
			{
				cam5_prp_f[0] = -sin(ty) * tx;
				cam5_prp_f[2] = -cos(ty) * tx;
			}
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			tx = prp_t.x - cam5_prp_f[0];
			ty = prp_t.z - cam5_prp_f[2];
			cam5_vrp_f[0] -= tx;
			cam5_vrp_f[2] -= ty;
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'j':
		if (camera5_flag) {
			prp_t = cam5_prp;
			tx = sqrt(cam5_prp_f[0] * cam5_prp_f[0] + cam5_prp_f[2] * cam5_prp_f[2]);
			ty = atan(cam5_prp_f[0] / cam5_prp_f[2]);
			ty -= 0.01745329251;
			if ((cam5_prp_f[0] >= 0 && cam5_prp_f[2] >= 0) || (cam5_prp_f[2] >= 0 && cam5_prp_f[0] <= 0)) {
				cam5_prp_f[0] = sin(ty) * tx;
				cam5_prp_f[2] = cos(ty) * tx;
			}
			else
			{
				cam5_prp_f[0] = -sin(ty) * tx;
				cam5_prp_f[2] = -cos(ty) * tx;
			}
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			tx = prp_t.x - cam5_prp_f[0];
			ty = prp_t.z - cam5_prp_f[2];
			cam5_vrp_f[0] -= tx;
			cam5_vrp_f[2] -= ty;
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'k':
		if (camera5_flag) {
			prp_t = cam5_prp;
			tx = sqrt(cam5_prp_f[0] * cam5_prp_f[0] + cam5_prp_f[1] * cam5_prp_f[1]);
			ty = atan(cam5_prp_f[1] / cam5_prp_f[0]);
			ty += 0.01745329251;
			if ((cam5_prp_f[0] >= 0 && cam5_prp_f[1] >= 0) || (cam5_prp_f[0] >= 0 && cam5_prp_f[1] <= 0)) {
				cam5_prp_f[1] = sin(ty) * tx;
				cam5_prp_f[0] = cos(ty) * tx;
			}
			else
			{
				cam5_prp_f[1] = -sin(ty) * tx;
				cam5_prp_f[0] = -cos(ty) * tx;
			}
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			tx = prp_t.x - cam5_prp_f[0];
			ty = prp_t.y - cam5_prp_f[1];
			cam5_vrp_f[0] -= tx;
			cam5_vrp_f[1] -= ty;
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'l':
		if (camera5_flag) {
			prp_t = cam5_prp;
			tx = sqrt(cam5_prp_f[0] * cam5_prp_f[0] + cam5_prp_f[1] * cam5_prp_f[1]);
			ty = atan(cam5_prp_f[1] / cam5_prp_f[0]);
			ty -= 0.01745329251;
			if ((cam5_prp_f[0] >= 0 && cam5_prp_f[1] >= 0) || (cam5_prp_f[0] >= 0 && cam5_prp_f[1] <= 0)) {
				cam5_prp_f[1] = sin(ty) * tx;
				cam5_prp_f[0] = cos(ty) * tx;
			}
			else
			{
				cam5_prp_f[1] = -sin(ty) * tx;
				cam5_prp_f[0] = -cos(ty) * tx;
			}
			cam5_prp = glm::vec3(cam5_prp_f[0], cam5_prp_f[1], cam5_prp_f[2]);
			tx = prp_t.x - cam5_prp_f[0];
			ty = prp_t.y - cam5_prp_f[1];
			cam5_vrp_f[0] -= tx;
			cam5_vrp_f[1] -= ty;
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);

			ViewMatrix = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'o':
		if (camera5_flag)
		{
			if (cam5_zf < 7)
				cam5_zf += 0.1;
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'p':
		if (camera5_flag)
		{
			if (cam5_zf > 0.5)
				cam5_zf -= 0.1;
			ProjectionMatrix = glm::perspective(cam5_zf * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			glutPostRedisplay();
		}
		update_camera_light();
		break;
	case 'x':
		glUseProgram(h_ShaderProgram_TXPS);
		light[3].light_on = 1 - light[3].light_on;
		glUniform1i(loc_light[3].light_on, light[3].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		break;

	case 'z':
		glUseProgram(h_ShaderProgram_TXPS);
		light[1].light_on = 1 - light[1].light_on;
		glUniform1i(loc_light[1].light_on, light[1].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		break;

	case 'c':
		glUseProgram(h_ShaderProgram_TXPS);
		light[2].light_on = 1 - light[2].light_on;
		glUniform1i(loc_light[2].light_on, light[2].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'i':
		flag_texture_mapping = 1 - flag_texture_mapping;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case '0':
		flag_fog = 1 - flag_fog;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_fog, flag_fog);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case '9':
		flag_blind_effect = 1 - flag_blind_effect;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	}
	update_light();
}

//카메라 reshape

void reshape(int width, int height) {
	camera[0].aspect_ratio = (float)width / height;
	viewport.x = viewport.y = 0;
	viewport.w = width; viewport.h = height;
	ProjectionMatrix = glm::perspective(camera[0].zoom_factor * camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, 
		camera[0].near_clip, camera[0].far_clip);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &points_VAO);
	glDeleteBuffers(1, &points_VBO);

	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(N_OBJECTS, object_VAO);
	glDeleteBuffers(N_OBJECTS, object_VBO);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}
// End of callback function definitions

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousepress);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
	glutMotionFunc(motion_1);
}


//중요 : CAMERA SETTING 해주는곳!
void initialize_OpenGL(void) {
	// initialize light material
	initialize_light_material();

	// initialize the 0th camera.
	camera[0].prp = glm::vec3(-200.0f, 100.0f, 200.0f);
	camera[0].vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera[0].vup = glm::vec3(0.0f, 1.0f, 0.0f);

	// camera 1
	camera[1].prp = glm::vec3(-200.0f, 100.0f, 200.0f);
	camera[1].vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera[1].vup = glm::vec3(0.0f, 1.0f, 0.0f);

	//camera 5
	cam5_prp = glm::vec3(500.0f, 500.0f, 500.0f);
	cam5_prp_f[0] = 500.0f;
	cam5_prp_f[1] = 500.0f;
	cam5_prp_f[2] = 500.0f;
	cam5_vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	cam5_vrp_f[0] = 0.0f;
	cam5_vrp_f[1] = 0.0f;
	cam5_vrp_f[2] = 0.0f;
	cam5_vup = glm::vec3(0.0f, 1.0f, 0.0f);
	cam5_vup_f[0] = 0.0f;
	cam5_vup_f[1] = 1.0f;
	cam5_vup_f[2] = 0.0f;

	x_angle = 0.0f;
	z_angle = 0.0f;
	y_angle = 0.0f;

	camera[0].fov_y = 15.0f;
	camera[0].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[0].near_clip = 0.1f;
	camera[0].far_clip = MAX_CLIP_DISTANCE;
	camera[0].zoom_factor = 2.0f; // will be used for zoomming in and out.

	cam5_zf = 2.0f; // will be used for zoomming in and out.

	rotation_angle_cow = 0.0f;
	rotation_angle_bike = 0.0f;
	rotation_angle_bus = 0.0f;
	rotation_angle_dragon = 0.0f;

	ViewMatrix = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glClearColor(35 / 255.0f, 155 / 255.0f, 86 / 255.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLfloat fogColor[] = { 0.0f, 1.0f, 1.0f, 1.0f };

	glUseProgram(h_ShaderProgram_TXPS);
	flag_fog = 0;
	flag_texture_mapping = 1;
	flag_blind_effect = 0;

	glUniform1i(loc_flag_fog, flag_fog);
	glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUniform1i(loc_blind_effect, flag_blind_effect);
	glGenTextures(N_TEXTURES_USED, texture_names); //texture
	glUseProgram(0);
}

// 중요 : 새로운 object 준비시 prepare할것!
void prepare_scene(void) {
	prepare_points();
	prepare_axes();

	prepare_square();
	//texture
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);
	My_glTexImage2D_from_file("Data/tiger_tex.jpg");
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	prepare_tiger();
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_FLOOR);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_FLOOR]);
	My_glTexImage2D_from_file("Data/checker_tex.jpg");
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	prepare_cow();
	prepare_dragon();
	prepare_bike();

	prepare_bus();

	prepare_godzilla();
	prepare_ironman();
	prepare_optimus();
	prepare_tank();
	setup_scene_light();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 3
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 HW3";
	char messages[N_MESSAGE_LINES][256] = { "Keys used: q,w,e,r,t,y,o,p,a,s,d,f,g,h,j,k,l,v,b,n,m\nMouse used: Left Butten Click and Move"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}

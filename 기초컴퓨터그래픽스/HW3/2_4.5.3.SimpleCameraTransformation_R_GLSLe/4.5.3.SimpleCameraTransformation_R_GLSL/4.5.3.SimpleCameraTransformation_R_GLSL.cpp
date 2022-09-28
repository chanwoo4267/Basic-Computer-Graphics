#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

#define PI 3.1415926535897

// Begin of shader setup
#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}
// End of shader setup

// Begin of geometry setup
#include "4.5.3.GeometrySetup.h"
// End of geometry setup

// Begin of Callback function definitions

// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

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

//유의사항 : 현재 camera[0]은 2,3,4,5번 카메라가 공통 사용, camera[1]의 vup, vrp, prp는 1번카메라가 사용, camera[1]의 zoom factor만 5번카메라가 사용

glm::vec3 cam5_prp, cam5_vrp, cam5_vup;
float cam5_prp_f[3], cam5_vrp_f[3], cam5_vup_f[3];
float x_angle, y_angle, z_angle;

bool camera1_flag = true;
bool camera5_flag = false;

VIEWPORT viewport[NUMBER_OF_CAMERAS];

// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix[NUMBER_OF_CAMERAS], ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];

// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when it is ready.

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
	// should optimize this dispaly function further to reduce the amount of floating-point operations.

	glViewport(viewport[camera_index].x, viewport[camera_index].y, viewport[camera_index].w, viewport[camera_index].h);
	
	// At this point, the matrix ViewProjectionMatrix has been properly set up.
	
		ModelViewProjectionMatrix = glm::scale(ViewProjectionMatrix[camera_index], glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glLineWidth(5.0f);
		draw_axes(); // draw the WC axes.
		glLineWidth(1.0f);

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index], glm::vec3(-12.0f, -0.01f, -12.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(24.0f, 24.0f, 24.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		draw_object(OBJECT_SQUARE16, 20 / 255.0f, 120 / 255.0f, 50 / 255.0f); //  draw the floor.
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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


		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index],glm::vec3(-3.0f, 0.0f, -3.0f + godzilla_movement));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.01f + godzilla_movement * 0.001f, 0.01f + godzilla_movement * 0.001f, 0.01f + godzilla_movement * 0.001f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_GODZILLA, 0 / 255.0f, 255 / 255.0f, 255 / 255.0f); // GODZILLA
		
		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix[camera_index], rotation_angle_dragon * TO_RADIAN, glm::vec3(0.0, 1.0, 0.0));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(3.0f, 3.0f, 2.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.1f + dragon_movement * 0.01 , 0.1f + dragon_movement * 0.01, 0.1f + dragon_movement * 0.01));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_DRAGON, 255 / 255.0f, 255 / 255.0f, 0 / 255.0f); //  DRAGON
		
		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index], glm::vec3(-5.0f, 3.5f, 5.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, rotation_angle_bike * 2 * TO_RADIAN, glm::vec3(0.0, 0.0, 1.0));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(-2.0f, 0.0f, -2.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_BIKE, 255 / 255.0f, 0 / 255.0f, 255 / 255.0f); // BIKE

		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix[camera_index], rotation_angle_bus * 2 * TO_RADIAN, glm::vec3(0.0, 1.0, 0.0));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(5.0f, 0.0f, -2.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, rotation_angle_bus * 10 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.15f, 0.15f, 0.15f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_BUS, 0 / 255.0f, 255 / 255.0f, 0 / 255.0f); // BUS

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index], glm::vec3(3.0f, 0.0f, 2.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_TIGER, 255 / 255.0f, 0 / 255.0f, 0 / 255.0f); // TIGER

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index], glm::vec3(-1.0f, 0.0f, 6.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_OPTIMUS, 0 / 255.0f, 0 / 255.0f, 0 / 255.0f); // OPTIMUS

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index], glm::vec3(2.0f, 0.0f, -3.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_TANK, 25 / 255.0f, 30 / 255.0f, 50 / 255.0f); // TANK

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index], glm::vec3(5.0f, 0.5f, 6.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_COW, 100 / 255.0f, 100 / 255.0f, 100 / 255.0f); // COW

		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_index], glm::vec3(-6.0f, 3.5f, -6.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 30.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_DRAGON, 255 / 255.0f, 100 / 255.0f, 20 / 255.0f); // DRAGON STATIC
}

//중요 : display_camera 추가

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

		ViewMatrix[0] = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);

		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		glutPostRedisplay();
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
	glm::vec3 prp_t;
	float delx = 10.0f;
	float tx, ty;
	switch (key) {
	case '1': // loot at the origin.
		camera1_flag = true;
		camera5_flag = false;
		ViewMatrix[0] = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);
		ProjectionMatrix[0] = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		break;
	case '2': // loot at the red TIGER.
		camera1_flag = false;
		camera5_flag = false;
		camera[0].prp = glm::vec3(-20.0f, 10.0f, -20.0f);
		ViewMatrix[0] = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
		ProjectionMatrix[0] = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		break;
	case '3':
		camera1_flag = false;
		camera5_flag = false;
		camera[0].prp = glm::vec3(20.0f, 10.0f, 20.0f);
		ViewMatrix[0] = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
		ProjectionMatrix[0] = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		break;
	case '4':
		camera1_flag = false;
		camera5_flag = false;
		camera[0].prp = glm::vec3(20.0f, 10.0f, -20.0f);
		ViewMatrix[0] = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
		ProjectionMatrix[0] = glm::perspective(camera[0].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		break;
	case '5':
		camera1_flag = false;
		camera5_flag = true;
		ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
		ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
			camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
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
			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		}
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
			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		}
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
			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		}
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
			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		}
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
			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		}
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
			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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
			cam5_prp = glm::vec3(cam5_prp_f[0],cam5_prp_f[1],cam5_prp_f[2]);
			tx = prp_t.y - cam5_prp_f[1];
			ty = prp_t.z - cam5_prp_f[2];
			cam5_vrp_f[1] -= tx;
			cam5_vrp_f[2] -= ty;
			cam5_vrp = glm::vec3(cam5_vrp_f[0], cam5_vrp_f[1], cam5_vrp_f[2]);

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
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

			ViewMatrix[0] = glm::lookAt(cam5_prp, cam5_vrp, cam5_vup);
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'o':
		if (camera5_flag)
		{
			if (camera[1].zoom_factor < 7)
				camera[1].zoom_factor += 0.1;
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	case 'p':
		if (camera5_flag)
		{
			if (camera[1].zoom_factor > 0.5)
				camera[1].zoom_factor -= 0.1;
			ProjectionMatrix[0] = glm::perspective(camera[1].zoom_factor * camera[0].fov_y * TO_RADIAN, camera[0].aspect_ratio,
				camera[0].near_clip, camera[0].far_clip);
			ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
			glutPostRedisplay();
		}
		break;
	}
	glutPostRedisplay();
}

//카메라 reshape

void reshape(int width, int height) {
	camera[0].aspect_ratio = (float)width / height;
	viewport[0].x = viewport[1].y = 0;
	viewport[0].w = width; viewport[0].h = height;
	ProjectionMatrix[0] = glm::perspective(camera[0].zoom_factor * camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, 
		camera[0].near_clip, camera[0].far_clip);
	ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];

	camera[1].aspect_ratio = (float)width / height;
	viewport[0].x = viewport[1].y = 0;
	viewport[1].w = width; viewport[1].h = height;
	ProjectionMatrix[1] = glm::perspective(camera[1].zoom_factor * camera[1].fov_y * TO_RADIAN, camera[1].aspect_ratio,
		camera[1].near_clip, camera[1].far_clip);
	ViewProjectionMatrix[1] = ProjectionMatrix[1] * ViewMatrix[1];
	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &points_VAO);
	glDeleteBuffers(1, &points_VBO);

	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(N_OBJECTS, object_VAO);
	glDeleteBuffers(N_OBJECTS, object_VBO);
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
	// initialize the 0th camera.
	camera[0].prp = glm::vec3(-20.0f, 10.0f, 20.0f);
	camera[0].vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera[0].vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix[0] = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);

	// camera 1
	camera[1].prp = glm::vec3(-20.0f, 10.0f, 20.0f);
	camera[1].vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera[1].vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix[1] = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);

	cam5_prp = glm::vec3(10.0f, 10.0f, 20.0f);
	cam5_prp_f[0] = 10.0f;
	cam5_prp_f[1] = 10.0f;
	cam5_prp_f[2] = 20.0f;
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

	camera[1].fov_y = 15.0f;
	camera[1].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[1].near_clip = 0.1f;
	camera[1].far_clip = MAX_CLIP_DISTANCE;
	camera[1].zoom_factor = 2.0f; // will be used for zoomming in and out.

	rotation_angle_cow = 0.0f;
	rotation_angle_bike = 0.0f;
	rotation_angle_bus = 0.0f;
	rotation_angle_dragon = 0.0f;

	glClearColor(35 / 255.0f, 155 / 255.0f, 86 / 255.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
}

// 중요 : 새로운 object 준비시 prepare할것!
void prepare_scene(void) {
	prepare_points();
	prepare_axes();
	prepare_square();
	prepare_tiger();
	prepare_cow();
	prepare_dragon();
	prepare_bike();
	prepare_bus();
	prepare_godzilla();
	prepare_ironman();
	prepare_optimus();
	prepare_tank();
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

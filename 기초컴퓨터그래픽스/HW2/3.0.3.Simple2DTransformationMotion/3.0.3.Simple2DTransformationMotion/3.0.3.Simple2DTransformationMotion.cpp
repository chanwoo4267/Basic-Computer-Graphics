#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f;
float xbefore = 0.0f, ybefore = 0.0f;
float keyy = 100.0f;
float targetx = 0.0f, targety = -300.0f;
float car_x = 0.0f, car_y = 0.0f;
float airplane_x = 0.0f, airplane_y = 0.0f;

int is_space = 1;
int is_enter = 0;
int dir = 1;
int boomflag = 0;
int car_clock = 0;
int dir_car = 1;
int can_input = 1;
int timeh = 0;
float house_scale = 2.0f;
float rad = 50;
int plus = 1;
int mul = 4;



// 2D 물체 정의 부분은 objects.h 파일로 분리
// 새로운 물체 추가 시 prepare_scene() 함수에서 해당 물체에 대한 prepare_***() 함수를 수행함.
// (필수는 아니나 올바른 코딩을 위하여) cleanup() 함수에서 해당 resource를 free 시킴.
#include "objects.h"

unsigned int timestamp = 0;
void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	int f = rand();

	//ground / sky
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -400.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_ground();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 300.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_ground2();

	//set house location randomly
	int house_clock = timestamp % 500;
	if (house_clock == 0)
	{
		int t = rand();
		targetx = t % 1100 - 550; //(-550 ~ 550)
		targety = -300; 
	}

	//airplane
	if (xbefore < -650)
		xbefore = 650;
	else if (xbefore > 650)
		xbefore = -650;

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3( xbefore + mul * dir, keyy, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, dir * 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	xbefore = xbefore + mul * dir;
	airplane_x = xbefore;
	airplane_y = keyy;

	//background airplanes
	int airplane2_clock = timestamp % 620;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-610 + airplane2_clock * 2, 350.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.2f, 0.8f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	int airplane3_clock = timestamp % 310;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-610 + airplane3_clock * 4, 380.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f, 1.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	int airplane4_clock = timestamp % 410;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(610 - airplane4_clock * 3, 390.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f, 0.8f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	int airplane5_clock = timestamp % 1220;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(610 - airplane5_clock * 1, 370.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f, 1.2f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();


	if (is_space)
	{
		//card drawing
		int card1_clock = timestamp % 921;

		if (card1_clock <= 360) {
			if (card1_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (360 - card1_clock) / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card1_clock / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card1_clock <= 920) {
			if (card1_clock > 820) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card1_clock > 640) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card1_clock - 100) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card1_clock > 540) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card1_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}

		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card6_clock = timestamp % 921;

		if (card6_clock <= 360) {
			if (card6_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(360 - card6_clock) / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card6_clock / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card6_clock <= 920) {
			if (card6_clock > 820) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card6_clock > 640) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card6_clock - 100) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card6_clock > 540) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card6_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}

		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card2_clock = timestamp % 921;

		if (card2_clock <= 360) {
			if (card2_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (360 - card2_clock) / 5 * 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card2_clock / 5 * 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card2_clock <= 920) {
			if (card1_clock > 830) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card2_clock > 650) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card2_clock - 110) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card2_clock > 550) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card2_clock > 370) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card1_clock - 10) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card7_clock = timestamp % 921;

		if (card7_clock <= 360) {
			if (card7_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(360 - card7_clock) / 5 * 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card7_clock / 5 * 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card7_clock <= 920) {
			if (card1_clock > 830) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card7_clock > 650) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card7_clock - 110) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card7_clock > 550) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card7_clock > 370) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card1_clock - 10) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card3_clock = timestamp % 921;

		if (card3_clock <= 360) {
			if (card3_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (360 - card3_clock) / 3 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card3_clock / 3 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card3_clock <= 920) {
			if (card3_clock > 840) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card3_clock > 660) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card3_clock - 120) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card3_clock > 560) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card3_clock > 380) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card3_clock - 20) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card8_clock = timestamp % 921;

		if (card8_clock <= 360) {
			if (card8_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(360 - card8_clock) / 3 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card8_clock / 3 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card8_clock <= 920) {
			if (card8_clock > 840) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card8_clock > 660) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card8_clock - 120) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card8_clock > 560) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card8_clock > 380) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card8_clock - 20) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card4_clock = timestamp % 921;

		if (card4_clock <= 360) {
			if (card4_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (360 - card4_clock) / 4 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card4_clock / 4 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card4_clock <= 920) {
			if (card4_clock > 850) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card4_clock > 670) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card4_clock - 130) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card4_clock > 570) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card4_clock > 390) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card4_clock - 30) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();


		int card9_clock = timestamp % 921;

		if (card9_clock <= 360) {
			if (card9_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(360 - card9_clock) / 4 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card9_clock / 4 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card9_clock <= 920) {
			if (card9_clock > 850) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card9_clock > 670) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card9_clock - 130) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card9_clock > 570) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card9_clock > 390) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card9_clock - 30) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card5_clock = timestamp % 921;

		if (card5_clock <= 360) {
			if (card5_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (360 - card5_clock) / 5 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card5_clock / 5 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card5_clock <= 920) {
			if (card5_clock > 860) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card5_clock > 680) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card5_clock - 140) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card5_clock > 580) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card5_clock > 400) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card5_clock - 40) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card10_clock = timestamp % 921;

		if (card10_clock <= 360) {
			if (card10_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(360 - card10_clock) / 5 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card10_clock / 5 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card10_clock <= 920) {
			if (card10_clock > 860) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card10_clock > 680) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card10_clock - 140) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card10_clock > 580) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card10_clock > 400) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card10_clock - 40) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card11_clock = timestamp % 921;

		if (card11_clock <= 360) {
			if (card11_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (360 - card11_clock) / 6 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card11_clock / 6 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card11_clock <= 920) {
			if (card11_clock > 870) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card11_clock > 690) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card11_clock - 150) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card11_clock > 590) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card11_clock > 410) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card11_clock - 50) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card12_clock = timestamp % 921;

		if (card12_clock <= 360) {
			if (card12_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(360 - card12_clock) / 6 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card12_clock / 6 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card12_clock <= 920) {
			if (card12_clock > 870) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card12_clock > 690) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card12_clock - 150) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card12_clock > 590) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card12_clock > 410) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card12_clock - 50) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card13_clock = timestamp % 921;

		if (card13_clock <= 360) {
			if (card13_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (360 - card13_clock) / 9 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, card13_clock / 9 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card13_clock <= 920) {
			if (card13_clock > 880) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card13_clock > 700) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card13_clock - 160) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card13_clock > 600) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card13_clock > 420) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card13_clock - 60) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();

		int card14_clock = timestamp % 921;

		if (card14_clock <= 360) {
			if (card14_clock > 180) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(360 - card14_clock) / 9 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -card14_clock / 9 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		else if (card14_clock <= 920) {
			if (card14_clock > 880) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card14_clock > 700) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (card14_clock - 160) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card14_clock > 600) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 540 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else if (card14_clock > 420) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -(card14_clock - 60) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_x, airplane_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 3.0f, 1.0f));
			}
		}
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_card();
	}

	//sword

	int sword1_clock = timestamp % 531;
	if (sword1_clock < 100) // 충돌전 회전
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, -250.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -sword1_clock / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword1_clock < 200) { // 충돌후 회전
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, -250.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (sword1_clock - 200) / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword1_clock < 290) { //회전
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -(sword1_clock - 200) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword1_clock < 350) { // 전진
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f + (sword1_clock - 290) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword1_clock < 410) { // 후진 회전
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(20 - (sword1_clock - 350) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (-90 + (sword1_clock - 350) * 3 / 2) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword1_clock < 470) { // 후진
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100 - (sword1_clock - 410) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword1_clock < 530) { //전진
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100 - (530 - sword1_clock) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_sword();

	int sword2_clock = timestamp % 531;
	if (sword2_clock < 100) // 충돌전 회전
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -250.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, sword2_clock / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword2_clock < 200) { // 충돌후 휘전
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -250.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -(sword2_clock - 200) / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword2_clock < 290) { // 대기
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -200.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword2_clock < 350) { // 후퇴
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0 + (sword2_clock - 290) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 0 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword2_clock < 410) { // 전진 회전
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(120 - (sword2_clock - 350) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (sword2_clock - 350) * 3 / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword2_clock < 470) {	//전진
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0 - (sword2_clock - 410) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	else if (sword2_clock < 530) {	//후진 회전
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0 - (530 - sword2_clock) * 2, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (60 - (sword2_clock - 470)) * 3 / 2 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	}
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_sword();

	//shirt
	int shirt1_clock = timestamp % 531;
	if (shirt1_clock < 100) // 검-셔츠 간격 20
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f + shirt1_clock / 4, -220.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -shirt1_clock / 4 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt1_clock < 200)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f + (200 - shirt1_clock) / 4, -220.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (-25 + (shirt1_clock-100) / 4) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt1_clock < 290)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt1_clock < 350)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f + (shirt1_clock - 290) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt1_clock < 410)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-30 - (shirt1_clock - 350) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt1_clock < 470)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f - (shirt1_clock - 410) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt1_clock < 530)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-270.0f + (shirt1_clock - 470) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_shirt();

	int shirt2_clock = timestamp % 531;
	if (shirt2_clock < 100) // 검-셔츠 간격 20
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f - shirt2_clock / 4, -220.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, shirt2_clock / 4 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt2_clock < 200)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f - (200 - shirt2_clock) / 4, -220.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (25 - (shirt2_clock - 100) / 4) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt2_clock < 290)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt2_clock < 350)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f + (shirt2_clock - 290) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt2_clock < 410)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(170 - (shirt2_clock - 350) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt2_clock < 470)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f - (shirt2_clock - 410) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	else if (shirt2_clock < 530)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-70.0f + (shirt2_clock - 470) * 2, -220.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	}
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_shirt();

	//car
	

	if (is_enter)
	{
		if (!boomflag) 
		{
			if (car_y > -300)
				car_y -= 10;
			else {
				boomflag = 1;
				car_clock = 0;
				if (car_x < 0)
					dir_car = 1;
				else
					dir_car = -1;
			}

			if (targetx - 20 < car_x && car_x < targetx + 20 && targety - 10 < car_y && car_y < targety + 10)
			{
				house_scale = 0;
			}

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car_x, car_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 1.5f, 1.0f));

				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car();
		}
		else 
		{
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car_x - ((airplane_y + 100)*(dir_car)), car_y, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, (dir_car*car_clock) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3((airplane_y + 100)*dir_car, 0, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, car_clock*5*dir_car *TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 1.5f, 1.0f));

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car();

			car_clock += 3;

			if (car_clock > 180) {
				boomflag = 0;
				car_x = airplane_x;
				car_y = airplane_y;
				is_enter = 0;
				can_input = 1;
			}
		}
	}

	//house

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(targetx, targety, 0.0f));
	if (house_scale == 0) {
		timeh += 1;
		if (timeh > 20)
			timeh -= 1;
		
		if (house_clock == 0) {
			timeh = 0;
			house_scale = 1;
		}
		ModelMatrix = glm::rotate(ModelMatrix, timeh * 20 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0, 2.0 - 0.1*timeh, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	}
	else {
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0, 2.0, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	}
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_house();

	rad += plus;
	if (rad >= 100)
		plus = -1;
	if (rad <= 0)
		plus = 1;

	float rate = rad / 50;

	int car2_clock = timestamp % 361;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, (car2_clock) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(rad, 0, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(rate, rate, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	int car3_clock = timestamp % 361 - 60;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, (car3_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(rad, 0, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(rate, rate, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	int car4_clock = timestamp % 361 - 120;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, (car4_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(rad, 0, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(rate, rate, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	int car5_clock = timestamp % 361 - 180;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, (car5_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(rad, 0, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(rate, rate, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	int car6_clock = timestamp % 361 - 240;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, (car6_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(rad, 0, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(rate, rate, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	int car7_clock = timestamp % 361 - 300;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, (car7_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(rad, 0, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(rate, rate, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	

	glFlush();	
}   

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 13:
		if (can_input) {
			is_enter = 1;
			car_x = airplane_x;
			car_y = airplane_y;
			can_input = 0;
		}
		break;
	case 32:
		if (!is_space)
			is_space = 1;
		else
			is_space = 0;
		break;
	case 87: // W
		keyy = keyy + 1;
		if (keyy > 400)
			keyy = 400;
		break;
	case 83: // S
		keyy = keyy - 1;
		if (keyy < 10)
			keyy = 10;
		break;
	case 65: // A
		dir = -1;
		break;
	case 68: // D
		dir = 1;
		break;
	case 81 :
		mul += 1;
		break;
	case 69 :
		mul -= 1;
		if (mul <= 0)
			mul = 1;
		break;
	}

}

int leftbuttonpressed = 0;
void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
}

void motion(int x, int y) {
	if (leftbuttonpressed) {
		centerx =  x - win_width/2.0f, centery = (win_height - y) - win_height/2.0f;
		//glutPostRedisplay();
	}
} 
	
void reshape(int width, int height) {
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	glDeleteVertexArrays(1, &VAO_house);
	glDeleteBuffers(1, &VBO_house);

	glDeleteVertexArrays(1, &VAO_car);
	glDeleteBuffers(1, &VBO_car);

	glDeleteVertexArrays(1, &VAO_sword);
	glDeleteBuffers(1, &VBO_sword);

	glDeleteVertexArrays(1, &VAO_car2);
	glDeleteBuffers(1, &VBO_car2);

	glDeleteVertexArrays(1, &VAO_card);
	glDeleteBuffers(1, &VBO_card);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
}

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

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(44 / 255.0f, 180 / 255.0f, 49 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_airplane();
	prepare_house();
	prepare_car();
	prepare_car2();
	prepare_sword();
	prepare_card();
	prepare_shirt();
	prepare_ground();
	prepare_ground2();
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

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
int main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple2DTransformationMotion_GLSL_3.0.3";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC'"
		"    - Mouse used: L-click and move"
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	srand((unsigned int)time(NULL));

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutMainLoop();
}


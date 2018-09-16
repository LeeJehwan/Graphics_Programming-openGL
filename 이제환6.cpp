/*---------------------------------------------------------------------------------------------------------------------------
*	파일명 : 이제환6.cpp
*	제작: 이제환
*
*	기능 설명 :
*	이 프로그램은 마우스 왼쪽 버튼을 이용해 시계 방향으로 점을 찍는다.
*	이후에 마우스 오른쪽 버튼을 누르면 9가지의 메인 메뉴가 나온다.
*	첫번째 메뉴인 Polygon은 점을 이어서 3D의 다각형을 만들게 된다.
*	두번째 메뉴인 Rotate Mode 안에는 Rotate Object와 Rotate Light로 이루어져 있다.
*		Rotate Object : 조명과 축은 고정되고, 원점 중심으로부터 물체가 움직인다.
*		Rotate Light  : 누르면 물체는 고정되고, 조명과 축이 움직이게 된다.
*	세번째 메뉴인 Reset Mode 안에는 Reset Object, Reset Light, Reset Material로 이루어져 있다.
*		Reset Object  : 물체가 움직인 회전이 처음 회전으로 변하고 상대적으로 축이 회전한다.
*		Reset Light   : 조명과 축이 움직인 회전이 처음 회전으로 변하고 상대적으로 물체가 회전한다.
*		Reset Material: 물체의 각 면의 성질을 초기화 해준다.
*	네번째 메뉴인 Orhtographic/Perspective는 토글 키로 기본 모드는 Orhtographic이고, 클릭시 Perspective모드가 실행된다.
*	다섯번째 메뉴인 Normal ON/OFF는 초기에는 ON으로 Vertex Normal이 켜진다. 클릭시 OFF가 된다.
*	여섯번째 메뉴인 Varying Material ON/OFF는 초기에는 OFF로 클릭시 각 면의 물체의 특성이 변화된다.
*	일곱번째 메뉴인 Texture ON/OFF는 초기에는 OFF로 클릭시 수박 Texture가 입혀진다.
*	여덟번째 메뉴인 Clear는 처음 실행 상태로 돌아간다.
*	아홉번째 메뉴인 Exit는 프로그램을 종료한다.
---------------------------------------------------------------------------------------------------------------------------*/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <windows.h>
#include <cstdio>
#include <gl/glut.h>
#include <cmath>
#include "SOIL.h"
using namespace std;

/* 함수들의 선언 */
void mouse_Drag(GLint x, GLint y);											//glutMotionFunc callback function
void mouse_Click(GLint button, GLint state, GLint x, GLint y);				//glutMouseFunc callback function
void calCenter();															//무게중심 계산
void menu(GLint value);														//glutCreateMenu callback function
void clearInit();															//기존 내용 삭제
void reshape(int width, int height);										//glutReshapeFunc callback function
void init();																//초기화
void drawAxis();															//좌표축을 그린다.
void drawPolygon();															//도형을 그린다.
void display();																//glutDisplayFunc callback function
void varyMat(GLfloat mat[], GLfloat speed);									//물체의 물질을 변화시킨다.
void getNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]);		//3개의 점이 이루는 면의 법선을 구한다.
void avgNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]);		//3개의 벡터의 평균을 구한다.
void calNormal();															//vertex normal을 구한다.
void drawString(const char *str, int x, int y, float color[4], void *font);	//문자열을 화면에 출력한다.
void showInfo();															//화면에 현재 도형의 정보를 출력해준다.
void initLighting();														//lighting을 초기화 한다.

#define PI 3.141592						//상수 PI

/* 변수들의 선언 */
GLfloat vertex_f[100][3];				//앞면 점들의 집합
GLfloat vertex_b[100][3];				//뒷면 점들의 집합
GLfloat normal[100][3];					//각 면들의 법선의 집합
GLfloat normal_v[100][3];				//각 vertex들의 법선의 집합
GLint pointNum = 0;						//점의 개수

GLfloat mx, my;							//마우스 좌표
GLfloat mdx, mdy;						//마우스 드래그 좌표
GLfloat aspect;							//종횡비
GLfloat fov;							//Field of view

GLfloat rx, ry;							//드래그해서 움직인 거리
GLfloat rx_old, ry_old;					//저장한 물체 회전값
GLfloat rx_light, ry_light;				//조명 회전값
GLfloat rx_light_old, ry_light_old;		//저장한 조명 회전값

GLfloat bx_up = -1.0, bx_lo = 1.0;		//x축 Bounding Box 좌표
GLfloat by_up = -1.0, by_lo = 1.0;		//y축 Bounding Box 좌표
GLfloat center_x, center_y, center_z;	//객체의 무게중심
GLfloat current_width = 500.0;			//현재 화면의 가로 크기
GLfloat current_height = 500.0;			//현재 화면의 세로 크기

//도형 앞면의 성질
GLfloat matF_diffuse[] = { 133 / 255.0, 255 / 255.0, 255 / 255.0, 1 };
GLfloat matF_specular[] = { 0.2, 0.2, 0.2, 1 };
GLfloat matF_shininess[] = { 15.0 };
//도형 뒷면의 성질
GLfloat matB_diffuse[] = { 85 / 255.0, 135 / 255.0, 255 / 255.0, 1 };
GLfloat matB_specular[] = { 0.2, 0.2, 0.2, 1 };
GLfloat matB_shininess[] = { 15.0 };
//도형 측면의 성질
GLfloat matS_diffuse[] = { 107 / 255.0, 102 / 255.0, 255 / 255.0, 1 };
GLfloat matS_specular[] = { 0.2, 0.2, 0.2, 1 };
GLfloat matS_shininess[] = { 15.0 };
//조명0의 정보
GLfloat light0_position[] = { 0, 0, 2, 1 };
GLfloat light0_diffuse[] = { 1, 1, 1, 1 };
GLfloat light0_specular[] = { 1, 1, 1, 1 };
//조명1의 정보
GLfloat light1_position[] = { 0, 0, -2, 1 };
GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1 };
GLfloat light1_specular[] = { 1, 1, 1, 1 };
//전역조명 설정
GLfloat model_ambient[] = { 1, 1, 1, 1 };

/* menu의 control 을 위한 boolean */
GLboolean MOUSE_BOOLEAN = TRUE;
GLboolean POLYGON_BOOLEAN = FALSE;
GLboolean CENTER_BOOLEAN = FALSE;
GLboolean GL3D_BOOLEAN = FALSE;
GLboolean NORMAL_BOOLEAN = TRUE;
GLboolean VARYING_BOOLEAN = FALSE;
GLboolean TEXTURE_BOOLEAN = FALSE;
GLboolean ProjectionMode = TRUE;		//TRUE: Orthographic, FALSE: Perspective
int transfrom_flag;						//transform flag(4:ROTATE_OBJ, 5:ROTATE_LIGHT)

/* menu number의 열거형 */
enum menu_enum {
	POLYGON = 1, PROJECT = 2, ROTATE_OBJ = 3, ROTATE_LIGHT = 4, RESET_OBJ = 5,
	RESET_LIGHT = 6, NORMAL = 7, VARYING = 8, RESET_MAT = 9, CLEAR = 10, EXIT = 11, TEXTURE = 12,
};

GLint texture[2];
int LoadGLTextures() {                                    // Load Bitmaps And Convert To Textures
	/* load an image file directly as a new OpenGL texture */
	texture[0] = SOIL_load_OGL_texture(
		"watermelon_front.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);
	if (texture[0] == 0)
		return false;
	texture[1] = SOIL_load_OGL_texture(
		"watermelon_side.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);
	if (texture[1] == 0)
		return false;

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return true;                                        // Return Success
}

/* 마우스 드래그 함수 */
void mouse_Drag(GLint x, GLint y) {
	mdx = (x - current_width / 2.0) / (current_width / 2.0);	//드래그 순간 x좌표 화면에 맞게 변환
	mdy = (current_height / 2.0 - y) / (current_height / 2.0);	//드래그 순간 y좌표 화면에 맞게 변환

	glMatrixMode(GL_MODELVIEW);							//Modeview matrix
	glLoadIdentity();									//matrix 초기화
	gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);		//Set up viewing transformation, looking down -Z axis
	if (transfrom_flag == ROTATE_OBJ) {					//Object Rotate Mode
		rx = 60 * (mdx - mx);							//회전의 민감도 조정
		ry = 60 * (mdy - my);
		glRotatef(-(ry + ry_old), 1, 0, 0);				//물체 x축 회전
		glRotatef(rx + rx_old, 0, 1, 0);				//물체 y축 회전

		if (VARYING_BOOLEAN) {							//마우스의 움직임에 따라 물질의 값을 변화
			varyMat(matF_diffuse, 0.01);				//앞면의 물질을 변화
			varyMat(matB_diffuse, 0.01);				//뒷면의 물질을 변화
			varyMat(matS_diffuse, 0.02);				//측면의 물질을 변화
		}
	}
	else if (transfrom_flag == ROTATE_LIGHT) {			//Lighting Rotate Mode
		rx_light = 60 * (mdx - mx);						//회전의 민감도 조정
		ry_light = 60 * (mdy - my);
		glRotatef(-(ry_light + ry_light_old), 1, 0, 0);	//축 x축 회전
		glRotatef(rx_light + rx_light_old, 0, 1, 0);	//축 y축 회전
		light0_position[0] = rx_light + rx_light_old;		//마우스의 움직임에 따라 조명회전
		light0_position[1] = ry_light + ry_light_old;
	}
	glutPostRedisplay();
}

/* 마우스 클릭 함수 */
void mouse_Click(GLint button, GLint state, GLint x, GLint y) {
	mx = (x - current_width / 2) / (current_width / 2);		//클릭시 x,y좌표 변환
	my = (current_height / 2 - y) / (current_height / 2);

	//마우스 클릭시 점 저장
	if (MOUSE_BOOLEAN && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (mx < bx_lo) bx_lo = mx;			//점들의 집합으로 Bounding Box를 생성
		if (mx > bx_up) bx_up = mx;
		if (my < by_lo) by_lo = my;
		if (my > by_up) by_up = my;
		vertex_f[pointNum][0] = mx;			//앞면 배열에 좌표 저장
		vertex_f[pointNum][1] = my;

		vertex_b[pointNum][0] = mx;			//뒷면 배열에 좌표 저장
		vertex_b[pointNum][1] = my;
		pointNum++;
	}
	else if (transfrom_flag == ROTATE_LIGHT && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		rx_light_old += rx_light;				//축 회전시 기존 회전 누적
		ry_light_old += ry_light;
	}
	else if (transfrom_flag == ROTATE_OBJ && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		rx_old += rx;						//물체 회전시 기존 회전 누적
		ry_old += ry;
	}
	glutPostRedisplay();
}

/* 도형의 물질을 변화시키는 함수 */
void varyMat(GLfloat mat[], GLfloat speed) {
	static bool con1 = true;	//값 control을 위한 변수
	static bool con2 = false;
	static bool con3 = false;
	static int a = 1;
	if (con1) {
		mat[0] += a*speed;		//첫번째 값을 변화시킴
		if (mat[0] > 2.0) {		//충분히 커지면 다음 값을 변화시킴
			con1 = false;
			con2 = true;
		}
		if (mat[0] < 0.0) {		//작아지면 다시 증가하는 방향으로 변경
			a = 1;
		}
	}
	if (con2) {
		mat[1] += a*speed;		//두번째 값을 변화시킴
		if (mat[1] > 2.0) {		//충분히 커지면 다음 값을 변화시킴
			con2 = false;
			con3 = true;
		}
		if (mat[1] < 0.0) {		//작아지면 다음 값을 변화시킴
			con2 = false;
			con1 = true;
		}
	}
	if (con3) {
		mat[2] += a*speed;		//세번째 값을 변화시킴
		if (mat[2] > 2.0) {		//충분히 커지면 줄어드는 방향으로 변경
			a = -1;
		}
		if (mat[2] < 0.0) {		//작아지면 다음 값을 변화시킴
			con3 = false;
			con2 = true;
		}
	}
}

/* 점 3개가 이루는 평면의 법선을 구하는 함수 */
void getNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]) {
	GLfloat ab[3];		//vector ab
	GLfloat ac[3];		//vector ac
	GLfloat n[3];		//normal vector
						//두 정점간의 벡터 계산
	ab[0] = b[0] - a[0];
	ab[1] = b[1] - a[1];
	ab[2] = b[2] - a[2];

	ac[0] = c[0] - a[0];
	ac[1] = c[1] - a[1];
	ac[2] = c[2] - a[2];
	//외적 계산
	n[0] = ab[1] * ac[2] - ac[1] * ab[2];
	n[1] = ac[0] * ab[2] - ab[0] * ac[2];
	n[2] = ab[0] * ac[1] - ac[0] * ab[1];
	// 정규화
	GLfloat l = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	norm[0] = n[0] / l;
	norm[1] = n[1] / l;
	norm[2] = n[2] / l;
}

/* 세 개의 벡터의 평균을 구하는 함수 */
void avgNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]) {
	norm[0] = (a[0] + b[0] + c[0]) / 3.0;
	norm[1] = (a[1] + b[1] + c[1]) / 3.0;
	norm[2] = (a[2] + b[2] + c[2]) / 3.0;
}

/* 각 vertex 마다 법선을 계산해 주는 함수 */
void calNormal() {
	//측면 normal
	for (int i = 0; i < pointNum; i++)
		getNormal(vertex_f[i], vertex_b[i], vertex_b[(i + 1) % pointNum], normal[i]);
	//앞면 normal
	getNormal(vertex_f[0], vertex_f[1], vertex_f[2], normal[pointNum]);
	//뒷면 noraml
	getNormal(vertex_b[0], vertex_b[2], vertex_b[1], normal[pointNum + 1]);
}

/* 무게중심 계산 */
void calCenter() {
	if (CENTER_BOOLEAN)		//1번만 계산
		return;
	center_x = 0;
	center_y = 0;
	center_z = -(abs(bx_up - bx_lo) + abs(by_up - by_lo)) / 4.0;	//BoundingBox로 z의 무게중심을 구함

	for (int i = 0; i < pointNum; i++) {		//무게중심 계산
		center_x += vertex_f[i][0];
		center_y += vertex_f[i][1];
		vertex_b[i][2] = center_z;				//뒷면 Z좌표 설정
		vertex_f[i][2] = -center_z;
	}
	center_x = center_x / pointNum;
	center_y = center_y / pointNum;
	center_z = 0;
	calNormal();			//법선벡터를 계산한다
	CENTER_BOOLEAN = TRUE;	//무게중심의 좌표가 변하지 않게 실행흐름 제어
}

/* 메뉴함수 : boolean을 통해 메뉴의 실행흐름을 제어 */
void menu(GLint value) {
	switch (value) {
	case POLYGON:									//점들을 이어서 polygon을 생성
		POLYGON_BOOLEAN = TRUE;
		MOUSE_BOOLEAN = FALSE;
		transfrom_flag = 0;
		GL3D_BOOLEAN = TRUE;
		calCenter();
		break;
	case PROJECT:									//PROJECT모드
		ProjectionMode = !ProjectionMode;			//클릭시 토글로 ortho 와 perspective 모드 전환
		init();
		if (transfrom_flag == ROTATE_OBJ) {			//클릭시 회전 장면 유지
			glRotatef(-ry_old, 1, 0, 0);
			glRotatef(rx_old, 0, 1, 0);
		}
		else if (transfrom_flag == ROTATE_LIGHT) {	//클릭시 회전 장면 유지
			glRotatef(-ry_light_old, 1, 0, 0);
			glRotatef(rx_light_old, 0, 1, 0);
		}
		break;
	case NORMAL:									//vertex normal을 on/off
		NORMAL_BOOLEAN = !NORMAL_BOOLEAN;
		break;
	case VARYING:									//material varying on/off
		VARYING_BOOLEAN = !VARYING_BOOLEAN;
		if(VARYING_BOOLEAN)
			transfrom_flag = ROTATE_OBJ;
		break;
	case TEXTURE:									//material varying on/off
		TEXTURE_BOOLEAN = !TEXTURE_BOOLEAN;
		break;
	case ROTATE_OBJ:								//object rotate 모드
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		transfrom_flag = ROTATE_OBJ;
		init();
		glRotatef(-ry_old, 1, 0, 0);				//클릭시 기존 물체 회전 장면 유지
		glRotatef(rx_old, 0, 1, 0);
		break;
	case ROTATE_LIGHT:								//lighting rotate 모드
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		transfrom_flag = ROTATE_LIGHT;
		init();
		glRotatef(-ry_light_old, 1, 0, 0);			//클릭시 기존 축 회전 장면 유지
		glRotatef(rx_light_old, 0, 1, 0);
		break;
	case RESET_OBJ:									//물체 회전 초기화
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		rx_light_old -= rx_old;						//물체가 회전한 각도의 반대 방향으로 축 회전
		ry_light_old -= ry_old;
		rx = ry = rx_old = ry_old = 0;				//물체는 회전각 초기화
		transfrom_flag = ROTATE_OBJ;
		init();
		break;
	case RESET_LIGHT:								//축 회전 초기화
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		rx_old -= rx_light_old;						//축이 회전한 각도의 반대 방향으로 물체 호전
		ry_old -= ry_light_old;
		rx_light = ry_light = rx_light_old = ry_light_old = 0;	//축은 회전각 초기화
		light0_position[0] = 0;						//조명의 위치를 초기화
		light0_position[1] = 0;
		transfrom_flag = ROTATE_LIGHT;
		init();
		break;
	case RESET_MAT:									//material을 처음 값으로 초기화 시켜준다.
		//앞면 초기화
		matF_diffuse[0] = 133 / 255.0;
		matF_diffuse[1] = 255 / 255.0;
		matF_diffuse[2] = 255 / 255.0;
		//뒷면 초기화
		matB_diffuse[0] = 85 / 255.0;
		matB_diffuse[1] = 135 / 255.0;
		matB_diffuse[2] = 255 / 255.0;
		//측면 초기화
		matS_diffuse[0] = 107 / 255.0;
		matS_diffuse[1] = 102 / 255.0;
		matS_diffuse[2] = 255 / 255.0;
		VARYING_BOOLEAN = FALSE;
		break;
	case CLEAR:										//물체와 축을 지운다
		clearInit();
		break;
	case EXIT:										//프로그램 종료
		exit(0);
		break;
	}
	glutPostRedisplay();
}

/* 기존 내용 삭제 및 초기화 */
void clearInit() {
	pointNum = 0;
	mdx = mdy = 0;
	bx_up = by_up = -1.0;
	bx_lo = by_lo = 1.0;
	center_x = center_y = center_z = 0;
	rx = ry = rx_old = ry_old = 0;
	rx_light = ry_light = rx_light_old = ry_light_old = 0;
	light0_position[0] = light0_position[1] = 0;
	matF_diffuse[0] = 133 / 255.0;
	matF_diffuse[1] = 255 / 255.0;
	matF_diffuse[2] = 255 / 255.0;

	matB_diffuse[0] = 85 / 255.0;
	matB_diffuse[1] = 135 / 255.0;
	matB_diffuse[2] = 255 / 255.0;

	matS_diffuse[0] = 107 / 255.0;
	matS_diffuse[1] = 102 / 255.0;
	matS_diffuse[2] = 255 / 255.0;

	MOUSE_BOOLEAN = TRUE;
	POLYGON_BOOLEAN = FALSE;
	CENTER_BOOLEAN = FALSE;
	GL3D_BOOLEAN = FALSE;
	NORMAL_BOOLEAN = TRUE;
	VARYING_BOOLEAN = FALSE;
	ProjectionMode = TRUE;
	transfrom_flag = 0;
	init();
}

/* 창 크기 변화시 좌표 재조정 */
void reshape(int width, int height) {
	current_width = width;
	current_height = height;
	glViewport(0, 0, width, height);
	init();
}

/* 초기화 및 Projection 모드 변경 */
void init() {
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (ProjectionMode) {											//Default: Ortho
		glOrtho(-1.0, 1.0, -1.0, 1.0, 0, 100.0);
	}
	else {															//Perspective mode
		aspect = (GLfloat)current_width / (GLfloat)current_height;	//aspect = 1
		fov = atan(1.0)*180. / PI*1.5;								//fov = 67.5 degree
		gluPerspective(fov, aspect, 0.1, 100.0);					//원근법 적용
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);					// Set up viewing transformation, looking down -Z axis
}

/* 축을 그리는 함수 */
void drawAxis() {
	glBegin(GL_LINES);				//x축
	{
		glColor3f(0.3, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
	}
	glEnd();
	glBegin(GL_LINES);				//x축 (+)화살표
	{
		glVertex3f(0.95, 0.05, 0);
		glVertex3f(1, 0, 0);
	}
	glEnd();
	glBegin(GL_LINES);
	{
		glVertex3f(1, 0, 0);
		glVertex3f(0.95, -0.05, 0);
	}
	glEnd();
	glBegin(GL_LINES);				//y축
	{
		glColor3f(0, 0.3, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
	}
	glEnd();
	glBegin(GL_LINES);				//y축 (+)화살표
	{
		glVertex3f(0.05, 0.95, 0);
		glVertex3f(0, 1, 0);
	}
	glEnd();
	glBegin(GL_LINES);
	{
		glVertex3f(0, 1, 0);
		glVertex3f(-0.05, 0.95, 0);
	}
	glEnd();
	glBegin(GL_LINES);				//z축
	{
		glColor3f(0, 0, 0.3);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
	}
	glEnd();
	glBegin(GL_LINES);				//z축 (+)화살표
	{
		glVertex3f(0, 0.05, 0.95);
		glVertex3f(0, 0, 1);
	}
	glEnd();
	glBegin(GL_LINES);
	{
		glVertex3f(0, 0, 1);
		glVertex3f(0, -0.05, 0.95);
	}
	glEnd();
}

/* 도형을 그리는 함수 */
void drawPolygon() {
	if (TEXTURE_BOOLEAN) {
		LoadGLTextures();
	}
	glPointSize(7.0);

	GLfloat texture_arr[][2]{
		{0.0 , 1.0},
		{0.0 , 0.0},
		{1.0, 0.0},
		{1.0, 1.0}
	};
	
	//앞면 Polygon
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	//texture 시작
	if (!TEXTURE_BOOLEAN)
		glDisable(GL_TEXTURE_2D);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, matF_diffuse);		//앞면 Polygon 물체 성질 부여
	glMaterialfv(GL_FRONT, GL_SPECULAR, matF_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matF_shininess);
	glBegin(GL_POLYGON);			//앞면 Polygon
	for (GLint i = 0; i < pointNum; i++) {
		avgNormal(normal[pointNum], normal[(i + pointNum - 1) % pointNum], normal[i], normal_v[i]);
		if (NORMAL_BOOLEAN) {				//vertex normal 부여 
			glNormal3fv(normal_v[i]);
		}
			glTexCoord2fv(texture_arr[i]);			//texture 부여
		glVertex3f(vertex_f[i][0], vertex_f[i][1], vertex_f[i][2]);
	}
	glEnd();
	

	if (GL3D_BOOLEAN) {					//입체 모드일 경우
										//뒷면 Polygon
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matB_diffuse);		//뒷면 Polygon 물체 성질 부여 
		glMaterialfv(GL_FRONT, GL_SPECULAR, matB_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, matB_shininess);
		glBegin(GL_POLYGON);		//뒷면 Polygon
		{
			for (GLint i = 0; i < pointNum; i++) {
				avgNormal(normal[pointNum + 1], normal[(i + pointNum - 1) % pointNum], normal[i], normal_v[i + pointNum]);
				if (NORMAL_BOOLEAN) {		//vertex normal 부여 
					glNormal3fv(normal_v[i + pointNum]);
				}
				glTexCoord2fv(texture_arr[i]);	//texture 부여
				glVertex3f(vertex_b[i][0], vertex_b[i][1], vertex_b[i][2]);
			}
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		//측면 Polygon
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matS_diffuse);	//측면 Polygon 물체 성질 부여 
		glMaterialfv(GL_FRONT, GL_SPECULAR, matS_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, matS_shininess);
		glLineWidth(2.0);
		for (int i = 0; i < pointNum; i++) {
			glBegin(GL_POLYGON);		//측면 연결 선
			{
				if (NORMAL_BOOLEAN) {	//vertex normal 부여 
					glNormal3fv(normal_v[i]);
				}
				glTexCoord2fv(texture_arr[0]);	//texture 부여
				glVertex3f(vertex_f[i][0], vertex_f[i][1], vertex_f[i][2]);
				if (NORMAL_BOOLEAN) {	//vertex normal 부여 
					glNormal3fv(normal_v[i + pointNum]);
				}
				glTexCoord2fv(texture_arr[1]);
				glVertex3f(vertex_b[i][0], vertex_b[i][1], vertex_b[i][2]);
				if (NORMAL_BOOLEAN) {	//vertex normal 부여 
					GLint temp = i + pointNum + 1;
					if (temp == 2 * pointNum)
						temp -= pointNum;
					glNormal3fv(normal_v[temp]);
				}
				glTexCoord2fv(texture_arr[2]);
				glVertex3f(vertex_b[(i + 1) % pointNum][0], vertex_b[(i + 1) % pointNum][1], vertex_b[(i + 1) % pointNum][2]);
				if (NORMAL_BOOLEAN) {	//vertex normal 부여 
					glNormal3fv(normal_v[(i + 1) % pointNum]);
				}
				glTexCoord2fv(texture_arr[3]);
				glVertex3f(vertex_f[(i + 1) % pointNum][0], vertex_f[(i + 1) % pointNum][1], vertex_f[(i + 1) % pointNum][2]);

			}
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);				//원점으로부터 물체 중심까지의 연결선
	{
		glVertex3f(0, 0, 0);
		glVertex3f(center_x, center_y, center_z);
	}
	glEnd();

	glBegin(GL_POINTS);				//물체의 중심
	glVertex3f(center_x, center_y, center_z);
	glEnd();
	//vertex normal 선 가시화
	if (NORMAL_BOOLEAN) {
		for (GLint i = 0; i < pointNum; i++) {
			glBegin(GL_LINES);
			{
				glVertex3f(vertex_f[i][0], vertex_f[i][1], vertex_f[i][2]);
				glVertex3f(normal_v[i][0] + vertex_f[i][0], normal_v[i][1] + vertex_f[i][1], normal_v[i][2] + vertex_f[i][2]);
			}
			glEnd();
		}
		for (GLint i = 0; i < pointNum; i++) {
			glBegin(GL_LINES);
			{
				glVertex3f(vertex_b[i][0], vertex_b[i][1], vertex_b[i][2]);
				glVertex3f(normal_v[i + pointNum][0] + vertex_b[i][0], normal_v[i + pointNum][1] + vertex_b[i][1], normal_v[i + pointNum][2] + vertex_b[i][2]);
			}
			glEnd();
		}
	}


}

/* 문자열을 화면에 표시 */
void drawString(const char *str, int x, int y, float color[4], void *font) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);
	glDepthFunc(GL_ALWAYS);

	glColor4fv(color);          // set text color
	glRasterPos2i(x, y);        // place text position

								// loop all characters in the string
	while (*str) {
		glutBitmapCharacter(font, *str);
		++str;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
	glPopAttrib();
}

/* 도형의 정보를 화면에 표시하는 함수 */
void showInfo() {
	glPushMatrix();                     // save current modelview matrix
	glLoadIdentity();                   // reset modelview matrix

	glMatrixMode(GL_PROJECTION);        // switch to projection matrix
	glPushMatrix();                     // save current projection matrix
	glLoadIdentity();                   // reset projection matrix
	gluOrtho2D(0, current_width, 0, current_height);  // set to orthogonal projection

	float color[4] = { 1, 1, 1, 1 };

	//정보를 출력
	std::stringstream ss;
	ss << "Projection mode: " << (ProjectionMode ? "Orthographic" : "Perspective");
	void *font = GLUT_BITMAP_8_BY_13;
	drawString(ss.str().c_str(), 2, current_height - 13, color, font);
	ss.str("");

	ss << "Rotate mode: " << (transfrom_flag == ROTATE_OBJ ? "object rotate" : transfrom_flag == ROTATE_LIGHT ? "light rotate" : "unchosen");
	drawString(ss.str().c_str(), 2, current_height - 2 * 13, color, font);
	ss.str("");

	ss << "Vertex normal: " << (NORMAL_BOOLEAN ? "ON" : "OFF");
	drawString(ss.str().c_str(), 2, current_height - 3 * 13, color, font);
	ss.str("");

	ss << "Varying Material mode: " << (VARYING_BOOLEAN ? "ON" : "OFF");
	drawString(ss.str().c_str(), 2, current_height - 4 * 13, color, font);
	ss.str("");

	ss << "Texture mode: " << (TEXTURE_BOOLEAN ? "ON" : "OFF");
	drawString(ss.str().c_str(), 2, current_height - 5 * 13, color, font);
	ss.str("");

	// restore projection matrix
	glPopMatrix();                   // restore to previous projection matrix
									 // restore modelview matrix
	glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
	glPopMatrix();                   // restore to previous modelview matrix
}

void initLighting() {
	glShadeModel(GL_SMOOTH);    //구로 셰이딩
	glEnable(GL_NORMALIZE);		//법선을 정규화
	glEnable(GL_LIGHTING);		//조명 사용
	glEnable(GL_LIGHT0);		//조명0 사용
	glEnable(GL_LIGHT1);		//조명1 사용

	//조명 0번 설정
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	//조명 1번 설정
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	//조명 0번의 attenuation 
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1);
	//조명 1번의 attenuation 
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1);
	//조명1번의 위치
	
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
}


/* diplay callback func */
void display() {
	//color, depth, stencil 버퍼를 비운다
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	showInfo();

	glDisable(GL_LIGHTING);					//Point 를 그린다
	glColor3f(1, 1, 1);
	glPointSize(7.0);
	if (MOUSE_BOOLEAN) {
		glBegin(GL_POINTS);
		for (GLint i = 0; i < pointNum; i++)
			glVertex3f(vertex_f[i][0], vertex_f[i][1], vertex_f[i][2]);
		glEnd();
	}
	glEnable(GL_LIGHTING);

	if (transfrom_flag == ROTATE_LIGHT) {					//축 회전시 draw
		glDisable(GL_LIGHTING);								//축은 조명의 영향을 받지 않음
		drawAxis();											//축을 그리고 축은 회전에 영향을 받음
		glEnable(GL_LIGHTING);								//조명을 다시 on
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	//조명 0번의 위치를 설정
		glPushMatrix();										//물체는 회전의 영향을 받지 않기 위해 설정
		glLoadIdentity();								//행렬 초기화
		gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);	//카메라 뷰 설정
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightfv(GL_LIGHT_MODEL_AMBIENT, GL_POSITION, light0_position);
		glRotatef(-(ry_old), 1, 0, 0);					//기존 물체의 회전을 가져옴
		glRotatef(rx_old, 0, 1, 0);
		drawPolygon();									//물체를 그린다.
		glPopMatrix();										//행렬 스택 복구
	}
	else if (transfrom_flag == ROTATE_OBJ) {				//물체 회전시 draw
		drawPolygon();										//물체를 그리고 물체는 회전에 영향을 받음
		glPushMatrix();										//축은 회전의 영향을 받지 않기 위해 설정
		glLoadIdentity();								//행렬 초기화
		gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);	//카메라 뷰 설정
		light0_position[0] = rx_light_old;				//기존 조명 회전 유지
		light0_position[1] = ry_light_old;
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightfv(GL_LIGHT_MODEL_AMBIENT, GL_POSITION, light0_position);
		glRotatef(-(ry_light_old), 1, 0, 0);			//기존 축 회전 가져옴
		glRotatef(rx_light_old, 0, 1, 0);
		glDisable(GL_LIGHTING);
		drawAxis();										//축을 그린다
		glEnable(GL_LIGHTING);
		glPopMatrix();										//행렬 스택 복구
	}
	else if (POLYGON_BOOLEAN) {
		glPushMatrix();										//축은 회전의 영향을 받지 않기 위해 설정
		glLoadIdentity();								//행렬 초기화
		gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);	//카메라 뷰 설정
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

		drawPolygon();									//물체를 그린다.
		glPopMatrix();										//행렬 스택 복구
	}
	glutSwapBuffers();
}

/* main func */
int main(int argc, char * argv[]) {
	glutInit(&argc, argv);										//glut 초기화 및 설정
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(current_width, current_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("이제환_openGL_HW6");
	clearInit();
	initLighting();

	glutDisplayFunc(display);									//glut call back 함수
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse_Click);
	glutMotionFunc(mouse_Drag);

	GLint subMenu1 = glutCreateMenu(menu);						//submenu 생성
	glutAddMenuEntry("Rotate Object", ROTATE_OBJ);
	glutAddMenuEntry("Rotate Light", ROTATE_LIGHT);

	GLint subMenu2 = glutCreateMenu(menu);						//submenu 생성
	glutAddMenuEntry("Reset Object", RESET_OBJ);
	glutAddMenuEntry("Reset Light", RESET_LIGHT);
	glutAddMenuEntry("Reset Material", RESET_MAT);

	glutCreateMenu(menu);										//menu생성
	glutAddMenuEntry("Polygon", POLYGON);
	glutAddSubMenu("Rotate Mode", subMenu1);
	glutAddSubMenu("Reset Mode", subMenu2);
	glutAddMenuEntry("Orhtographic/Perspective", PROJECT);
	glutAddMenuEntry("Normal ON/OFF", NORMAL);
	glutAddMenuEntry("Varing Material ON/OFF", VARYING);
	glutAddMenuEntry("Texture ON/OFF", TEXTURE);
	glutAddMenuEntry("Clear", CLEAR);
	glutAddMenuEntry("Exit", EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}
/*---------------------------------------------------------------------------------------------------------------------------
*	���ϸ� : ����ȯ6.cpp
*	����: ����ȯ
*
*	��� ���� :
*	�� ���α׷��� ���콺 ���� ��ư�� �̿��� �ð� �������� ���� ��´�.
*	���Ŀ� ���콺 ������ ��ư�� ������ 9������ ���� �޴��� ���´�.
*	ù��° �޴��� Polygon�� ���� �̾ 3D�� �ٰ����� ����� �ȴ�.
*	�ι�° �޴��� Rotate Mode �ȿ��� Rotate Object�� Rotate Light�� �̷���� �ִ�.
*		Rotate Object : ����� ���� �����ǰ�, ���� �߽����κ��� ��ü�� �����δ�.
*		Rotate Light  : ������ ��ü�� �����ǰ�, ����� ���� �����̰� �ȴ�.
*	����° �޴��� Reset Mode �ȿ��� Reset Object, Reset Light, Reset Material�� �̷���� �ִ�.
*		Reset Object  : ��ü�� ������ ȸ���� ó�� ȸ������ ���ϰ� ��������� ���� ȸ���Ѵ�.
*		Reset Light   : ����� ���� ������ ȸ���� ó�� ȸ������ ���ϰ� ��������� ��ü�� ȸ���Ѵ�.
*		Reset Material: ��ü�� �� ���� ������ �ʱ�ȭ ���ش�.
*	�׹�° �޴��� Orhtographic/Perspective�� ��� Ű�� �⺻ ���� Orhtographic�̰�, Ŭ���� Perspective��尡 ����ȴ�.
*	�ټ���° �޴��� Normal ON/OFF�� �ʱ⿡�� ON���� Vertex Normal�� ������. Ŭ���� OFF�� �ȴ�.
*	������° �޴��� Varying Material ON/OFF�� �ʱ⿡�� OFF�� Ŭ���� �� ���� ��ü�� Ư���� ��ȭ�ȴ�.
*	�ϰ���° �޴��� Texture ON/OFF�� �ʱ⿡�� OFF�� Ŭ���� ���� Texture�� ��������.
*	������° �޴��� Clear�� ó�� ���� ���·� ���ư���.
*	��ȩ��° �޴��� Exit�� ���α׷��� �����Ѵ�.
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

/* �Լ����� ���� */
void mouse_Drag(GLint x, GLint y);											//glutMotionFunc callback function
void mouse_Click(GLint button, GLint state, GLint x, GLint y);				//glutMouseFunc callback function
void calCenter();															//�����߽� ���
void menu(GLint value);														//glutCreateMenu callback function
void clearInit();															//���� ���� ����
void reshape(int width, int height);										//glutReshapeFunc callback function
void init();																//�ʱ�ȭ
void drawAxis();															//��ǥ���� �׸���.
void drawPolygon();															//������ �׸���.
void display();																//glutDisplayFunc callback function
void varyMat(GLfloat mat[], GLfloat speed);									//��ü�� ������ ��ȭ��Ų��.
void getNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]);		//3���� ���� �̷�� ���� ������ ���Ѵ�.
void avgNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]);		//3���� ������ ����� ���Ѵ�.
void calNormal();															//vertex normal�� ���Ѵ�.
void drawString(const char *str, int x, int y, float color[4], void *font);	//���ڿ��� ȭ�鿡 ����Ѵ�.
void showInfo();															//ȭ�鿡 ���� ������ ������ ������ش�.
void initLighting();														//lighting�� �ʱ�ȭ �Ѵ�.

#define PI 3.141592						//��� PI

/* �������� ���� */
GLfloat vertex_f[100][3];				//�ո� ������ ����
GLfloat vertex_b[100][3];				//�޸� ������ ����
GLfloat normal[100][3];					//�� ����� ������ ����
GLfloat normal_v[100][3];				//�� vertex���� ������ ����
GLint pointNum = 0;						//���� ����

GLfloat mx, my;							//���콺 ��ǥ
GLfloat mdx, mdy;						//���콺 �巡�� ��ǥ
GLfloat aspect;							//��Ⱦ��
GLfloat fov;							//Field of view

GLfloat rx, ry;							//�巡���ؼ� ������ �Ÿ�
GLfloat rx_old, ry_old;					//������ ��ü ȸ����
GLfloat rx_light, ry_light;				//���� ȸ����
GLfloat rx_light_old, ry_light_old;		//������ ���� ȸ����

GLfloat bx_up = -1.0, bx_lo = 1.0;		//x�� Bounding Box ��ǥ
GLfloat by_up = -1.0, by_lo = 1.0;		//y�� Bounding Box ��ǥ
GLfloat center_x, center_y, center_z;	//��ü�� �����߽�
GLfloat current_width = 500.0;			//���� ȭ���� ���� ũ��
GLfloat current_height = 500.0;			//���� ȭ���� ���� ũ��

//���� �ո��� ����
GLfloat matF_diffuse[] = { 133 / 255.0, 255 / 255.0, 255 / 255.0, 1 };
GLfloat matF_specular[] = { 0.2, 0.2, 0.2, 1 };
GLfloat matF_shininess[] = { 15.0 };
//���� �޸��� ����
GLfloat matB_diffuse[] = { 85 / 255.0, 135 / 255.0, 255 / 255.0, 1 };
GLfloat matB_specular[] = { 0.2, 0.2, 0.2, 1 };
GLfloat matB_shininess[] = { 15.0 };
//���� ������ ����
GLfloat matS_diffuse[] = { 107 / 255.0, 102 / 255.0, 255 / 255.0, 1 };
GLfloat matS_specular[] = { 0.2, 0.2, 0.2, 1 };
GLfloat matS_shininess[] = { 15.0 };
//����0�� ����
GLfloat light0_position[] = { 0, 0, 2, 1 };
GLfloat light0_diffuse[] = { 1, 1, 1, 1 };
GLfloat light0_specular[] = { 1, 1, 1, 1 };
//����1�� ����
GLfloat light1_position[] = { 0, 0, -2, 1 };
GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1 };
GLfloat light1_specular[] = { 1, 1, 1, 1 };
//�������� ����
GLfloat model_ambient[] = { 1, 1, 1, 1 };

/* menu�� control �� ���� boolean */
GLboolean MOUSE_BOOLEAN = TRUE;
GLboolean POLYGON_BOOLEAN = FALSE;
GLboolean CENTER_BOOLEAN = FALSE;
GLboolean GL3D_BOOLEAN = FALSE;
GLboolean NORMAL_BOOLEAN = TRUE;
GLboolean VARYING_BOOLEAN = FALSE;
GLboolean TEXTURE_BOOLEAN = FALSE;
GLboolean ProjectionMode = TRUE;		//TRUE: Orthographic, FALSE: Perspective
int transfrom_flag;						//transform flag(4:ROTATE_OBJ, 5:ROTATE_LIGHT)

/* menu number�� ������ */
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

/* ���콺 �巡�� �Լ� */
void mouse_Drag(GLint x, GLint y) {
	mdx = (x - current_width / 2.0) / (current_width / 2.0);	//�巡�� ���� x��ǥ ȭ�鿡 �°� ��ȯ
	mdy = (current_height / 2.0 - y) / (current_height / 2.0);	//�巡�� ���� y��ǥ ȭ�鿡 �°� ��ȯ

	glMatrixMode(GL_MODELVIEW);							//Modeview matrix
	glLoadIdentity();									//matrix �ʱ�ȭ
	gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);		//Set up viewing transformation, looking down -Z axis
	if (transfrom_flag == ROTATE_OBJ) {					//Object Rotate Mode
		rx = 60 * (mdx - mx);							//ȸ���� �ΰ��� ����
		ry = 60 * (mdy - my);
		glRotatef(-(ry + ry_old), 1, 0, 0);				//��ü x�� ȸ��
		glRotatef(rx + rx_old, 0, 1, 0);				//��ü y�� ȸ��

		if (VARYING_BOOLEAN) {							//���콺�� �����ӿ� ���� ������ ���� ��ȭ
			varyMat(matF_diffuse, 0.01);				//�ո��� ������ ��ȭ
			varyMat(matB_diffuse, 0.01);				//�޸��� ������ ��ȭ
			varyMat(matS_diffuse, 0.02);				//������ ������ ��ȭ
		}
	}
	else if (transfrom_flag == ROTATE_LIGHT) {			//Lighting Rotate Mode
		rx_light = 60 * (mdx - mx);						//ȸ���� �ΰ��� ����
		ry_light = 60 * (mdy - my);
		glRotatef(-(ry_light + ry_light_old), 1, 0, 0);	//�� x�� ȸ��
		glRotatef(rx_light + rx_light_old, 0, 1, 0);	//�� y�� ȸ��
		light0_position[0] = rx_light + rx_light_old;		//���콺�� �����ӿ� ���� ����ȸ��
		light0_position[1] = ry_light + ry_light_old;
	}
	glutPostRedisplay();
}

/* ���콺 Ŭ�� �Լ� */
void mouse_Click(GLint button, GLint state, GLint x, GLint y) {
	mx = (x - current_width / 2) / (current_width / 2);		//Ŭ���� x,y��ǥ ��ȯ
	my = (current_height / 2 - y) / (current_height / 2);

	//���콺 Ŭ���� �� ����
	if (MOUSE_BOOLEAN && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (mx < bx_lo) bx_lo = mx;			//������ �������� Bounding Box�� ����
		if (mx > bx_up) bx_up = mx;
		if (my < by_lo) by_lo = my;
		if (my > by_up) by_up = my;
		vertex_f[pointNum][0] = mx;			//�ո� �迭�� ��ǥ ����
		vertex_f[pointNum][1] = my;

		vertex_b[pointNum][0] = mx;			//�޸� �迭�� ��ǥ ����
		vertex_b[pointNum][1] = my;
		pointNum++;
	}
	else if (transfrom_flag == ROTATE_LIGHT && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		rx_light_old += rx_light;				//�� ȸ���� ���� ȸ�� ����
		ry_light_old += ry_light;
	}
	else if (transfrom_flag == ROTATE_OBJ && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		rx_old += rx;						//��ü ȸ���� ���� ȸ�� ����
		ry_old += ry;
	}
	glutPostRedisplay();
}

/* ������ ������ ��ȭ��Ű�� �Լ� */
void varyMat(GLfloat mat[], GLfloat speed) {
	static bool con1 = true;	//�� control�� ���� ����
	static bool con2 = false;
	static bool con3 = false;
	static int a = 1;
	if (con1) {
		mat[0] += a*speed;		//ù��° ���� ��ȭ��Ŵ
		if (mat[0] > 2.0) {		//����� Ŀ���� ���� ���� ��ȭ��Ŵ
			con1 = false;
			con2 = true;
		}
		if (mat[0] < 0.0) {		//�۾����� �ٽ� �����ϴ� �������� ����
			a = 1;
		}
	}
	if (con2) {
		mat[1] += a*speed;		//�ι�° ���� ��ȭ��Ŵ
		if (mat[1] > 2.0) {		//����� Ŀ���� ���� ���� ��ȭ��Ŵ
			con2 = false;
			con3 = true;
		}
		if (mat[1] < 0.0) {		//�۾����� ���� ���� ��ȭ��Ŵ
			con2 = false;
			con1 = true;
		}
	}
	if (con3) {
		mat[2] += a*speed;		//����° ���� ��ȭ��Ŵ
		if (mat[2] > 2.0) {		//����� Ŀ���� �پ��� �������� ����
			a = -1;
		}
		if (mat[2] < 0.0) {		//�۾����� ���� ���� ��ȭ��Ŵ
			con3 = false;
			con2 = true;
		}
	}
}

/* �� 3���� �̷�� ����� ������ ���ϴ� �Լ� */
void getNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]) {
	GLfloat ab[3];		//vector ab
	GLfloat ac[3];		//vector ac
	GLfloat n[3];		//normal vector
						//�� �������� ���� ���
	ab[0] = b[0] - a[0];
	ab[1] = b[1] - a[1];
	ab[2] = b[2] - a[2];

	ac[0] = c[0] - a[0];
	ac[1] = c[1] - a[1];
	ac[2] = c[2] - a[2];
	//���� ���
	n[0] = ab[1] * ac[2] - ac[1] * ab[2];
	n[1] = ac[0] * ab[2] - ab[0] * ac[2];
	n[2] = ab[0] * ac[1] - ac[0] * ab[1];
	// ����ȭ
	GLfloat l = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	norm[0] = n[0] / l;
	norm[1] = n[1] / l;
	norm[2] = n[2] / l;
}

/* �� ���� ������ ����� ���ϴ� �Լ� */
void avgNormal(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat norm[]) {
	norm[0] = (a[0] + b[0] + c[0]) / 3.0;
	norm[1] = (a[1] + b[1] + c[1]) / 3.0;
	norm[2] = (a[2] + b[2] + c[2]) / 3.0;
}

/* �� vertex ���� ������ ����� �ִ� �Լ� */
void calNormal() {
	//���� normal
	for (int i = 0; i < pointNum; i++)
		getNormal(vertex_f[i], vertex_b[i], vertex_b[(i + 1) % pointNum], normal[i]);
	//�ո� normal
	getNormal(vertex_f[0], vertex_f[1], vertex_f[2], normal[pointNum]);
	//�޸� noraml
	getNormal(vertex_b[0], vertex_b[2], vertex_b[1], normal[pointNum + 1]);
}

/* �����߽� ��� */
void calCenter() {
	if (CENTER_BOOLEAN)		//1���� ���
		return;
	center_x = 0;
	center_y = 0;
	center_z = -(abs(bx_up - bx_lo) + abs(by_up - by_lo)) / 4.0;	//BoundingBox�� z�� �����߽��� ����

	for (int i = 0; i < pointNum; i++) {		//�����߽� ���
		center_x += vertex_f[i][0];
		center_y += vertex_f[i][1];
		vertex_b[i][2] = center_z;				//�޸� Z��ǥ ����
		vertex_f[i][2] = -center_z;
	}
	center_x = center_x / pointNum;
	center_y = center_y / pointNum;
	center_z = 0;
	calNormal();			//�������͸� ����Ѵ�
	CENTER_BOOLEAN = TRUE;	//�����߽��� ��ǥ�� ������ �ʰ� �����帧 ����
}

/* �޴��Լ� : boolean�� ���� �޴��� �����帧�� ���� */
void menu(GLint value) {
	switch (value) {
	case POLYGON:									//������ �̾ polygon�� ����
		POLYGON_BOOLEAN = TRUE;
		MOUSE_BOOLEAN = FALSE;
		transfrom_flag = 0;
		GL3D_BOOLEAN = TRUE;
		calCenter();
		break;
	case PROJECT:									//PROJECT���
		ProjectionMode = !ProjectionMode;			//Ŭ���� ��۷� ortho �� perspective ��� ��ȯ
		init();
		if (transfrom_flag == ROTATE_OBJ) {			//Ŭ���� ȸ�� ��� ����
			glRotatef(-ry_old, 1, 0, 0);
			glRotatef(rx_old, 0, 1, 0);
		}
		else if (transfrom_flag == ROTATE_LIGHT) {	//Ŭ���� ȸ�� ��� ����
			glRotatef(-ry_light_old, 1, 0, 0);
			glRotatef(rx_light_old, 0, 1, 0);
		}
		break;
	case NORMAL:									//vertex normal�� on/off
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
	case ROTATE_OBJ:								//object rotate ���
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		transfrom_flag = ROTATE_OBJ;
		init();
		glRotatef(-ry_old, 1, 0, 0);				//Ŭ���� ���� ��ü ȸ�� ��� ����
		glRotatef(rx_old, 0, 1, 0);
		break;
	case ROTATE_LIGHT:								//lighting rotate ���
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		transfrom_flag = ROTATE_LIGHT;
		init();
		glRotatef(-ry_light_old, 1, 0, 0);			//Ŭ���� ���� �� ȸ�� ��� ����
		glRotatef(rx_light_old, 0, 1, 0);
		break;
	case RESET_OBJ:									//��ü ȸ�� �ʱ�ȭ
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		rx_light_old -= rx_old;						//��ü�� ȸ���� ������ �ݴ� �������� �� ȸ��
		ry_light_old -= ry_old;
		rx = ry = rx_old = ry_old = 0;				//��ü�� ȸ���� �ʱ�ȭ
		transfrom_flag = ROTATE_OBJ;
		init();
		break;
	case RESET_LIGHT:								//�� ȸ�� �ʱ�ȭ
		POLYGON_BOOLEAN = FALSE;
		calCenter();
		MOUSE_BOOLEAN = FALSE;
		rx_old -= rx_light_old;						//���� ȸ���� ������ �ݴ� �������� ��ü ȣ��
		ry_old -= ry_light_old;
		rx_light = ry_light = rx_light_old = ry_light_old = 0;	//���� ȸ���� �ʱ�ȭ
		light0_position[0] = 0;						//������ ��ġ�� �ʱ�ȭ
		light0_position[1] = 0;
		transfrom_flag = ROTATE_LIGHT;
		init();
		break;
	case RESET_MAT:									//material�� ó�� ������ �ʱ�ȭ �����ش�.
		//�ո� �ʱ�ȭ
		matF_diffuse[0] = 133 / 255.0;
		matF_diffuse[1] = 255 / 255.0;
		matF_diffuse[2] = 255 / 255.0;
		//�޸� �ʱ�ȭ
		matB_diffuse[0] = 85 / 255.0;
		matB_diffuse[1] = 135 / 255.0;
		matB_diffuse[2] = 255 / 255.0;
		//���� �ʱ�ȭ
		matS_diffuse[0] = 107 / 255.0;
		matS_diffuse[1] = 102 / 255.0;
		matS_diffuse[2] = 255 / 255.0;
		VARYING_BOOLEAN = FALSE;
		break;
	case CLEAR:										//��ü�� ���� �����
		clearInit();
		break;
	case EXIT:										//���α׷� ����
		exit(0);
		break;
	}
	glutPostRedisplay();
}

/* ���� ���� ���� �� �ʱ�ȭ */
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

/* â ũ�� ��ȭ�� ��ǥ ������ */
void reshape(int width, int height) {
	current_width = width;
	current_height = height;
	glViewport(0, 0, width, height);
	init();
}

/* �ʱ�ȭ �� Projection ��� ���� */
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
		gluPerspective(fov, aspect, 0.1, 100.0);					//���ٹ� ����
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);					// Set up viewing transformation, looking down -Z axis
}

/* ���� �׸��� �Լ� */
void drawAxis() {
	glBegin(GL_LINES);				//x��
	{
		glColor3f(0.3, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
	}
	glEnd();
	glBegin(GL_LINES);				//x�� (+)ȭ��ǥ
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
	glBegin(GL_LINES);				//y��
	{
		glColor3f(0, 0.3, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
	}
	glEnd();
	glBegin(GL_LINES);				//y�� (+)ȭ��ǥ
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
	glBegin(GL_LINES);				//z��
	{
		glColor3f(0, 0, 0.3);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
	}
	glEnd();
	glBegin(GL_LINES);				//z�� (+)ȭ��ǥ
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

/* ������ �׸��� �Լ� */
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
	
	//�ո� Polygon
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	//texture ����
	if (!TEXTURE_BOOLEAN)
		glDisable(GL_TEXTURE_2D);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, matF_diffuse);		//�ո� Polygon ��ü ���� �ο�
	glMaterialfv(GL_FRONT, GL_SPECULAR, matF_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matF_shininess);
	glBegin(GL_POLYGON);			//�ո� Polygon
	for (GLint i = 0; i < pointNum; i++) {
		avgNormal(normal[pointNum], normal[(i + pointNum - 1) % pointNum], normal[i], normal_v[i]);
		if (NORMAL_BOOLEAN) {				//vertex normal �ο� 
			glNormal3fv(normal_v[i]);
		}
			glTexCoord2fv(texture_arr[i]);			//texture �ο�
		glVertex3f(vertex_f[i][0], vertex_f[i][1], vertex_f[i][2]);
	}
	glEnd();
	

	if (GL3D_BOOLEAN) {					//��ü ����� ���
										//�޸� Polygon
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matB_diffuse);		//�޸� Polygon ��ü ���� �ο� 
		glMaterialfv(GL_FRONT, GL_SPECULAR, matB_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, matB_shininess);
		glBegin(GL_POLYGON);		//�޸� Polygon
		{
			for (GLint i = 0; i < pointNum; i++) {
				avgNormal(normal[pointNum + 1], normal[(i + pointNum - 1) % pointNum], normal[i], normal_v[i + pointNum]);
				if (NORMAL_BOOLEAN) {		//vertex normal �ο� 
					glNormal3fv(normal_v[i + pointNum]);
				}
				glTexCoord2fv(texture_arr[i]);	//texture �ο�
				glVertex3f(vertex_b[i][0], vertex_b[i][1], vertex_b[i][2]);
			}
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		//���� Polygon
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matS_diffuse);	//���� Polygon ��ü ���� �ο� 
		glMaterialfv(GL_FRONT, GL_SPECULAR, matS_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, matS_shininess);
		glLineWidth(2.0);
		for (int i = 0; i < pointNum; i++) {
			glBegin(GL_POLYGON);		//���� ���� ��
			{
				if (NORMAL_BOOLEAN) {	//vertex normal �ο� 
					glNormal3fv(normal_v[i]);
				}
				glTexCoord2fv(texture_arr[0]);	//texture �ο�
				glVertex3f(vertex_f[i][0], vertex_f[i][1], vertex_f[i][2]);
				if (NORMAL_BOOLEAN) {	//vertex normal �ο� 
					glNormal3fv(normal_v[i + pointNum]);
				}
				glTexCoord2fv(texture_arr[1]);
				glVertex3f(vertex_b[i][0], vertex_b[i][1], vertex_b[i][2]);
				if (NORMAL_BOOLEAN) {	//vertex normal �ο� 
					GLint temp = i + pointNum + 1;
					if (temp == 2 * pointNum)
						temp -= pointNum;
					glNormal3fv(normal_v[temp]);
				}
				glTexCoord2fv(texture_arr[2]);
				glVertex3f(vertex_b[(i + 1) % pointNum][0], vertex_b[(i + 1) % pointNum][1], vertex_b[(i + 1) % pointNum][2]);
				if (NORMAL_BOOLEAN) {	//vertex normal �ο� 
					glNormal3fv(normal_v[(i + 1) % pointNum]);
				}
				glTexCoord2fv(texture_arr[3]);
				glVertex3f(vertex_f[(i + 1) % pointNum][0], vertex_f[(i + 1) % pointNum][1], vertex_f[(i + 1) % pointNum][2]);

			}
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);				//�������κ��� ��ü �߽ɱ����� ���ἱ
	{
		glVertex3f(0, 0, 0);
		glVertex3f(center_x, center_y, center_z);
	}
	glEnd();

	glBegin(GL_POINTS);				//��ü�� �߽�
	glVertex3f(center_x, center_y, center_z);
	glEnd();
	//vertex normal �� ����ȭ
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

/* ���ڿ��� ȭ�鿡 ǥ�� */
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

/* ������ ������ ȭ�鿡 ǥ���ϴ� �Լ� */
void showInfo() {
	glPushMatrix();                     // save current modelview matrix
	glLoadIdentity();                   // reset modelview matrix

	glMatrixMode(GL_PROJECTION);        // switch to projection matrix
	glPushMatrix();                     // save current projection matrix
	glLoadIdentity();                   // reset projection matrix
	gluOrtho2D(0, current_width, 0, current_height);  // set to orthogonal projection

	float color[4] = { 1, 1, 1, 1 };

	//������ ���
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
	glShadeModel(GL_SMOOTH);    //���� ���̵�
	glEnable(GL_NORMALIZE);		//������ ����ȭ
	glEnable(GL_LIGHTING);		//���� ���
	glEnable(GL_LIGHT0);		//����0 ���
	glEnable(GL_LIGHT1);		//����1 ���

	//���� 0�� ����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	//���� 1�� ����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	//���� 0���� attenuation 
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1);
	//���� 1���� attenuation 
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1);
	//����1���� ��ġ
	
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
}


/* diplay callback func */
void display() {
	//color, depth, stencil ���۸� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	showInfo();

	glDisable(GL_LIGHTING);					//Point �� �׸���
	glColor3f(1, 1, 1);
	glPointSize(7.0);
	if (MOUSE_BOOLEAN) {
		glBegin(GL_POINTS);
		for (GLint i = 0; i < pointNum; i++)
			glVertex3f(vertex_f[i][0], vertex_f[i][1], vertex_f[i][2]);
		glEnd();
	}
	glEnable(GL_LIGHTING);

	if (transfrom_flag == ROTATE_LIGHT) {					//�� ȸ���� draw
		glDisable(GL_LIGHTING);								//���� ������ ������ ���� ����
		drawAxis();											//���� �׸��� ���� ȸ���� ������ ����
		glEnable(GL_LIGHTING);								//������ �ٽ� on
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	//���� 0���� ��ġ�� ����
		glPushMatrix();										//��ü�� ȸ���� ������ ���� �ʱ� ���� ����
		glLoadIdentity();								//��� �ʱ�ȭ
		gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);	//ī�޶� �� ����
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightfv(GL_LIGHT_MODEL_AMBIENT, GL_POSITION, light0_position);
		glRotatef(-(ry_old), 1, 0, 0);					//���� ��ü�� ȸ���� ������
		glRotatef(rx_old, 0, 1, 0);
		drawPolygon();									//��ü�� �׸���.
		glPopMatrix();										//��� ���� ����
	}
	else if (transfrom_flag == ROTATE_OBJ) {				//��ü ȸ���� draw
		drawPolygon();										//��ü�� �׸��� ��ü�� ȸ���� ������ ����
		glPushMatrix();										//���� ȸ���� ������ ���� �ʱ� ���� ����
		glLoadIdentity();								//��� �ʱ�ȭ
		gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);	//ī�޶� �� ����
		light0_position[0] = rx_light_old;				//���� ���� ȸ�� ����
		light0_position[1] = ry_light_old;
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightfv(GL_LIGHT_MODEL_AMBIENT, GL_POSITION, light0_position);
		glRotatef(-(ry_light_old), 1, 0, 0);			//���� �� ȸ�� ������
		glRotatef(rx_light_old, 0, 1, 0);
		glDisable(GL_LIGHTING);
		drawAxis();										//���� �׸���
		glEnable(GL_LIGHTING);
		glPopMatrix();										//��� ���� ����
	}
	else if (POLYGON_BOOLEAN) {
		glPushMatrix();										//���� ȸ���� ������ ���� �ʱ� ���� ����
		glLoadIdentity();								//��� �ʱ�ȭ
		gluLookAt(0., 0., 1., 0., 0., 0., 0., 1., 0.);	//ī�޶� �� ����
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

		drawPolygon();									//��ü�� �׸���.
		glPopMatrix();										//��� ���� ����
	}
	glutSwapBuffers();
}

/* main func */
int main(int argc, char * argv[]) {
	glutInit(&argc, argv);										//glut �ʱ�ȭ �� ����
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(current_width, current_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("����ȯ_openGL_HW6");
	clearInit();
	initLighting();

	glutDisplayFunc(display);									//glut call back �Լ�
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse_Click);
	glutMotionFunc(mouse_Drag);

	GLint subMenu1 = glutCreateMenu(menu);						//submenu ����
	glutAddMenuEntry("Rotate Object", ROTATE_OBJ);
	glutAddMenuEntry("Rotate Light", ROTATE_LIGHT);

	GLint subMenu2 = glutCreateMenu(menu);						//submenu ����
	glutAddMenuEntry("Reset Object", RESET_OBJ);
	glutAddMenuEntry("Reset Light", RESET_LIGHT);
	glutAddMenuEntry("Reset Material", RESET_MAT);

	glutCreateMenu(menu);										//menu����
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
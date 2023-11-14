//  ========================================================================
//  COSC363: Computer Graphics;  University of Canterbury.
//
//  FILE NAME: Assignment1.cpp
//  ========================================================================




#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <climits>
#include <math.h> 
#include <GL/freeglut.h>
#include "loadTGA.h"
using namespace std;
GLuint txId[6];
GLUquadricObj* q;
//GLUquadric* q;

//--Globals ---------------------------------------------------------------
// For OFF flie
float* x, * y, * z;						//vertex coordinates
int* nv, * t1, * t2, * t3, * t4;		//number of vertices and vertex indices of each face
int nvert, nface;						//total number of vertices and faces

//float sceneAngleX = 0, sceneAngleY = 0, sceneAngleZ = 0;	//Scene - Rotation angles about x, y axes
//float xAxis = 0, yAxis = 0, zAxis = 0;

float angleX = 90, angleY = -20, angleZ = 0;						//Rotate object angles about x, y, zaxes
float angle = 0, look_x = 0, look_z = 0, eye_x = 0, eye_z = 15;		//Camera view parameters


//----AAO-3----
//float xAixs = 0, yAixs = 0;
float angle1 = 10.0;		//Rotation angle for viewing
int theta = 20;				// Human Rotation angle
double x_parabolic = 0.65, y_parabolic = 0;
const double step = 0.105;  // parabolic motion per second


//G
const int N = 16;  // Total number of vertices on the base curve

//float vx_init[N] = { 0, 7, 7, 6, 6.3, 4, 4, 5, 5, 6.3, 7.2, 7.6, 8, 0 };
//float vy_init[N] = { 11, 11, 9, 9, 7.3, 6, -6, -6, -8, -8.3, -9, -10, -11, -11 };

float vx_init[N] = { 0, 7, 7, 6, 6.3, 4, 4, 5, 5, 5.776, 6.5, 7.1213, 7.5981, 7.898, 8, 0 };
float vy_init[N] = { 11, 11, 9, 9, 7.3, 6, -6, -6, -8, -8.102, -8.401, -8.8787, -9.5, -10.22, -11, -11};



//----Code--------------------------------------------------------------------------
//Football parabolic position
double parabolicPosition(double x) {
	double a = -4.0;
	double b = -4.0;
	double c = 5.0;
	return a * x * x + b * x + c;
}



void loadTexture()
{
	glGenTextures(6, txId);   //Get 6 texture IDs 

	glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture name for the following OpenGL texture
	loadTGA("illusion.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture name for the following OpenGL texture
	loadTGA("floor1.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[2]);  //Use this texture name for the following OpenGL texture
	loadTGA("grass1.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[3]);  //Use this texture name for the following OpenGL texture
	loadTGA("squad.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[4]);  //Use this texture name for the following OpenGL texture
	loadTGA("illusionS.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[5]);  //Use this texture name for the following OpenGL texture
	loadTGA("Melbourne.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}


//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)
{
	ifstream fp_in;
	int ne;

	fp_in.open(fname, ios::in);
	if (!fp_in.is_open())
	{
		cout << "Error opening mesh file" << endl;
		exit(1);
	}

	fp_in.ignore(INT_MAX, '\n');				//ignore first line
	fp_in >> nvert >> nface >> ne;			    // read number of vertices, polygons, edges (not used)

	x = new float[nvert];                        //create arrays
	y = new float[nvert];
	z = new float[nvert];

	nv = new int[nface];
	t1 = new int[nface];
	t2 = new int[nface];
	t3 = new int[nface];
	t4 = new int[nface];

	for (int i = 0; i < nvert; i++)                         //read vertex list 
		fp_in >> x[i] >> y[i] >> z[i];

	for (int i = 0; i < nface; i++)                         //read polygon list 
	{
		fp_in >> nv[i] >> t1[i] >> t2[i] >> t3[i];
		if (nv[i] == 4)
			fp_in >> t4[i];
		else
			t4[i] = -1;
		//printf("%d,%d,%d,%d,%d \n", *nv, *t1, *t2, *t3, *t4);
	}


	fp_in.close();
	cout << " File successfully read." << endl;
}





//--Function to compute the normal vector of a triangle with index indx ----------
//void normal(int indx)
//{
//	float x1 = x[t1[indx]], x2 = x[t2[indx]], x3 = x[t3[indx]];
//	float y1 = y[t1[indx]], y2 = y[t2[indx]], y3 = y[t3[indx]];
//	float z1 = z[t1[indx]], z2 = z[t2[indx]], z3 = z[t3[indx]];
//	float nx, ny, nz;
//	nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
//	ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
//	nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
//	glNormal3f(nx, ny, nz);
//}

void normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) //Face normal vector
{
	float nx, ny, nz;
	nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
	ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
	nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
	glNormal3f(nx, ny, nz);
}



//------- Initialize OpenGL parameters -----------------------------------
void initialize()
{

	float light0_ambient[4] = { 0.2, 0.2, 0.2, 1.0 };

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	loadTexture();
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);


	loadMeshFile("amesWindow.off");			//Specify mesh file name here
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	//Background colour

	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);



	float white[] = { 1, 1, 1, 1 };
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5);




	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(50, 1., 1., 100);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
}



//AAO
void draw_amesWindow() 
{

	//glColor3f(1.0, 1.0, 1.0);
	for (int indx = 0; indx < nface; indx++)		//draw each face
	{
		//normal(indx);

		if (t1[indx] == 2 || t1[indx] == 9 || t1[indx] == 16 || t1[indx] == 22 || t1[indx] == 26 || t1[indx] == 33 || t1[indx] == 40) {
			glColor3f(0, 0.6, 1);
		}
		else if (t1[indx] == 4 || t1[indx] == 11 || t1[indx] == 18 || t1[indx] == 27 || t1[indx] == 34 || t1[indx] == 41)
		{
			glColor3f(0, 0, 0);
		}
		else {
			glColor3f(1.0, 1.0, 1.0);
		}

		if (nv[indx] == 3)	glBegin(GL_TRIANGLES);
		else				glBegin(GL_QUADS);
		glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
		glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
		glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
		if (nv[indx] == 4)
			glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);

		glEnd();
	}


	//glPopMatrix();
}




void drawMoireQuad1()
{
	
	glDisable(GL_BLEND);
	
	glNormal3f(0, 1, 0);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	glColor3f(1, 1, 1);		//colour

	glBegin(GL_QUADS);			//A single quad

	glTexCoord2f(0, 0);
	glVertex3f(-2.5, 0, 2.5);
	glTexCoord2f(1, 0);
	glVertex3f(2.5, 0, 2.5);
	glTexCoord2f(1, 1);
	glVertex3f(2.5, 0, -2.5);
	glTexCoord2f(0, 1);
	glVertex3f(-2.5, 0, -2.5);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	
	
}
void drawMoireQuad2()
{
	glEnable(GL_BLEND);
	
	glNormal3f(0, 1, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[0]);

	glColor4f(1, 1, 1, 0.8);
	glBegin(GL_QUADS);			//A single quad

	glTexCoord2f(0, 0);
	glVertex3f(-2.5, 0, 2.5);
	glTexCoord2f(1, 0);
	glVertex3f(2.5, 0, 2.5);
	glTexCoord2f(1, 1);
	glVertex3f(2.5, 0, -2.5);
	glTexCoord2f(0, 1);
	glVertex3f(-2.5, 0, -2.5);

	glEnd();
	glDisable(GL_TEXTURE_2D);

}





void drawHuman()
{
	glColor3f(1., 0.78, 0.06);		//Head
	glPushMatrix();
	glTranslatef(0, 7.7, 0);
	glutSolidCube(1.4);
	glPopMatrix();

	glColor3f(1., 0., 0.);			//Torso
	glPushMatrix();
	glTranslatef(0, 5.5, 0);
	glScalef(3, 3, 1.4);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Right leg
	glPushMatrix();
	glTranslatef(-0.8, 4, 0);
	glRotatef(-theta * 2, 1, 0, 0);
	glTranslatef(0.8, -4, 0);
	glTranslatef(-0.8, 2.2, 0);
	glScalef(1, 4.4, 1);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Left leg
	glPushMatrix();
	glTranslatef(0.8, 4, 0);
	glRotatef(theta * 2, 1, 0, 0);
	glTranslatef(-0.8, -4, 0);
	glTranslatef(0.8, 2.2, 0);
	glScalef(1, 4.4, 1);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Right arm
	glPushMatrix();
	glTranslatef(-2, 6.5, 0);
	glRotatef(theta, 1, 0, 0);
	glTranslatef(2, -6.5, 0);
	glTranslatef(-2, 5, 0);
	glScalef(1, 4, 1);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Left arm
	glPushMatrix();
	glTranslatef(2, 6.5, 0);
	glRotatef(theta, -1, 0, 0);
	glTranslatef(-2, -6.5, 0);
	glTranslatef(2, 5, 0);
	glScalef(1, 4, 1);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(1., 1., 1.);
	glPushMatrix();
	glTranslatef(-x_parabolic, y_parabolic, 3);
	//glTranslatef(1, 2, 3.0);
	//glTranslatef(0.5, 2, 3.0);
	glTranslatef(-0.5, 0, 0.0);
	glutSolidSphere(0.6, 20, 20);
	glPopMatrix();

}

void drawHumanBackground()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	gluQuadricTexture(q, GL_TRUE);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(11, -2.5, 6);
	glRotatef(-90, 1, 0, 0);
	gluDisk(q, 0, 4, 400, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	glColor3f(1., 1., 1.);			
	glPushMatrix();
	glTranslatef(10.9, 2.5, 6.05);
	glRotatef(90 + angle1, 0, 1, 0);
	glRotatef(150, 1, 0, 0);
	glScalef(0.3, 1.8, 0.3);
	glutSolidCube(1);
	glPopMatrix();

	glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslatef(10.9, 3.6, 6.05);
	glRotatef(90 + angle1, 0, 1, 0);
	//glRotatef(140, 1, 0, 0);
	glScalef(0.25, 2, 0.25);
	glutSolidCube(1);
	glPopMatrix();


	float light1_pos[4] = { 0, 0, 0, 1 };
	float light1_dir[3] = { 0, -0.1, 0 };
	glPushMatrix();
	glTranslatef(11, 0, 6); 
	glRotatef(angle1, 0, 1, 0);
	glTranslatef(2.5, 0, 0);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);
	glPopMatrix();
}





void drawDisk()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	gluQuadricTexture(q, GL_TRUE);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(0, -5, 10);
	glRotatef(-90, 1, 0, 0);
	gluDisk(q, 7.8, 10, 400, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	gluQuadricTexture(q, GL_TRUE);
	glColor3f(0.8, 0.8, 0.8);
	glPushMatrix();
	glTranslatef(0, -5, 10);
	glRotatef(-90, 1, 0, 0);
	gluDisk(q, 0, 7.8, 400, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, txId[4]);
	//gluQuadricTexture(q, GL_TRUE);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(0, 5, 10);
	glRotatef(-90, 1, 0, 0);
	gluDisk(q, 0, 10, 400, 100);
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
}


void drawEntranceDoor() {

	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[4]);

	glPushMatrix();
	glTranslatef(-3.5, -5, 19.5+20);
	glScalef(7, 9, 7);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(0, 0, 0);
		glTexCoord2f(0, 1);
		glVertex3f(0, 1, 0);
		glTexCoord2f(1, 1);
		glVertex3f(1, 1, 0);
		glTexCoord2f(1, 0);
		glVertex3f(1, 0, 0);
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);



	glColor3f(0.8, 0.5, 0.5);
	glPushMatrix();
	glTranslatef(-4, -5.1, 20 + 20);
	glRotatef(-90, 1, 0, 0);
	glScalef(8, 21, 8);
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 0, 0);
	glEnd();
	glPopMatrix();
}



void drawPillar()
{

	float toRadians = M_PI / 180.0;   //Conversion from degrees to radians
	float angStep = 10.0 * toRadians;  //Rotate base curve in 10 deg steps
	int nSlices = 36;				  //36 slices at 10 deg intervals

	float vx[N], vy[N], vz[N];   //vertex positions
	float wx[N], wy[N], wz[N];



	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[5]);

	glPushMatrix();
	glTranslatef(4.0, -1.6, 17.5);
	glScalef(0.2, 0.3, 0.2);
	//glTranslatef(4.25, -4.5, 18);
	//glScalef(0.3, 0.2, 0.3);
	//glTranslatef(0, -8, 40);
	for (int i = 0; i < N; i++)		//Initialize data everytime the frame is refreshed
	{
		vx[i] = vx_init[i];
		vy[i] = vy_init[i];
		vz[i] = 0;
	}

	for (int j = 0; j < nSlices; j++) { //Transform verts and normals
		for (int i = 0; i < N; i++) {
			wx[i] = cos(angStep) * vx[i] + sin(angStep) * vz[i];
			wy[i] = vy[i];
			wz[i] = -sin(angStep) * vx[i] + cos(angStep) * vz[i];
			//mx[i] = cos(angStep) * nx[i] + sin(angStep) * nz[i];
			//my[i] = ny[i];
			//mz[i] = -sin(angStep) * nx[i] + cos(angStep) * nz[i];

		}

		glBegin(GL_QUAD_STRIP); //Generate quad strip
		for (int i = 0; i < N; i++) {

			glTexCoord2f(j / (float)nSlices, i / (float)(N - 1));
			if (i > 0)
			{
				normal(vx[i - 1], vy[i - 1], vz[i - 1],
						vx[i], vy[i], vz[i],
						wx[i], wy[i], wz[i]);
			}
			glVertex3f(vx[i], vy[i], vz[i]);


			glTexCoord2f((j + 1) / (float)nSlices, i / (float)(N - 1));
			glVertex3f(wx[i], wy[i], wz[i]);

		}
		glEnd();



		for (int i = 0; i < N; i++) { //Update verts
			vx[i] = wx[i]; vy[i] = wy[i]; vz[i] = wz[i];

		}

	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}






//--Display: ----------------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.
void display()
{
	//float lpos[4] = { -10., 10., 10., 1.0 };			//light's position
	float lpos[4] = { 0, 0, 3, 1.0 };			//light's position

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	gluLookAt(eye_x, 0, eye_z, look_x, 0, look_z, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);	//set light position
	
	glTranslatef(0, 0, -3);  //scene move backward

	
	glDisable(GL_LIGHT1);
	drawDisk();
	glEnable(GL_LIGHT1);
	drawHumanBackground();


	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(-11, 0, 6);
	glScalef(2, 2, 2);
	glRotatef(angleY, 0.0, 1.0, 0.0);			//rotate the object about the y-axis
	draw_amesWindow();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);


	//AAO-2
	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	drawMoireQuad1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0.5, 0.2);
	glRotatef(angleZ, 0.0, 0.0, 1.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	drawMoireQuad2();
	glPopMatrix();


	//AAO-3
	glPushMatrix();
	glTranslatef(11, -2.45, 6);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glTranslatef(3, 0, 0);
	glScalef(0.4, 0.4, 0.4);
	drawHuman();
	glPopMatrix();


	drawEntranceDoor();

	

	drawPillar();

	glPushMatrix();
	glTranslatef(-8, 0, 0);
	drawPillar();
	glPopMatrix();

	//glPushMatrix();
	//glTranslatef(-10.5, 0, -16);
	//drawPillar();
	//glPopMatrix();

	//glPushMatrix();
	//glTranslatef(0, 0, -16);
	//drawPillar();
	//glPopMatrix();


	
	//glFlush();
	glutSwapBuffers();
}










//------------ Special key event callback ---------------------------------
// To enable the use of left and right arrow keys to rotate the scene
void special(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) angle -= 0.1;  //Change direction
	else if (key == GLUT_KEY_RIGHT) angle += 0.1;

	else if (key == GLUT_KEY_DOWN)
	{  //Move backward
		eye_x -= 0.1 * sin(angle);
		eye_z += 0.1 * cos(angle);
	}
	else if (key == GLUT_KEY_UP)
	{ //Move forward
		eye_x += 0.1 * sin(angle);
		eye_z -= 0.1 * cos(angle);
	}

	look_x = eye_x + 10 * sin(angle);
	look_z = eye_z - 10 * cos(angle);


	printf("%f, %f, %f, %f, %f\n", angle, eye_x, eye_z, look_x, look_z);
	glutPostRedisplay();
}




void keyboard(unsigned char key, int x, int y) {
	switch (key) 
	{
	case '1':
		angle = -1.000000;
		eye_x = 0.000000;
		eye_z = 9.999981;
		look_x = -8.414711;
		look_z = 4.596959;
		break;

	case '2':
		angle = 0;
		eye_x = 0;
		eye_z = 10.899982;
		look_x = 0;
		look_z = 0.899982;
		break;

	case '3':
		angle = 1.2;
		eye_x = -1.398059;
		eye_z = 7.743512;
		look_x = 7.9223333;
		look_z = 4.119936;
		break;

	case '0':
		angle = 0;
		eye_x = 0;
		eye_z = 15;
		look_x = 0;
		look_z = 0;
		break;

	case '7':
		angle = 0.000000;
		eye_x = 0.000000;
		eye_z = 36.499996;
		look_x = 0.000000;
		look_z = 26.499996;
		break;

	case '8':
		angle = 3.099999;
		eye_x = -0.511452;
		eye_z = 2.710627;
		look_x = -0.095635;
		look_z = 12.701979;
		break;

	case '9':
		angle = 3.099999;
		eye_x = 0.000000;
		eye_z = 20.400021;
		look_x = 0.415815;
		look_z = 30.391373;
		break;

	default:
		break;
	}

	glutPostRedisplay();
}


void myTimer(int value)
{
	angleY += 2.5;
	angleZ++;
	


	static bool reverse = false;
	if (reverse) {
		theta -= 2;
	} else {
		theta += 2;
	}

	if (theta >= 20) {
		reverse = true;
	} else if (theta <= -20) {
		reverse = false;
	}


	angle1--;

	if (reverse) {
		x_parabolic -= step;
	} else {
		x_parabolic += step;
	}
	y_parabolic = parabolicPosition(x_parabolic);


	glutPostRedisplay();
	glutTimerFunc(50, myTimer, 0);
}



//------- Main: Initialize glut window and register call backs -----------
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Model3D");
	initialize();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(50, myTimer, 0);
	glutMainLoop();

	return 0;
}




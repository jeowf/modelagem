//g++ codigo.cpp -lGL -lGLU -lglut

#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>

double tAtual = 0.0; //um ponto é renderizado na reta
int delta = 1; //= 1 ou -1... variação positiva ou negativa de tAtual, quando animacao = 1
int animacao = 1;
double top = 6, bottom = -4, left = -4, right = 6;
int showGuideLines = true;

using namespace std;

//vetor que contem as abscissas dos pontos
vector<double> vx;
//vetor que contem as ordenadas dos pontos
vector<double> vy;

enum {ADICIONARREMOVER, ALTERAR};
int estado = ADICIONARREMOVER;
int pontoAtual = 0;

void inicializacao() {
	glClearColor(0.5, 0.5, 0.5, 0.0);
}


double l(int k, vector<double> & v, double t){
	int n = v.size();
	double r = 1;

	double inc = 1.0/(n-1);

	for (int i = 0; i < v.size();i++){
		if (i!=k){
			r *= ((t-i*inc)/(k*inc-i*inc));
		}
	}

	return r;

}

int fac[2000] = {0};
int comb[2000][2000] = {0};

int factorial(int n)
{
	if (fac[n] == 0)
		fac[n] = (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
  return fac[n];
}

int combination(int r, int n){
	if (comb[r][n] == 0)
		comb[r][n] = factorial(n) / (factorial(r) * factorial(n-r));
	return comb[r][n];
}

double lagrange_interpolacao(vector<double> v, double t) {
	int n = v.size();
	if(n < 2) return 0;
	double ret = 0;

	for (int i = 0; i < n;i++){
		ret += l(i, v, t)*v[i];
	}

	return ret;
}

double b(int i, int n, double t){
	return combination(i,n)*pow(t,i)*pow((1-t),(n-i));
}
double naive_bezier_interpolacao(vector<double> & v, double t){
	int n = v.size();

	double ret = 0;
	for (int i = 0; i < n; ++i)
	{
		ret+=b(i,n-1,t)*v[i];
	}

	return ret;
}


double castejau_interpolacao(vector<double> & v, double t){
	// algoritmo de De Casteljau
	if (v.size() == 0)
		return 0;
	int n = v.size() - 1;
	double ret = 0;
	vector<double> q(n+1);

	for (int i = 0; i <= n;i++)
		q[i] = v[i];
	

	for (int i = 1; i <= n; i++){
		for (int j = 0; j <= (n-i); j++){

			q[j] = (1-t)*q[j] + t*q[j+1];
		}
	}

	return q[0];
}

double g(double t){
	if (t >= 0 and t <= 1)
		return (t*t)/2.0;
	else if (t >=1 and t <= 2)
		return 0.75 - pow((t-1.5),2);
	else if (t >= 2 and t <=3 )
		return pow((3-t),2)/2.0;
	else
		return 0.0;
}

double spline_interpolacao(vector<double> & v, double t){
	if (v.size() == 0)
		return 0;

	int n = v.size();
	t = 2 + t*(n-1-1);

	double ret = 0;

	for (int i = 0; i < n; i++){
		ret += v[i]*g(t-i);
	}


	return ret;
}

//compile com g++ codigo.cpp -lGL -lGLU -lglut
//preencha a seguinte função
//seja n a quantidade de pontos (tamanho do vetor)
//a função deve retornar o valor v(t), tal que:
//	quando t = 0, retorna v[0]
//	quando t = 1, retorna v[n-1]
double interpolacao(vector<double> v, double t) {
	return spline_interpolacao(v,t);
}


void funcaoDisplay() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, right, bottom, top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);

	for(int i = 0; i < (int)vx.size(); i++) {
		if(i == pontoAtual && estado == ALTERAR) {
			glColor3f(0, 0, 1);
			glPointSize(6.0);
		} else {
			glColor3f(1, 1, 1);
			glPointSize(3.0);
		}
		glBegin(GL_POINTS);
		glVertex2f(vx[i], vy[i]);
		glEnd();
	}

	//renderização de tAtual
	double px = interpolacao(vx, tAtual);
	double py = interpolacao(vy, tAtual);

	if(vx.size() > 2) {
		glColor3f(1, 1, 1);
		glPointSize(6.0);
		glBegin(GL_POINTS);
		glVertex2f(px, py);
		glEnd();
	}

	if(vx.size() > 2 && showGuideLines) {
		for(int i = 0; i < (int)vx.size()-1; i++) {
			glColor3f(0, 0, 1);
			glBegin(GL_LINES);
			glVertex2f(vx[i], vy[i]);
			glVertex2f(vx[i+1], vy[i+1]);
			glEnd();
		}
	}

	int n = 100*vx.size();
	for(int i = 0; i < n-1; i++) {
		double t = ((double)i)/(n-1);
		double tnext = ((double)(i+1))/(n-1);
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		glVertex2f(interpolacao(vx, t), interpolacao(vy, t));
		glVertex2f(interpolacao(vx, tnext), interpolacao(vy, tnext));
		glEnd();
	}


	//escreve um texto na tela para acompanhar a coordenada para t = tAtual
	if(vx.size() > 2) {
		glRasterPos2f(px+0.1, py+0.1);
		char texto[60];
		sprintf(texto, "t = %.2f, (x, y) = (%.2f, %.2f)", tAtual, px, py);
		for(int i = 0; i < (int)strlen(texto); i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
	}

	glutSwapBuffers();
	glFlush();
}

void funcaoKeyboard(unsigned char key, int , int ) {

	if(key == 'q') {
		exit(-1);
	}
	if(!animacao) {
		if(key == 'a') {
			tAtual -= 0.01;
		}
		if(key == 'd') {
			tAtual += 0.01;
		}
	}
	if(key == 's')
		animacao = 1 - animacao;
	if(key == 'g')
		showGuideLines = !showGuideLines;
	glutPostRedisplay();
}

void temporizador() {
	glutPostRedisplay();
	if(animacao) {
		tAtual += 0.005*delta;
		if(tAtual >= 1) {
			tAtual = 1;
			delta = -1;
		}
		if(tAtual < 0) {
			tAtual = 0;
			delta = 1;
		}
	}
}

void funcaoMouseMotion(int x, int y) {

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble worldX, worldY, worldZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (double)x;
	winY = (double)viewport[3] - (double)y;
	winZ = 0;

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);

	if(estado == ALTERAR) {
		vx[pontoAtual] = worldX;
		vy[pontoAtual] = worldY;
	}
}

void funcaoMouse(int button, int state, int x, int y) {

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble worldX, worldY, worldZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (double)x;
	winY = (double)viewport[3] - (double)y;
	winZ = 0;

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);

	if(state == GLUT_UP) {
		if(estado == ADICIONARREMOVER) {
			if(button == GLUT_LEFT_BUTTON) {
				vx.push_back(worldX);
				vy.push_back(worldY);
			}
			if(button == GLUT_RIGHT_BUTTON && vx.size() > 0) {
				vx.pop_back();
				vy.pop_back();
			}
		} else {
			if(button == GLUT_LEFT_BUTTON) {
				vx[pontoAtual] = worldX;
				vy[pontoAtual] = worldY;
				estado = ADICIONARREMOVER;
			}
		}
	} if(state == GLUT_DOWN) {
		double menorDistancia = -1;
		int pontoMaisProximo = 0;
		for(int i = 0; i < (int)vx.size(); i++) {
			double distancia1 = fabs(vx[i] - worldX) + fabs(vy[i] - worldY);
			if(i == 0 || distancia1 < menorDistancia) {
				menorDistancia = distancia1;
				pontoMaisProximo = i;
			}
		}
		if(menorDistancia < 0.2 && (int)vx.size() > 0 && estado == ADICIONARREMOVER) {
			estado = ALTERAR;
			pontoAtual = pontoMaisProximo;
		}
	}
	glutPostRedisplay();
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Splines");
	glutKeyboardFunc(funcaoKeyboard);
	glutMouseFunc(funcaoMouse);
	glutMotionFunc(funcaoMouseMotion);
	glutDisplayFunc(funcaoDisplay);
	glutIdleFunc(temporizador);
	inicializacao();

	glutMainLoop();

	return 0;
}


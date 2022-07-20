#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

#define PI M_PI
#define pi M_PI

#define AXES true
#define GRID true
#define A_STEP 0.5
#define STEP 1.0

class camera_t {
public:
  double height;
  double angle;
} camera;

class point {
public:
  double x;
  double y;
  double z;

  point(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  point() {}

  void rotate3D(point constant, double angle) {

    point temp;

    temp.x = constant.y * this->z - constant.z * this->y;
    temp.y = constant.z * this->x - constant.x * this->z;
    temp.z = constant.x * this->y - constant.y * this->x;

    this->x =
        this->x * cos(angle * PI / 180.0) + temp.x * sin(angle * PI / 180.0);
    this->y =
        this->y * cos(angle * PI / 180.0) + temp.y * sin(angle * PI / 180.0);
    this->z =
        this->z * cos(angle * PI / 180.0) + temp.z * sin(angle * PI / 180.0);
  }
};

void drawAxes() {
  if (AXES) {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);

    glVertex3f(100, 0, 0);
    glVertex3f(-100, 0, 0);

    glVertex3f(0, -100, 0);
    glVertex3f(0, 100, 0);

    glVertex3f(0, 0, 100);
    glVertex3f(0, 0, -100);

    glEnd();
  }
}

void drawGrid() {
  if (GRID) {
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINES);

    for (int i = -10; i <= 10; i++) {
      // lines parallel to Y-axis
      glVertex3f(i * 10, -90, 0);
      glVertex3f(i * 10, 90, 0);

      // lines parallel to X-axis
      glVertex3f(-90, i * 10, 0);
      glVertex3f(90, i * 10, 0);
    }
    glEnd();
  }
}
double sqrt2 = sqrt(2);
point pos(200, 200, 75), u(0, 0, 1), r(-1 / sqrt2, 1 / sqrt2, 0),
    l(-1 / sqrt2, -1 / sqrt2, 0);
double angle;
double maxlen = 60.0;
double len = 40.0;

void display();

void init() {

  camera.height = 150.0;
  camera.angle = 1.0;
  angle = 0;

  glClearColor(0.0, 0.0, 0.0, 1.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(80, 1, 1, 1000.0);
}

void timer(int);

void reshape(int, int);

void keyboardController(unsigned char key, int x, int y) {
  switch (key) {
  case '1':
    // LEFT
    l.rotate3D(u, A_STEP);
    r.rotate3D(u, A_STEP);
    break;
  case '2':
    // RIGHT
    l.rotate3D(u, -A_STEP);
    r.rotate3D(u, -A_STEP);
    break;
  case '3':
    // UP
    u.rotate3D(r, A_STEP);
    l.rotate3D(r, A_STEP);
    break;
  case '4':
    // DOWN
    u.rotate3D(r, -A_STEP);
    l.rotate3D(r, -A_STEP);
    break;
  case '5':
    // CLOCKWISE
    u.rotate3D(l, -A_STEP);
    r.rotate3D(l, -A_STEP);
    break;
  case '6':
    // ANTICLOCKWISE
    u.rotate3D(l, A_STEP);
    r.rotate3D(l, A_STEP);
    break;
  default:
    break;
  }
}

void specialKeyController(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_DOWN:
    pos.x -= l.x;
    pos.y -= l.y;
    pos.z -= l.z;
    break;
  case GLUT_KEY_UP:
    pos.x += l.x;
    pos.y += l.y;
    pos.z += l.z;
    break;
  case GLUT_KEY_RIGHT:
    pos.x += r.x;
    pos.y += r.y;
    pos.z += r.z;
    break;
  case GLUT_KEY_LEFT:
    pos.x -= r.x;
    pos.y -= r.y;
    pos.z -= r.z;
    break;
  case GLUT_KEY_PAGE_UP:
    pos.z += STEP;
    break;
  case GLUT_KEY_PAGE_DOWN:
    pos.z -= STEP;
    break;
  case GLUT_KEY_HOME:
    if (len < 60.0)
      len++;
    break;
  case GLUT_KEY_END:
    if (len > 0)
      len--;
    break;
  }
}

void mouseController(int button, int state, int x, int y) {
  switch (button) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
      // TODO
    }
    break;
  default:
    break;
  }
}

void animate() {
  angle += A_STEP;
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(0, 0);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE |
                      GLUT_RGB); // Depth, Double buffer, RGB color

  glutCreateWindow("1705069_1");

  init();

  glEnable(GL_DEPTH_TEST);

  glutDisplayFunc(display);
  glutIdleFunc(animate);

  glutKeyboardFunc(keyboardController);
  glutSpecialFunc(specialKeyController);
  glutMouseFunc(mouseController);

  glutMainLoop();

  return 0;
}

void drawOneEighthSphere(double radius) {
  int slices = 100;
  int stacks = 100;
  glColor3f(1.0, 0.6, 0.2);
  struct point points[stacks + 5][slices + 5];
  int i, j;
  double h, r;
  // generate points
  for (i = 0; i <= stacks; i++) {
    h = radius * sin(((double)i / (double)stacks) * (pi / 2));
    r = radius * cos(((double)i / (double)stacks) * (pi / 2));
    for (j = 0; j <= slices; j++) {
      points[i][j].x = r * cos(((double)j / (double)slices) * 0.5 * pi);
      points[i][j].y = r * sin(((double)j / (double)slices) * 0.5 * pi);
      points[i][j].z = h;
    }
  }
  // draw quads using generated points

  for (i = 0; i < stacks; i++) {
    for (j = 0; j < slices; j++) {
      glBegin(GL_QUADS);
      {
        // upper hemisphere
        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y,
                   points[i + 1][j + 1].z);
        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
      }
      glEnd();
    }
  }
}

void drawCylinder(double radius, double height, int slices, int stacks) {
  glColor3f(0, 0, 1);
  struct point points[100][100];
  int i, j;
  double h, r;

  for (i = 0; i <= stacks; i++) {
    h = height * sin(((double)i / (double)stacks) * (pi / 2.0));

    r = radius;
    for (j = 0; j <= slices; j++) {
      points[i][j].x = r * cos(((double)j / (double)slices) / 2.0 * pi);
      points[i][j].y = r * sin(((double)j / (double)slices) / 2.0 * pi);
      points[i][j].z = h;
    }
  }

  for (i = 0; i < stacks; i++) {

    for (j = 0; j < slices; j++) {
      glBegin(GL_QUADS);
      {
        // Upper Hemisphere
        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y,
                   points[i + 1][j + 1].z);
        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);

        // Lower Hemisphere
        glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y,
                   -points[i + 1][j + 1].z);
        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
      }
      glEnd();
    }
  }
}

void draw12Cylinder() {
  // 1
  glPushMatrix();
  glTranslated(len, len, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 2
  glPushMatrix();
  glTranslated(len, -len, 0);
  glRotated(-90, 0, 0, 1);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 3

  glPushMatrix();
  glTranslated(-len, len, 0);
  glRotated(90, 0, 0, 1);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 4
  glPushMatrix();
  glTranslated(-len, -len, 0);
  glRotated(180, 0, 0, 1);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 5
  glPushMatrix();
  glTranslated(len, 0, len);
  glRotated(90, 1, 0, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 6
  glPushMatrix();
  glTranslated(-len, 0, len);
  glRotated(90, 0, 0, 1);
  glRotated(-90, 0, 1, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 7
  glPushMatrix();
  glTranslated(len, 0, -len);
  glRotated(-90, 1, 0, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 8
  glPushMatrix();
  glTranslated(-len, 0, -len);
  glRotated(90, 0, 0, 1);
  glRotated(90, 0, 1, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 9

  glPushMatrix();
  glTranslated(0, len, len);
  glRotated(90, 0, 0, 1);
  glRotated(90, 1, 0, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 10
  glPushMatrix();
  glTranslated(0, len, -len);
  glRotated(90, 0, 0, 1);
  glRotated(-90, 1, 0, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 11
  glPushMatrix();
  glTranslated(0, -len, len);
  glRotated(-90, 0, 0, 1);
  glRotated(90, 1, 0, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();

  // 12
  glPushMatrix();
  glTranslated(0, -len, -len);
  glRotated(-90, 0, 0, 1);
  glRotated(-90, 1, 0, 0);
  drawCylinder(maxlen - len, len, 50, 50);
  glPopMatrix();
}

void drawSquare(double a) {
  glColor3f(1, 0, 0);
  glBegin(GL_QUADS);
  {
    glVertex3f(a, a, 0);
    glVertex3f(a, -a, 0);
    glVertex3f(-a, -a, 0);
    glVertex3f(-a, a, 0);
  }
  glEnd();
}

void draw6Square() {
  // 1

  glPushMatrix();
  glTranslated(0, 0, maxlen);
  drawSquare(len);
  glPopMatrix();

  // 2
  glPushMatrix();
  glTranslated(0, 0, -maxlen);
  drawSquare(len);
  glPopMatrix();

  // 3
  glPushMatrix();
  glTranslated(0, maxlen, 0);
  glRotated(90, 1, 0, 0);
  drawSquare(len);
  glPopMatrix();

  // 4
  glPushMatrix();
  glTranslated(0, -maxlen, 0);
  glRotated(90, 1, 0, 0);
  drawSquare(len);
  glPopMatrix();

  // 5
  glPushMatrix();
  glTranslated(maxlen, 0, 0);
  glRotated(90, 0, 1, 0);
  drawSquare(len);
  glPopMatrix();

  // 6
  glPushMatrix();
  glTranslated(-maxlen, 0, 0);
  glRotated(90, 0, 1, 0);
  drawSquare(len);
  glPopMatrix();
}

void draw8Sphere() {

  // 1
  glPushMatrix();
  glTranslated(len, len, len);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();

  // 2
  glPushMatrix();
  glTranslated(len, -len, len);
  glRotated(-90, 0, 0, 1);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();

  // 3
  glPushMatrix();
  glTranslated(-len, len, len);
  glRotated(90, 0, 0, 1);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();

  // 4
  glPushMatrix();
  glTranslated(-len, -len, len);
  glRotated(180, 0, 0, 1);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();

  // 5
  glPushMatrix();
  glTranslated(len, len, -len);
  glRotated(90, 0, 1, 0);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();

  // 6
  glPushMatrix();
  glTranslated(len, -len, -len);
  glRotated(180, 1, 0, 0);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();

  // 7
  glPushMatrix();
  glTranslated(-len, len, -len);
  glRotated(-180, 0, 1, 0);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();

  // 8
  glPushMatrix();
  glTranslated(-len, -len, -len);
  glRotated(180, 1, 1, 0);
  glRotated(180, 0, 0, 1);
  drawOneEighthSphere(maxlen - len);
  glPopMatrix();
}

void display() {
  // Setup
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0, 0, 0); // color black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Perspective
  gluLookAt(pos.x, pos.y, pos.z, l.x + pos.x, l.y + pos.y, l.z + pos.z, u.x,
            u.y, u.z);
  glMatrixMode(GL_MODELVIEW);

  drawAxes();
  drawGrid();
  draw8Sphere();
  draw12Cylinder();
  draw6Square();

  glutSwapBuffers();
}

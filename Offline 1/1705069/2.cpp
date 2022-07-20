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

#define WHEEL_RADIUS 20
#define WHEEL_WIDTH 10

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

  void rotate2D(double angle) {
    point perp;
    perp.x = -this->y;
    perp.y = this->x;
    perp.z = this->z;

    this->x =
        this->x * cos(angle * PI / 180.0) + perp.x * sin(angle * PI / 180.0);
    this->y =
        this->y * cos(angle * PI / 180.0) + perp.y * sin(angle * PI / 180.0);
    this->z =
        this->z * cos(angle * PI / 180.0) + perp.z * sin(angle * PI / 180.0);
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
double rotationAngle = 0.0;

point wheel(0, 0, WHEEL_RADIUS), direction(1, 0, 0);

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
  double angleChange = (360 * STEP) / (2 * PI * WHEEL_RADIUS);
  switch (key) {
  case 's':
    wheel.x += STEP * direction.x;
    wheel.y += STEP * direction.y;
    rotationAngle += angleChange;
    break;
  case 'w':
    wheel.x -= STEP * direction.x;
    wheel.y -= STEP * direction.y;
    rotationAngle -= angleChange;
    break;
  case 'a':
    direction.rotate2D(1.0);
    break;
  case 'd':
    direction.rotate2D(-1.0);
    break;
  default:
    break;
  }
}

void specialKeyController(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_DOWN:
    camera.height -= 1.0;
    break;
  case GLUT_KEY_UP:
    camera.height += 1.0;
    break;
  case GLUT_KEY_RIGHT:
    camera.angle += A_STEP;
    break;
  case GLUT_KEY_LEFT:
    camera.angle -= A_STEP;
    break;
  default:
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

  glutCreateWindow("1705069_2");

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

void drawCylinder(double radius, double height, int slices, int stacks) {
  struct point points[100][100];

  double h, r;

  for (int i = 0; i <= stacks; i++) {
    h = height * sin(((double)i / (double)stacks) * (pi / 2.0));

    r = radius;
    for (int j = 0; j <= slices; j++) {
      points[i][j].x = r * cos(((double)j / (double)slices) * 2.0 * pi);
      points[i][j].y = r * sin(((double)j / (double)slices) * 2.0 * pi);
      points[i][j].z = h;
    }
  }

  for (int i = 0; i < stacks; i++) {
    glColor3f(1, 1, 0.5);
    for (int j = 0; j < slices; j++) {
      glBegin(GL_QUADS);
      {

        double shade;
        if (i < slices / 2)
          shade = 2 * (double)i / (double)slices;
        else
          shade = 2 * (1.0 - (double)i / (double)slices);

        glColor3f(shade, shade, shade);

        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y,
                   points[i + 1][j + 1].z);
        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);

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

void drawWheel();

void display() {
  // Setup
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0, 0, 0); // color black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Perspective

  gluLookAt(200 * cos(camera.angle), 200 * sin(camera.angle), camera.height, 0,
            0, 0, 0, 0, 1);

  glMatrixMode(GL_MODELVIEW);

  drawAxes();
  drawGrid();

  drawWheel();

  glutSwapBuffers();
}

void drawWheel() {
  glTranslated(wheel.x, wheel.y, wheel.z); 
  glRotated((atan2(direction.y, direction.x) * 180) / pi, 0, 0,
            1);                       
  glRotated(90, 1, 0, 0);             
  glRotated(-rotationAngle, 0, 0, 1); 
  drawCylinder(WHEEL_RADIUS, WHEEL_WIDTH, 50, 60);

  glColor3f(1.0, 0.0, 0.0);

  // Rectangles for the rim
  glBegin(GL_QUADS);

  glVertex3f(WHEEL_RADIUS, 0, WHEEL_WIDTH / 2);
  glVertex3f(WHEEL_RADIUS, 0, -WHEEL_WIDTH / 2);
  glVertex3f(-WHEEL_RADIUS, 0, -WHEEL_WIDTH / 2);
  glVertex3f(-WHEEL_RADIUS, 0, WHEEL_WIDTH / 2);

  glEnd();

  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_QUADS);

  glVertex3f(0, WHEEL_RADIUS, WHEEL_WIDTH / 2);
  glVertex3f(0, WHEEL_RADIUS, -WHEEL_WIDTH / 2);
  glVertex3f(0, -WHEEL_RADIUS, -WHEEL_WIDTH / 2);
  glVertex3f(0, -WHEEL_RADIUS, WHEEL_WIDTH / 2);

  glEnd();
}

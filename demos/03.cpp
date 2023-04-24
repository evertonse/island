
// Por Beatriz Trinchão Andrade
// Baseado no código disponível em http://www.glprogramming.com/red/chapter01.html

// Compilar: g++  demos/03_rotacao_modelo_mouse.cpp -omain -lGL -lGLU -lglut
// rodar: ./03_rotacao_modelo_mouse
// Windows: g++  demos/03_rotacao_modelo_mouse.cpp -omain -DUNICODE -Ivendor/  -lopengl32 -lglu32 -lgdi32

#if defined(_WIN32) || defined(_WIN64)
  #define SWIN_IMPLEMENTATION
  #include "swin.hpp"
#else
  #include <GL/glut.h>
  #include <GL/glu.h>
  #include <GL/gl.h>
#endif


#include <stdlib.h>
#include <stdio.h>


void draw_cube();
void spinDisplay(void);

static GLfloat spin = 0.0;

void init(void)
{
  // define a cor de limpeza
  glClearColor(1.0, 1.0, 1.0, 0.0);

  // modo de desenho dos polígonos
  //[GL_FRONT,GL_BACK] x [GL_LINE, GL_POINT, GL_FILL]
  glPolygonMode(GL_BACK, GL_LINE);

  // modo de sombreamento
  //[GL_FLAT,GL_SMOOTH]
  glShadeModel(GL_SMOOTH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(120.0, 1, 0.5, 1000.0);
  gluPerspective(60.0f, 1.0f, 0.1f, 100.0f); // Use a 60 degree field of view, aspect ratio of 1, near clipping plane at 0.1 units, and far clipping plane at 100 units
  // gluPerspective(40.0, 1, 0.5, 1000.0);

  // Projeçao ortográfica
  //glOrtho(-1.5, 1.0, -1.5, 1.0, 0.5, 1000);

  gluLookAt(0.0, 0.0, 1.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

bool should_spin = false;

void display(void)
{ 
  if (should_spin) {
    spinDisplay(); 
  }
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Mais sobre os comandos de push e pop matrices aqui:
  // http://www.opengl.org/sdk/docs/man2/xhtml/glPushMatrix.xml
  glPushMatrix();
  glRotatef(spin, 1.0, 1.0, 0.0);
  glColor3f(0.7, 0.7, 0.7);

  // Mecanismo similar ao glPushMatrix
  // no topo da pilha é colocada uma cópia de um conjunto de atributos,
  // incluindo a cor
  glPushAttrib(GL_CURRENT_BIT);

  glBegin(GL_TRIANGLE_FAN);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.5, -0.5, 0.0);

  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.5, -0.5, 0.0);

  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.5, 0.5, 0.0);

  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(-0.5, 0.5, 0.0);

  glEnd();
  draw_cube();
  // desenha um cubo
  // coloque o glPopMatrix abaixo dessa função. O que muda? Por quê?

  glTranslatef(-1.5, -1.5, 0);

  // retira cabeça da pilha de atributos
  glPopAttrib();

  // retira a matriz de transformação da cabeça da pilha
  glPopMatrix();

}

// função que incrementa o ângulo de rotação
void spinDisplay(void)
{
  spin = spin + 2.0;
  if (spin > 360.0)
    spin = spin - 360.0;

}
void draw_cube() {
    // Draw a simple cube
    glBegin(GL_QUADS);

    // Front face
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Top face
    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Bottom face
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Left face
    glColor3f(1.0f, 0.0f, 1.0f); // Magenta
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Right face
    glColor3f(0.0f, 1.0f, 1.0f); // Cyan
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    glEnd();
}
// função chamada quando a tela é redimensionada
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

// evento que trata clique do mouse
void mouse(swin::Mouse m)
{
  switch (m.button)
  {
    using swin::MouseButton;  
    case MouseButton::LEFT:
    if (m.state == true)
      // faz com que a função spinDisplay seja chamada sempre que o
      // programa estiver ocioso
      should_spin = true;
    if (m.state == false)
      should_spin = false;
    break;
  default:
    break;
  }
}

auto winmain() {
  auto wind = swin::Window("hi", 600, 600);
  wind.init(init);
  wind.display(display);
  wind.mouse(mouse);
  wind.resize(reshape);
  wind.loop();
}

int main(int argc, char **argv) {
  #if defined(_WIN32) || defined(_WIN64)
    winmain();
  #else
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(500, 100);
  glutCreateWindow(argv[0]);

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);

  glutMainLoop();

  #endif
  return 0;
}

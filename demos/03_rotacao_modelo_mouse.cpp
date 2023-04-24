
//Por Beatriz Trinchão Andrade
//Baseado no código disponível em http://www.glprogramming.com/red/chapter01.html

//Compilar Linux: g++ -o 03_rotacao_modelo_mouse 03_rotacao_modelo_mouse.cpp -lGL -lGLU -lglut
//Compilar Windows: g++ -o 03_rotacao_modelo_mouse 03_rotacao_modelo_mouse.cpp -lopengl32 -lglu32 -lfreeglut

//rodar: ./03_rotacao_modelo_mouse

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
static GLfloat spin = 0.0;

void init(void) 
{
   //define a cor de limpeza 
   glClearColor (1.0, 1.0, 1.0, 0.0);
   
   //modo de desenho dos polígonos
   //[GL_FRONT,GL_BACK] x [GL_LINE, GL_POINT, GL_FILL]
   glPolygonMode(GL_BACK, GL_LINE);
   
   //modo de sombreamento 
   //[GL_FLAT,GL_SMOOTH]
   glShadeModel (GL_SMOOTH);
 
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   
   //Coloca projeção perspectiva, veja o efeito ao alterar o ângulo: 
   //gluPerspective(120.0, 1, 0.5, 1000.0);  
   //gluPerspective(40.0, 1, 0.5, 1000.0);
   
   //Projeçao ortográfica
   glOrtho(-1.5, 1.0, -1.5, 1.0, 0.5, 1000);
    
   gluLookAt (0.0, 0.0, 1.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();    
    
   //Mais sobre os comandos de push e pop matrices aqui: 
   //http://www.opengl.org/sdk/docs/man2/xhtml/glPushMatrix.xml
   glPushMatrix();  
   glRotatef(spin, 1.0, 1.0, 0.0);
   glColor3f (0.7, 0.7, 0.7);
   
   //Mecanismo similar ao glPushMatrix
   //no topo da pilha é colocada uma cópia de um conjunto de atributos,
   //incluindo a cor
   glPushAttrib(GL_CURRENT_BIT);
   
   glBegin(GL_TRIANGLE_FAN);
        glColor3f (1.0, 1.0, 1.0);
        glVertex3f (-0.5, -0.5, 0.0);
        
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f (0.5, -0.5, 0.0);
        
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f (0.5, 0.5, 0.0);
        
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f (-0.5, 0.5, 0.0);
        
    glEnd(); 
  

   
   //desenha um cubo
   //coloque o glPopMatrix abaixo dessa função. O que muda? Por quê?
  
  glTranslatef (-1.5, -1.5, 0);
 glutWireCube(0.5);

   //retira cabeça da pilha de atributos
   glPopAttrib();

    //retira a matriz de transformação da cabeça da pilha
   glPopMatrix();

   //troca de buffers, o flush é implícito aqui
   glutSwapBuffers();
}

//função que incrementa o ângulo de rotação
void spinDisplay(void)
{
   spin = spin + 2.0;
   if (spin > 360.0)
      spin = spin - 360.0;
      
   //faz com que a tela seja redesenhada na proxima iteração do loop
   //do glut (com isso a função display é chamada)
   glutPostRedisplay();
}


//função chamada quando a tela é redimensionada 
void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
     
}

//evento que trata clique do mouse
void mouse(int button, int state, int x, int y) 
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN)
            //faz com que a função spinDisplay seja chamada sempre que o 
            //programa estiver ocioso
            glutIdleFunc(spinDisplay);
         break;
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN)
            //faz com que nenhuma função seja chamada quando o 
            //programa estiver ocioso
            glutIdleFunc(NULL);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (500, 100);
   glutCreateWindow (argv[0]);
   
   init ();
   
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutMouseFunc(mouse);
   
   glutMainLoop();
   
   return 0;
}

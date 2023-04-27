
//Por Beatriz Trinchão Andrade
//Baseado no código disponível em http://www.glprogramming.com/red/chapter01.html

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdlib.h>

static GLfloat spin = 0.0;

void init(void) 
{
   //inicializa a cor de limpeza 
   glClearColor (0.0, 0.0, 0.0, 0.0);
   
   //modo de sombreamento. Leiam mais sobre isso aqui: 
   //https://www.glprogramming.com/red/chapter04.html	
   glShadeModel (GL_SMOOTH);
 
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
  
   //Mais sobre os comandos de push e pop matrices aqui: 
   //http://www.opengl.org/sdk/docs/man2/xhtml/glPushMatrix.xml
        
   glPushMatrix();
   
   glRotatef(spin, 1.0, 1.0, 0.0);
   glColor3f (0.7, 0.7, 0.7);
   
   //Mecanismo similar ao glPushMatrix
   //no topo da pilha é colocada uma cópia de um conjunto de atributos,
   //incluindo a cor
   glPushAttrib(GL_CURRENT_BIT);
       
   //USO DE VERTEX ARRAYS
   //preenche vetores com informações sobre os vértices 
   static GLfloat vertices[] = {-0.5, -0.5, 0.0, 0.5, -0.5, 0.0, 0.5, 0.5, 0.0, -0.5, 0.5, 0.0};
   static GLfloat colors[] = {1.0, 1.0, 1.0, 1.0, 0.0, 0.0 ,0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

   //ativa arrays que serão usados
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);
   
   //associa dados aos arrays
   glColorPointer   (3, GL_FLOAT, 0, colors);
   glVertexPointer  (3, GL_FLOAT, 0, vertices);

   //Formas de desenho

   //1.Desenho por vértice
   /*
   glBegin(GL_TRIANGLE_FAN);
		glArrayElement (0);
		glArrayElement (1);
		glArrayElement (2);
		glArrayElement (3);
   glEnd();
   */
    
   //2. Desenho na ordem definida por um array de índices -> glBegin e glEnd implícitos  
   static GLubyte indices[] = {1,2,3,0};    
   glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, indices);
   
  
   //3. Desenho na ordem de declaração -> glBegin e glEnd implícitos
   //glDrawArrays(GL_TRIANGLE_FAN, 1, 3);
 
 
   //retira topo da pilha de atributos
   glPopAttrib();
   //retira a matriz de transformação do topo da pilha
   glPopMatrix();

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   
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
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
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
   glutInitWindowSize (250, 250); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   
   init ();
   
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutMouseFunc(mouse);
   
   glutMainLoop();
   
   return 0;
}


//Por Beatriz Trinchão Andrade
//Baseado em 
//http://www.glprogramming.com/red/chapter01.html e http://www.glprogramming.com/red/chapter09.html
//makeStripeImage: baseado no livro do Shirley, cap 11

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 64
#define STRIPE_WIDTH 5

static GLfloat spin = 0.0;
static GLubyte checkImage[IMAGE_HEIGHT][IMAGE_WIDTH][4];
static GLuint texName;
	
//cria uma imagem com listras de largura w
//uso da função seno para fazer a pintura variar em função de j
void makeStripeImage(int w)
{
	int i, j, r, g, b;
	//w controla a largura das faixas
	for (i = 0; i < IMAGE_HEIGHT; i++) {
		for (j = 0; j < IMAGE_WIDTH; j++) {
			if (sin(M_PI*j/w) > 0){
				r=255; g=255; b=255;			
			}
			else{
				r=255; g=0; b=0;	
			}
			checkImage[i][j][0] = (GLubyte) r;
			checkImage[i][j][1] = (GLubyte) g;
			checkImage[i][j][2] = (GLubyte) b;
			checkImage[i][j][3] = (GLubyte) 255;
		}
	}
}

void init(void) 
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_DEPTH_TEST);
	makeStripeImage(STRIPE_WIDTH);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //declara um objeto de textura
	glGenTextures(1, &texName);
	
	//cria e usa objetos de textura 
	//podem ser trocados ou removidos (remoção: texName=0)
	glBindTexture(GL_TEXTURE_2D, texName);
	
	//todas as alterações feitas a seguir afetam o objeto associado
	//à textura
	
	//como a textura será tratada se não há mapeamento direto 
	//entre pixels e coordenadas de textura (GL_REPEAT ou GL_CLAMP)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	//magnification e minification filters
	//GL_NEAREST: texel com coordenadas mais próximas do centro do pixel 
	//é usado
	//testar GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//define uma textura bidimensional
	/*void glTexImage2D(
	 * GLenum target, 
	 * GLint level, 
	 * GLint internalFormat, (número de componentes)
	 * GLsizei width, 
	 * GLsizei height, 
	 * GLint border,
	 * GLenum format, 
	 * GLenum type,    (tipos dos dados)
	 * const GLvoid *pixels);*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, 
	    IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);	
	    
	
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//ativa textura
	glEnable(GL_TEXTURE_2D);
		
	//primitivas usam cores diretamente da textura
	//Exercício: atribuir cores aos vértices e testar outros modos 
	//onde eles também são levados em conta
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);    
	
	glBindTexture(GL_TEXTURE_2D, texName);
	
   glPushMatrix(); 
   glRotatef(spin, 1.0, 1.0, 0.0);
   glPushAttrib(GL_CURRENT_BIT);
   
   //preenche vetores com informações sobre os vértices 
   static GLfloat vertices[] = {-0.5, -0.5, 0.0, 0.5, -0.5, 0.0, 0.5, 0.5, 0.0, -0.5, 0.5, 0.0};
   static GLfloat texels[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0 ,1.0, 0.0};

   //ativa arrays que serão usados
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);
   
   //associa dados aos arrays
   glTexCoordPointer (2, GL_FLOAT, 0, texels);
   glVertexPointer (3, GL_FLOAT, 0, vertices);
   
   glColor3f (0.7, 0.7, 0.7);
   
   glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
   glPopAttrib();
   glPopMatrix();
    
   glutSwapBuffers();
   glDisable(GL_TEXTURE_2D);
	
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

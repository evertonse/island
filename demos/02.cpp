// Baseado nos exemplos de http://www.glprogramming.com/red/chapter01.html
// Beatriz T. Andrade

// g++ -o primeiro_programa primeiro_programa.cpp -lGL -lGLU -lglut

// Windows: g++ -omain.exe demos/02_quadrado.cpp -DUNICODE -Ivendor/  -lopengl32 -lglu32 -lgdi32

//#include <GL/freeglut.h>
#if defined(_WIN32) || defined(_WIN64)
  #define SWIN_IMPLEMENTATION
  #include "swin.hpp"
#elif
  #include <GL/glut.h>
  #include <GL/glu.h>
  #include <GL/gl.h>
#endif

void display(void)
{
	
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-0.1f, -0.1f, 0.0f);  
    glRotatef(20, 1.0f, 1.0f, 0.0f); 
    //glScalef(1.2f, 0.5f, 1.0f);
    
    
/*  clear all pixels  */
    glClear (GL_COLOR_BUFFER_BIT);

/*  draw polygon (rectangle) with corners at
 *  (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)  
 */
    glBegin(GL_QUADS);
        // cores mapeadas para 0 e 1 a partir do intervalo 
        // de valores de cada tipo (ub = unsigned byte)
        glColor3ub (255, 255, 255);
        glVertex3f (0.25, 0.25, 0.9);
        
        glColor3ub (255, 0, 0);
        glVertex3f (0.75, 0.25, 0.9);
        
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f (0.75, 0.75, 0.9);
        
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f (0.25, 0.75, 0.9);
        
    glEnd();

}

void init (void) {
/*  select clearing (background) color       */
    glClearColor (1.0, 0.0, 0.0, 0.0);

/*  initialize viewing values  */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glRotatef(20, 1.0f, 1.0f, 0.0f); 

    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
  auto wind = swin::Window("hi",600,600);
  wind.init(init);
  wind.display(display); 
  wind.loop();
  return 0;  
}

#define STB_IMAGE_IMPLEMENTATION
#include "std_image.hpp"
#include "resources.hpp"

bool flag = true;
unsigned int ms = 100, angle = 0;
int init_time,cur_pointer = -1; 
double alpha1 = 1, alpha2=0;
Virus v1, v2, ring1[3], ring2[6], ring3[9];
double r1 = 0.1, r2 = 0.2, r3 = 0.35;
Cell cell;

void window(const char* title){
    const int x = 390,y = 80, height = 700, width = 700;
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(x,y);
    glutInitWindowSize(width,height);
    glutCreateWindow(title);   
}

void timer(int value){
glutPostRedisplay();
glutTimerFunc(ms,timer,0);
}

void render(){ 
    int test = 0;
    int time_per_scene[] = {15,20,55};
    int timestamps [4];
    if(test == 0) 
    for(int i =0; i < 4 ; i++,test += time_per_scene[i-1]) timestamps[i] = test;
    else {
        for(int i = 0; i < 4 ; i++ )timestamps[i]= 0;
        timestamps[test] = time_per_scene[test -1];
    }
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity(); 
    int cur = std::time(NULL) - init_time;
    int scene_time = cur - (cur_pointer == -1 ? 0 : timestamps[cur_pointer]);
    std::string msg = "";
    // Scene 1 : Title
    if(cur < timestamps[1]) {
        if(cur_pointer != 0){                                                   
            v1 = Virus(0,0.3,0,1);
            v2 = Virus(0.1,-0.45,0,0.2);
            cur_pointer = 0;
        }
        v1.display();
        print_text("SARS-C",-0.65,-0.5, GLUT_STROKE_MONO_ROMAN,10,3);
        v2.display();
        print_text("V-2",0.2,-0.5, GLUT_STROKE_MONO_ROMAN,10,3);
        print_text("Lifecycle",-0.4,-0.6, GLUT_STROKE_MONO_ROMAN,7,2);
        print_text("- Raghavendra G",0.45,-0.85,GLUT_BITMAP_TIMES_ROMAN_24);
        print_text("1AY17CS031",0.5,-0.95,GLUT_BITMAP_TIMES_ROMAN_24);
        goto nxtloop ;
    }
    
    // Scene 2 : Virus enters human body 
    if(cur < timestamps[2]) {
        if(cur_pointer != 1){                                                     
            v1 = Virus(-1,1,0,1);
            cur_pointer = 1;
        }  
        msg = "Virus enters human body";
        glPushMatrix();
            unsigned int texture = set_texture("background2.png");
            glColor4f(1, 1, 1, 1);
            glBegin(GL_QUADS);
            glVertex2f(-1,-1); glTexCoord2f(0,0);
            glVertex2f(-1,1); glTexCoord2f(1,0);
            glVertex2f(1,1); glTexCoord2f(1,1);
            glVertex2f(1,-1);  glTexCoord2f(0,1);
            glEnd();
            glDeleteTextures(1, &texture);
            if(scene_time < 10) {v1.x += 0.0096; v1.y -=0.0079; v1.scale -=0.0098;}
            if(scene_time >= 10 && scene_time < 11){
                v1.x += 0.002 ;v1.y += 0.01; v1.scale -= 0.001;
            }
            if(scene_time >= 11 && scene_time < 14){v1.y -= 0.022;}
            if(scene_time >= 14 && scene_time < 16){v1.x += 0.01; v1.y -= 0.01;}
            v1.display();
        glPopMatrix();
        goto nxtloop ;   
    }
    
    // Scene 3 : Virus absorbed by cell
    if(cur < timestamps[3]) {
        if(cur_pointer != 2){                                                     
            v1 = Virus(-1,1,0,1);     
            cur_pointer = 2;
        }
        unsigned int texture = set_texture("background3.png");
            glColor4f(1, 1, 1, 1);
            glBegin(GL_QUADS);
            glVertex2f(-1,-1); glTexCoord2f(0,0);
            glVertex2f(-1,1); glTexCoord2f(1,0);
            glVertex2f(1,1); glTexCoord2f(1,1);
            glVertex2f(1,-1);  glTexCoord2f(0,1);
            glEnd();
            glDeleteTextures(1, &texture);
        // a. Virus moves towards the cell :
        if(scene_time < 10){
            msg = "Virus moves towards the cell";
            v1.x += 0.01; v1.y -= 0.006; v1.scale -= 0.0095;
        }
        // b. Virus gets connected to the surface of the cell
        if(scene_time >= 10 && scene_time < 12){ 
            msg = "Virus gets connected to the surface of the cell";
            v1.rotate = false;
        }
        // c. Virus gets sucked in by the cell :
        if(scene_time >= 12 && scene_time < 15) {
            msg = "Virus gets sucked in by the cell";
            cell.compress = true; 
            v1.y -= 0.018; 
            ms = 100;
        } 
        if(scene_time >= 15 && scene_time < 16) { 
            msg = "Virus gets sucked in by the cell";
            v1.rotate = true;
            cell.compress = false; cell.expand = true;
            v1.y -= 0.006;
            ms = 20;  
        } 
        // d. Virus is inside the cell :
        if(scene_time >= 16 && scene_time < 18) {
            msg = "Virus is inside the cell";
            cell.expand = false;
            ms = 100;
            v1.y += 0.015;
        }
        // e. Virus Multiplying inside the cell :
        if(scene_time >= 18 && scene_time < 30){
            msg = "Virus Multiplying inside the cell";
            cell.alpha += 0.0001;
            if(cell.r < 255) cell.r += 3; 
            if(cell.g > 0) cell.g -= 3;
            if(cell.b > 0) cell.b -= 3;
            if(scene_time > 20)
               generate_virus(3, r1, v1.x, v1.y, v1.scale, ring1);
            if(scene_time > 23)
                generate_virus(6, r2, v1.x, v1.y, v1.scale, ring2);
            if(scene_time > 26)
                generate_virus(9, r3, v1.x, v1.y, v1.scale, ring3);
        }
        // f. Cell gets destroyed and the viruses inside it are freed :
        if(scene_time >=30 && scene_time < 32){
            msg = "Cell gets destroyed and the viruses inside it are freed";
            ms = 20;
            cell.scale += 0.2;
            r1 += 0.003; r2 += 0.006; r3 += 0.01;
            generate_virus(3, r1, v1.x, v1.y, v1.scale, ring1);
            generate_virus(6, r2, v1.x, v1.y, v1.scale, ring2);
            generate_virus(9, r3, v1.x, v1.y, v1.scale, ring3);
        }
        // g. Virus contaminating other healthy cells :
        if(scene_time >= 35 && scene_time < 45) { 
            msg = "Virus contaminating other healthy cells";
            ms = 100; alpha2 += 0.005;
        }
        glColor4f(1, 0, 0,alpha2);
        glBegin (GL_QUADS);
        glVertex2f(-1,-1); glVertex2f(-1,1);
        glVertex2f(1,1); glVertex2f(1,-1); 
        glEnd();  
        v1.display();
        if(scene_time > 20) for(int i = 0; i < 3; i++)ring1[i].display();
        if(scene_time > 23) for(int i = 0; i < 6; i++)ring2[i].display();
        if(scene_time > 26) for(int i = 0; i < 9; i++)ring3[i].display();
        cell.display(); 
        goto nxtloop ; 
    }
    
    //Scene 4 : Thank you
    if(cur < timestamps[4]) {
        cur_pointer = 3;
         glPushMatrix();
            unsigned int texture = set_texture("background4.png");
            glColor4f(1, 1, 1, 1);
            glBegin(GL_QUADS);
            glVertex2f(-1,-1); glTexCoord2f(0,0);
            glVertex2f(-1,1); glTexCoord2f(1,0);
            glVertex2f(1,1); glTexCoord2f(1,1);
            glVertex2f(1,-1);  glTexCoord2f(0,1);
            glEnd();
            glDeleteTextures(1, &texture);
            if(angle < 360){ glRotated(angle, 0,1,0); angle += 5;}
            print_text("Thank You",-0.9,0, GLUT_STROKE_MONO_ROMAN,18,3);
        glPopMatrix();
        if(scene_time > 6){
            msg = "Press any key to exit...";
            glutKeyboardFunc([](unsigned char, int, int) {exit(0);});
        }
        goto nxtloop ;    
    }
    nxtloop:
        if(scene_time < 5 && alpha1 > 0) alpha1 -= 0.025;
        if(cur_pointer !=3 && timestamps[cur_pointer + 1] - cur < 5 && alpha1 < 1) 
            alpha1 += 0.025;
        print_text(msg,-0.6,-0.95,GLUT_BITMAP_TIMES_ROMAN_24);
        glColor4f(0, 0, 0,alpha1);
        glBegin (GL_QUADS);
        glVertex2f(-1,-1); glVertex2f(-1,1);
        glVertex2f(1,1); glVertex2f(1,-1); 
        glEnd();
        glutSwapBuffers();   
}

int main(int argc, char** argv){
    glutInit(&argc,argv);
    window("SARS-CoV-2 Lifecycle"); 
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    init_time = std::time(NULL);
    for(int i = 0; i < 3; i++)ring1[i]=Virus();
    for(int i = 0; i < 6; i++)ring2[i]=Virus();
    for(int i = 0; i < 9; i++)ring3[i]=Virus();
    glutDisplayFunc(render);
    glutTimerFunc(0,timer,0);
    glutMainLoop();   
}
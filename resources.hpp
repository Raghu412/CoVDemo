#include<GL/glut.h>
#include<cmath>
#include<bitset>
#include<ctime>
#include<string>

void torus(double OR , double IR ,int rSeg , int cSeg ){
    for (int i = 0; i < rSeg; i++) { 
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j <= cSeg; j++)
            for (int k : {0, 1}) {
                double s = (i + k) % rSeg + 0.5;
                double t = j % (cSeg + 1);
                double x = (IR + OR * cos(s * 2* M_PI / rSeg)) * cos(t * 2* M_PI   / cSeg);
                double y = (IR + OR * cos(s * 2 * M_PI / rSeg)) * sin(t * 2* M_PI  / cSeg);
                double z = OR * sin(s * 2* M_PI / rSeg);
                glNormal3f(2 * x, 2 * y, 2 * z);
                glVertex3d(2 * x, 2 * y, 2 * z);
            }
        glEnd();
    }
}

void sphere(double r, int lats, int longs) {
    int i, j;
    glRotated(90,-1,0,0);
    for(i = 1; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_LINE_STRIP);
        for(j = 1; j <= longs+1; j++) {
            double lng =  2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
}

void arc(double cx,double cy,double start,double end,double RX, double RY) {
	int seg = 100;
    double deg = 2 * M_PI;
    glPushMatrix();
    glBegin(GL_LINE_STRIP);
    for( double angle = start; angle <= deg*end; angle += deg/seg) {
        double x = cx + cos(angle)*RX;
        double y = cy + sin(angle)*RY;
        glVertex2d(x, y);
    }
    glEnd();
    glPopMatrix();
}

void semi_torus(double IR, double OR, int invert){
    int rSeg = 530, cSeg = 50;
    glPushMatrix();
    glRotated(90,invert * -1,0,0);
    for (int i = rSeg/2; i < rSeg; i++) { 
        glBegin(GL_QUAD_STRIP);
        for (int j = 1; j <= cSeg; j++)
            for (int k : {0, 1}) {
                double s = (i + k) % rSeg + 0.5;
                double t = j % (cSeg + 1);
                double x = (IR + OR * cos(s * 2* M_PI / rSeg)) * cos(t * 2* M_PI   / cSeg);
                double y = (IR + OR * cos(s * 2 * M_PI / rSeg)) * sin(t * 2* M_PI  / cSeg);
                double z = OR * sin(s * 2* M_PI / rSeg);
                glNormal3f(2 * x, 2 * y, 2 * z);
                glVertex3d(2 * x, 2 * y, 2 * z);
            }
        glEnd();
    }
    glPopMatrix();
}

unsigned int set_texture(const char* filename){
    int width, height;
    unsigned char *data = stbi_load(filename, &width, &height, NULL, 0); 
    unsigned int texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    return texture;
}

void print_text(std::string text, double x, double y, 
                void* font, int size = -1, int thickness = 1){
    auto drawChar = (size == -1) ? glutBitmapCharacter : glutStrokeCharacter;
    auto position = (size == -1) ? glRasterPos3d : glTranslated;
    for(int i = 0; i < thickness*5; i++){
        glPushMatrix();
        position(x, y, 0);
        if(size != -1) glScalef(size * .0001,size * .0001,size * .0001);
        
        glColor3d(1,1,1);
        for (int character : text) drawChar(font, character);
        x += 0.001; y+=0.001;
        glPopMatrix();     
    } 
}

class Virus { 
    public:
        double x, y ,z, scale = 1.0;
        bool rotate = true, isResetCalled = false;
        unsigned int angle ;
        Virus(double x = 0, double y = 0,double z = 0, double scale = 1):
            x{x},y{y},z{z},scale{scale}{}     
        void display(){ 
            glPushMatrix();
                glColor3d(1,0,0);
                glTranslated(x,y,z);
                glScaled(scale,scale,scale);
                glPushMatrix();
                    if(rotate){ angle+=5; glRotated(angle,1,1,1);}
                    for(int i : {0, 90}){
                        glRotated(i, 0, 1, 0);
                        for(int j : {1, 2, 4}){
                            std::bitset<3> axes = j;
                            for(int k = 0;k < 360; k += 45){
                                glRotated(k, axes[0], axes[1], axes[2]);
                                glutWireSphere(0.25,10,10);
                                glPushMatrix();
                                    glTranslated(0,0,0.25);
                                    glutWireCone(0.02,0.2,10,10);
                                    glTranslated(0,0,0.18);
                                    glutSolidSphere(0.02,10,10);
                                glPopMatrix();
                            } 
                        }  
                    } 
                glPopMatrix();    
            glPopMatrix();
        }
};

class Cell {
    public :
        bool compress = false, expand = false;
        double r =99 ,g=155,b=192;
        double ir = 0, scale = 1, alpha = 0.2;
            void display(){
            if(compress && ir < 0.3) ir += 0.01;
            if(expand && ir > 0) ir -= 0.01;
            glPushMatrix();
            glTranslated(-0.02,-0.1,-0.03);
            glScaled(scale-1.5*ir,scale-1.5*ir,scale-1.5*ir);
            glColor4d(r/255,g/255,b/255,alpha);
            semi_torus(ir,0.25,-1);
            semi_torus(0,ir+0.25,1);
            glPopMatrix(); 
        }
};

void generate_virus(int count, double radius, 
                    double cx, double cy, double scale, Virus* virus){
    glPushMatrix();
        glTranslated(cx,cy,0);
    for(int i = 0; i < count; i++) {
        float theta = 2 * M_PI *i / count;
        double x = (radius * cos(theta));
        double y = (radius * sin(theta));
        virus[i].x = x; virus[i].y = y;
        virus[i].scale = scale;   
    } 
    glPopMatrix();
}

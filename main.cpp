#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#include <vector>
#include <cmath>

using std::vector;

void display();

void reshape(int, int);

void timer(int);

void mouseMovement(int, int);

struct V2 {
    float x;
    float y;
};
struct V3 {
    float x;
    float y;
    float z;
};

bool loadOBJ(const char *path, vector<V3> &out_vertices, vector<V3> &out_normals) {
    vector<unsigned int> vIndices, uvIndices, normalIndices;
    vector<V3> tmp_vertices;
    vector<V2> temp_uvs;
    vector<V3> temp_normals;
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        std::cout << "Impossible to open file!" << std::endl;
        return false;
    }

    while (1) {
        char line[128];
        int res = fscanf(file, "%s", line);
        if (res == EOF)
            break;
        if (strcmp(line, "v") == 0) {
            V3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tmp_vertices.push_back(vertex);
        } else if (strcmp(line, "vn") == 0) {
            V3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        } else if (strcmp(line, "f") == 0) {
            std::string v1, v2, v3;
            unsigned int vIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vIndex[2], &uvIndex[2], &normalIndex[2]
            );

            if (matches != 9) {
                printf("File can't be read by our simple parser");
                return false;
            }
            vIndices.push_back(vIndex[0]);
            vIndices.push_back(vIndex[1]);
            vIndices.push_back(vIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }
    for (unsigned int i = 0; i < vIndices.size(); i++) {
        unsigned int vIndex = vIndices[i];
        V3 v = tmp_vertices[vIndex - 1];
        out_vertices.push_back(v);
    }

    for (unsigned int i = 0; i < normalIndices.size(); i++) {
        unsigned int normalIndex = normalIndices[i];
        V3 normal = temp_normals[normalIndex - 1];
        out_normals.push_back(normal);
    }

}

vector<V3> moja_literka;
vector<V3> moja_literka_normalne;

void init() {
    glClearColor(0.3, 0.2, 0, 1.0);
    glEnable(GL_DEPTH_TEST | GL_LIGHTING);
}

int main(int argc, char **argv) {
    loadOBJ("../literka_a.obj", moja_literka, moja_literka_normalne);

    for (auto x: moja_literka_normalne) {
        std::cout << x.x << " " << x.y << " " << x.z << std::endl;
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowPosition(200, 100);
    glutInitWindowSize(500, 500);

    glutCreateWindow("Trujkontne okno");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(mouseMovement);
    glutTimerFunc(0, timer, 0);
    init();

    glutMainLoop();
    return 0;
}

float myScaleX = 1;
int myScaleState = 1;

int old_mouse_x = 0;
int old_mouse_y = 0;
float mouse_horizontal = 0;
float mouse_vertical = 0;

GLfloat BlueSurface[] = {0.7f, 0.5f, 0.9f, 1.0f};
GLfloat RedSurface[] = {1.0f, 0.3f, 0.3f, 1.0f};

GLfloat LightAmbient[] = {0.1f, 0.1f, 0.1f, 0.1f};
GLfloat LightDiffuse[] = {0.8f, 0.8f, 0.8f, 0.8f};
GLfloat LightPosition[] = {0.0f, 1.0f, 1.0f, 0.0f};

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);        //1 składowa: światło otaczające (bezkierunkowe)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);        //2 składowa: światło rozproszone (kierunkowe)
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    //    ------------------    LEWA LITERKA

    glRotatef(mouse_horizontal, 0, 1, 0);
    glRotatef(mouse_vertical, 1, 0, 0);
    glTranslatef(0, 0, -1);
    glRotatef(myScaleX * 160, 0, 1, 0);
    glScalef(myScaleX, myScaleX, myScaleX);
    glBegin(GL_TRIANGLES);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, BlueSurface);
    for (int i = 0; i < moja_literka.size(); i++) {
        auto vertex = moja_literka[i];
        auto normalna = moja_literka_normalne[i];
        glNormal3d(normalna.x, normalna.y, normalna.z);
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();

    //    ------------------    PRAWA LITERKA
    glLoadIdentity();
    glRotatef(mouse_horizontal, 0, 1, 0);
    glRotatef(mouse_vertical, 1, 0, 0);
    glTranslatef(1, 0, -1);
    glRotatef(myScaleX * 160, 0, 1, 0);
    glScalef(2 - myScaleX, 2 - myScaleX, 2 - myScaleX);

    glBegin(GL_TRIANGLES);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, RedSurface);

    for (int i = 0; i < moja_literka.size(); i++) {
        auto vertex = moja_literka[i];
        auto normalna = moja_literka_normalne[i];
        glNormal3d(normalna.x, normalna.y, normalna.z);
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 0.4, 50);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);

    switch (myScaleState) {
        case 1:
            if (myScaleX < 2) {
                myScaleX += 0.01;
            } else {
                myScaleState = -1;
            }
            break;
        case -1:
            if (myScaleX > 0) {
                myScaleX -= 0.01;
            } else {
                myScaleState = 1;
            }
            break;
    }
}


void mouseMovement(int x, int y) {
    mouse_horizontal += (old_mouse_x - x) * 0.1;
    mouse_vertical += (old_mouse_y - y) * 0.1;
    old_mouse_x = x;
    old_mouse_y = y;
}% 

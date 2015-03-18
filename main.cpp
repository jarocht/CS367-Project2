
#include <iostream>
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

/*#ifdef __APPLE__
#include <OpenGL/glu.h>
#else*/
#include <GL/gl.h>
#include <GL/glu.h>
//#endif


#include <glm/glm.hpp>
#include <sys/time.h>
#include <math.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/io.hpp>
#undef GLFW_DLL

#include "Shapes/Cylinder.h"
#include "Shapes/Sphere.h"
#include "cannon.h"
//#include <GLFW/glfw3.h>
#include <glfw3.h>
#include <lcms.h>

using namespace std;

Cylinder spot;
GLUquadric *wheel = gluNewQuadric();
Sphere sphere;
Cannon can;
float ball_speed = 4.0;
float cannon_speed = 2.0;
float gravity = 0.98;
float x_position = 0.0f;
float z_position = 0.0f;
GLUquadric *q = gluNewQuadric();


glm::mat4 camera_cf, light1_cf, light0_cf;
glm::mat4 cannon_cf, ball_cf, wheel_cf;
glm::mat4 *active;

void win_refresh(GLFWwindow*);
float arc_ball_rad_square;
int screen_ctr_x, screen_ctr_y;
float cannon_angle = M_PI/4;
float launch_angle = M_PI/4;
float cannon_x = 0;
float cannon_y = 0;
float x_init = 0;
float y_init = 0;

bool animateCannon = false;
bool animateBall = false;
bool animate = false;
bool end_trajectory = false;

static float COPPER_AMBIENT[] = {0.191250, 0.073500, 0.022500, 1.000000};
static float COPPER_DIFFUSE[] = {0.703800, 0.270480, 0.082800, 1.000000};
static float COPPER_SPECULAR[] = {0.256777, 0.137622, 0.086014, 1.000000};

static float SILVER_AMBIENT[] = {0.192250, 0.192250, 0.192250, 1.000000};
static float SILVER_DIFFUSE[] = {0.507540, 0.507540, 0.507540, 1.000000};
static float SILVER_SPECULAR[] = {0.508273, 0.508273, 0.508273, 1.000000};

static float BLACKRUBBER_AMBIENT[] = {0.020000, 0.020000, 0.020000, 1.000000};
static float BLACKRUBBER_DIFFUSE[] = {0.010000, 0.010000, 0.010000, 1.000000};
static float BLACKRUBBER_SPECULAR[] = {00.400000, 0.400000, 0.400000, 1.000000};



/* light source setting */
GLfloat light0_color[] = {1.0, 1.0, 1.0, 1.0};   /* color */
GLfloat light1_color[] = {1.0, 1.0, 0.6, 1.0};  /* color */
GLfloat black_color[] = {0.0, 0.0, 0.0, 1.0};   /* color */

void reshapeCallback (GLFWwindow *win, int w, int h)
{
    glViewport (0, 0, w, h);

    /* switch to Projection matrix mode */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    gluPerspective (60, (float) w / (float) h, 5.0, 100.0);

    /* switch back to Model View matrix mode */
    glMatrixMode (GL_MODELVIEW);
    camera_cf = glm::lookAt(glm::vec3(25,20,20), glm::vec3(0,0,10), glm::vec3(0,0,1));
}
/*================================================================*
 * Idle Callback function. This is the main engine for simulation *
 *================================================================*/
void simulate()
{
    static double last_timestamp = 0;
    float delta, current;

    current = glfwGetTime();
    if(animate) {
        delta = current - last_timestamp;
        if (animateCannon) {
            if (cannon_x < 10 && cannon_x >= -10) {
                cannon_cf = glm::translate(glm::vec3{cannon_x, 0, 0});
                cannon_cf *= glm::rotate(cannon_angle, glm::vec3{0, 1, 0});
                cannon_x += cannon_speed * delta;
                if (cannon_x >= 10) {
                    cannon_speed = -2;
                    cannon_x = 9.9;
                }
                else if (cannon_x <= -10) {
                    cannon_speed = 2;
                    cannon_x = -9.9;
                }
            }
        }
        if (animateBall) {
            if (!end_trajectory) {

                z_position += ball_speed * cos(launch_angle) * delta;
                if (ball_speed > 0)
                    x_position += ball_speed * sin(launch_angle) * delta;
                else
                    x_position -= ball_speed * sin(launch_angle) * delta;
                //ball_cf = launch_cf;
                ball_cf = glm::translate(glm::vec3{x_position + x_init, y_init, z_position});
                ball_speed -= delta * gravity;
                if (z_position < 0)
                    end_trajectory = true;

            } else {
                launch_angle = cannon_angle;
                ball_cf = cannon_cf * glm::translate(glm::vec3{0, 0, 3});
                end_trajectory = false;
                x_init = cannon_x;
                y_init = cannon_y;
                z_position = 0;
                x_position = 0;
                ball_speed = 4.0;
            }
        }
    }
    last_timestamp = current;
}

void err_function (int what, const char *msg) {
    cerr << what << " " << msg << endl;
}
void win_resize (GLFWwindow * win, int width, int height)
{
#ifdef DEBUG
    cout << __FUNCTION__ << " " << width << "x" << height << endl;
#endif
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    screen_ctr_x = w / 2.0;
    screen_ctr_y = h / 2.0;
    float rad = min(h,w)/2;
    arc_ball_rad_square = rad * rad;
    /* Use the entire window for our view port */
    glViewport(0, 0, width, height);
    /* Use GL_PROJECTION to select the type of synthetic camera */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* near-plane(1) & far-plane(10) are always POSITIVE and they measure
     * the distances along the Z-axis in front of the camera */
    gluPerspective(60.0, static_cast<float> (width)/ static_cast<float> (height), 1, 100);
}


void win_refresh (GLFWwindow *win) {
//    cout << __PRETTY_FUNCTION__ << endl;
    glClearColor(0, 0.7, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    /* place the camera using the camera coordinate frame */
    glMultMatrixf (glm::value_ptr(camera_cf));

    const float& S = 1;
    /* draw the axes */
    glBegin(GL_LINES);
    glColor3ub (255, 0, 0);
    glVertex2i (0, 0);
    glVertex2f (S * 1.1, 0);
    glColor3ub (0, 255, 0);
    glVertex2i (0, 0);
    glVertex2f (0, S * 1.1);
    glColor3ub (0, 0, 255);
    glVertex2i (0, 0);
    glVertex3f (0, 0, S * 1.1);
    glEnd();

    /* clear the window */
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadMatrixf(glm::value_ptr(camera_cf));

    glBegin (GL_LINES);
    glColor3ub (255, 0, 0);
    glVertex3i (0, 0, 0);
    glVertex3i (2, 0, 0);
    glColor3ub (0, 255, 0);
    glVertex3i (0, 0, 0);
    glVertex3i (0, 2, 0);
    glColor3ub (0, 0, 255);
    glVertex3i (0, 0, 0);
    glVertex3i (0, 0, 2);
    glEnd();


    /* Specify the reflectance property of the ground using glColor
       (instead of glMaterial....)
     */
    glEnable (GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub (29, 100, 56);

    glBegin (GL_QUADS);
    const int GROUND_SIZE = 40;
    glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
    glVertex2i (GROUND_SIZE, GROUND_SIZE);
    glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
    glVertex2i (-GROUND_SIZE, GROUND_SIZE);
    glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
    glVertex2i (-GROUND_SIZE, -GROUND_SIZE);
    glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
    glVertex2i (GROUND_SIZE, -GROUND_SIZE);
    glEnd();
    glDisable (GL_COLOR_MATERIAL);

    glLoadMatrixf(glm::value_ptr(camera_cf));

    /* place the light source in the scene. */
    glLightfv (GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::column(light0_cf, 3)));

    /* recall that the last column of a CF is the origin of the CF */
    glLightfv (GL_LIGHT1, GL_POSITION, glm::value_ptr(glm::column(light1_cf, 3)));

    /* we use the Z-axis of the light CF as the spotlight direction */
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, glm::value_ptr(glm::column(light1_cf, 2)));

    /* the curly bracket pairs below are only for readability */
    glPushMatrix();
    {
        glMultMatrixf(glm::value_ptr(light0_cf));

        /* Render light-0 as an emmisive object */
        if (glIsEnabled(GL_LIGHT0))
            glMaterialfv(GL_FRONT, GL_EMISSION, light0_color);
        sphere.render();
        glMaterialfv(GL_FRONT, GL_EMISSION, black_color);
    }
    glPopMatrix();

    glLoadMatrixf(glm::value_ptr(camera_cf));
    /* render the spot light using its coordinate frame */
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(light1_cf));
    spot.render();
    glPopMatrix();

    /* The following two groups of GL_LINE_LOOP and GL_LINES draw the square block
     * whose 4 vertices make the tetrahedron */
    //enable_contents();

    glLoadMatrixf(glm::value_ptr(camera_cf));

    glMaterialfv(GL_FRONT, GL_AMBIENT, COPPER_AMBIENT);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, COPPER_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, COPPER_SPECULAR);
    glMaterialf(GL_FRONT, GL_SHININESS, 40.0);

    glMultMatrixf(glm::value_ptr(cannon_cf));
    can.render();

    glMaterialfv(GL_FRONT, GL_AMBIENT, SILVER_AMBIENT);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, SILVER_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, SILVER_SPECULAR);
    glMaterialf(GL_FRONT, GL_SHININESS, 90.0);
    //glLoadMatrixf(glm::value_ptr(cannon_cf));
    glTranslatef(0,1.5,0);
    glRotatef(90,0,0,0);
    gluCylinder(wheel,1.0,1.0,3,10,10);


    glLoadMatrixf(glm::value_ptr(camera_cf));
    glMultMatrixf(glm::value_ptr(ball_cf));
    glPushMatrix();
    //glTranslatef(0,0,3);
    glMaterialfv(GL_FRONT, GL_AMBIENT, BLACKRUBBER_AMBIENT);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, BLACKRUBBER_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, BLACKRUBBER_SPECULAR);
    glMaterialf(GL_FRONT, GL_SHININESS, 40.0);
    gluSphere (q, 0.6,10,10);
    glfwSwapBuffers(win);

}



/* action: GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT */
void key_handler (GLFWwindow *win, int key, int scan_code, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    if (mods == GLFW_MOD_SHIFT) {
        switch (key) {
            case GLFW_KEY_D: /* Uppercase 'D' */
                /* pre mult: trans  Z-ax of the world */
//                hex1_cf = glm::translate(glm::vec3{0, +0.5f, 0}) * hex1_cf;
                break;
        }
    }
    else {
        float inc_angle = M_PI/20;
        switch (key) {
//            case GLFW_KEY_SPACE:
//                glPolygonMode(GL_FRONT, GL_LINE);
//                break;
//            case GLFW_KEY_C:
//                camera_cf = camera_cf * glm::translate(glm::vec3{-5, -5, 5});
//                break;
//            case GLFW_KEY_S:
//                camera_cf = board_cf* glm::translate(glm::vec3{0, 0, -2});
//                camera_cf *= glm::rotate(1.57f, glm::vec3{1,0,0});
//                break
            case GLFW_KEY_ESCAPE:
                exit(0);
            case GLFW_KEY_1:
                active = &light0_cf;
                if (glIsEnabled(GL_LIGHT0))
                    glDisable(GL_LIGHT0);
                else
                    glEnable(GL_LIGHT0);
                break;
            case GLFW_KEY_0:
                active = &light1_cf;
                if (glIsEnabled(GL_LIGHT1))
                    glDisable(GL_LIGHT1);
                else
                    glEnable(GL_LIGHT1);
                break;

            case GLFW_KEY_UP:
                cannon_angle += inc_angle;
                cannon_cf *= glm::rotate(inc_angle, glm::vec3{0, 1, 0});
                break;
            case GLFW_KEY_DOWN:
                cannon_angle -= inc_angle;
                cannon_cf *= glm::rotate(inc_angle, glm::vec3{0, -1, 0});
                break;
            case GLFW_KEY_SPACE:
                animate ^= true;
                break;
            case GLFW_KEY_LEFT_ALT:
                animateBall ^= true;
                break;
            case GLFW_KEY_RIGHT_ALT:
                animateCannon ^= true;
                break;
            case GLFW_KEY_A:
                camera_cf *= glm::translate(glm::vec3{-1,0,0});
                break;
            case GLFW_KEY_E:
                camera_cf *= glm::translate(glm::vec3{1,0,0});
                break;
            case GLFW_KEY_Q:
                camera_cf *= glm::translate(glm::vec3{0,1,0});
                break;
            case GLFW_KEY_D:
                camera_cf *= glm::translate(glm::vec3{0,-1,0});
                break;
            case GLFW_KEY_S:
                camera_cf *= glm::translate(glm::vec3{0,0,1});
                break;
            case GLFW_KEY_W:
                camera_cf *= glm::translate(glm::vec3{0,0,-1});
                break;
            case GLFW_KEY_Z:
                camera_cf *= glm::rotate(0.314f, glm::vec3{1,0,0});
                break;
            case GLFW_KEY_X:
                camera_cf *= glm::rotate(0.314f, glm::vec3{0,1,0});
                break;
            case GLFW_KEY_C:
                camera_cf *= glm::rotate(0.314f, glm::vec3{0,0,1});
                break;
            case GLFW_KEY_9:
                camera_cf *= glm::translate(glm::vec3{0, 0, -5});
                light0_cf = glm::translate(glm::vec3{-25, 8, 26});

                light1_cf = glm::translate(glm::vec3{0, -10, 18});
                light1_cf = light1_cf * glm::rotate (glm::radians(-120.0f), glm::vec3{1,0,0});
                break;
            case GLFW_KEY_2:
                light0_cf *= glm::translate(glm::vec3{1,0,0});
                break;
            case GLFW_KEY_3:
                light0_cf  *= glm::translate(glm::vec3{-1,0,0});
                break;
            case GLFW_KEY_4:
                light0_cf  *= glm::translate(glm::vec3{0,1,0});
                break;
            case GLFW_KEY_5:
                light0_cf *= glm::translate(glm::vec3{0,-1,0});
                break;
            case GLFW_KEY_6:
                light0_cf  *= glm::translate(glm::vec3{0,0,1});
                break;
            case GLFW_KEY_7:
                light0_cf  *= glm::translate(glm::vec3{0,0,-1});
                break;
            case GLFW_KEY_COMMA:
                light1_cf *= glm::translate(glm::vec3{1,0,0});
                break;
            case GLFW_KEY_PERIOD:
                light1_cf  *= glm::translate(glm::vec3{0,1,0});
                break;
            case GLFW_KEY_SLASH:
                light1_cf  *= glm::translate(glm::vec3{0,0,1});
                break;
            case GLFW_KEY_P:
                camera_cf = ball_cf;
                break;
            case GLFW_KEY_EQUAL:
                ball_speed += 0.5;
                break;
        }
    }

    win_refresh(win);
}

/*
    The virtual trackball technique implemented here is based on:
    https://www.opengl.org/wiki/Object_Mouse_Trackball
*/
void cursor_handler (GLFWwindow *win, double xpos, double ypos) {
    int state = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT);
    static glm::vec3 first_click;
    static glm::mat4 current_cam;
    static bool is_tracking = false;

    glm::vec3 this_vec;
    if (state == GLFW_PRESS) {
        /* TODO: use glUnproject? */
        float x =  (xpos - screen_ctr_x);
        float y = -(ypos - screen_ctr_y);
        float hypot_square = x * x + y * y;
        float z;

        /* determine whether the mouse is on the sphere or on the hyperbolic sheet */
        if (2 * hypot_square < arc_ball_rad_square)
            z = sqrt(arc_ball_rad_square - hypot_square);
        else
            z = arc_ball_rad_square / 2.0 / sqrt(hypot_square);
        if (!is_tracking) {
            /* store the mouse position when the button was pressed for the first time */
            first_click = glm::normalize(glm::vec3{x, y, z});
            current_cam = camera_cf;
            is_tracking = true;
        }
        else {
            /* compute the rotation w.r.t the initial click */
            this_vec = glm::normalize(glm::vec3{-x, y, z});
            /* determine axis of rotation */
            glm::vec3 N = glm::cross(first_click, this_vec);

            /* determine the angle of rotation */
            float theta = glm::angle(first_click, this_vec);

            /* create a quaternion of the rotation */
            glm::quat q{cos(theta / 2), sin(theta / 2) * N};
            /* apply the rotation w.r.t to the current camera CF */
            camera_cf = current_cam * glm::toMat4(glm::normalize(q));
        }
        win_refresh(win);
    }
    else {
        is_tracking = false;
    }
}

void scroll_handler (GLFWwindow *win, double xscroll, double yscroll) {
    /* translate along the camera Z-axis */
    glm::mat4 z_translate = glm::translate((float)yscroll * glm::vec3{0, 0, 1});
    camera_cf =  z_translate * camera_cf;
    win_refresh(win);
}

void init_gl() {
    glClearColor (0.0, 0.0, 0.0, 1.0); /* black background */

    /* fill front-facing polygon */
    glPolygonMode (GL_FRONT, GL_FILL);
    /* draw outline of back-facing polygon */
    glPolygonMode (GL_BACK, GL_LINE);

    /* Enable depth test for hidden surface removal */
    glEnable (GL_DEPTH_TEST);

    /* enable back-face culling */
    glEnable (GL_CULL_FACE);
    //glCullFace (GL_BACK);

    /* Enable shading */
    glEnable (GL_LIGHTING);
    glEnable (GL_NORMALIZE); /* Tell OpenGL to renormalize normal vector
                              after transformation */
    /* initialize two light sources */
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT0, GL_AMBIENT, light0_color);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light0_color);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light0_color);
    glEnable (GL_LIGHT1);
    glLightfv (GL_LIGHT1, GL_AMBIENT, light1_color);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, light1_color);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light1_color);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 40);

    glEnableClientState(GL_VERTEX_ARRAY);

    /* place the camera at Z=+5 (notice that the sign is OPPOSITE!) */
    camera_cf *= glm::translate(glm::vec3{0, 0, -5});
    light0_cf = glm::translate(glm::vec3{-25, 8, 26});

    light1_cf = glm::translate(glm::vec3{0, -10, 18});
    light1_cf = light1_cf * glm::rotate (glm::radians(-120.0f), glm::vec3{1,0,0});
}

void make_model() {
    cannon_cf = glm::translate(glm::vec3{0,0,0});
    cannon_cf *= glm::rotate(0.8f, glm::vec3{0,1,0});
    ball_cf = cannon_cf*glm::translate(glm::vec3{0,0,3});
    light0_cf = glm::translate(glm::vec3{-25, 8, 26});

    light1_cf = glm::translate(glm::vec3{0, -10, 18});
    light1_cf = light1_cf * glm::rotate (glm::radians(-120.0f), glm::vec3{1,0,0});
    sphere.build(15, 20);
    spot.build(1 + tan(glm::radians(40.0f)), 1, 2);
    can.build();

    gluQuadricOrientation (wheel, GLU_OUTSIDE);

    /* control the generation of normal vectors: one normal per vertex */
    gluQuadricNormals (wheel, GLU_SMOOTH);

    /* control the direction of normal vectors */
    gluQuadricOrientation (q, GLU_OUTSIDE);

    /* control the generation of normal vectors: one normal per vertex */
    gluQuadricNormals (q, GLU_SMOOTH);



}

int main() {

    if(!glfwInit()) {
        cerr << "Can't initialize GLFW" << endl;
        glfwTerminate();
        exit (EXIT_FAILURE);
    }

    GLFWwindow * win;
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    win = glfwCreateWindow(50, 50, "BoarderAnimation", NULL, NULL);

    srand (time(0));

    glfwSetFramebufferSizeCallback(win, reshapeCallback);
    glfwSetWindowRefreshCallback(win, win_refresh);
    /* On Mac with Retina display there is a discrepancy between units measured in
     * "screen coordinates" and "pixels" */
    glfwSetWindowSizeCallback(win, win_resize);  /* use this for non-retina displays */
   // glfwSetFramebufferSizeCallback(win, res); /* use this for retina displays */
    glfwSetKeyCallback(win, key_handler);
    glfwSetCursorPosCallback(win, cursor_handler);
    glfwSetScrollCallback(win, scroll_handler);
    glfwMakeContextCurrent(win);

    /* glewInit must be invoked AFTER glfwMakeContextCurrent() */
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf (stderr, "GLEW init error %s\n", glewGetErrorString(err));
        exit (EXIT_FAILURE);
    }

    /* GL functions can be called, only AFTER the window is created */
    const GLubyte *version = glGetString (GL_VERSION);
    printf ("GL Version is %s\n", version);


    glfwSetWindowSize(win, 800, 800);
    glfwSwapInterval(1);
    init_gl();
    make_model();

    win_refresh(win);

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        simulate();
        win_refresh(win);
    }
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}

#define GLEW_STATIC
#include <GL/glew.h>
#include "CannonBarrel.h"
#define _USE_MATH_DEFINES
#include <cmath>

/**
* This is the code of a truncated cone. Semi-Circle is too complicated so just going to have a cone at that end.
*/
CannonBarrel::~CannonBarrel() {
    glDeleteBuffers(1, &v_buf);
    glDeleteBuffers(1, &i_buf);
}

void CannonBarrel::build() {
    glGenBuffers (1, &v_buf);
    glGenBuffers (1, &i_buf);

    float delta = 2 * M_PI / N_POINTS;

    /* points in top ring */
    float angle = 0.0f;
    for (int k = 0; k  < N_POINTS; k++)
    {
        float x = TOP_RAD * cos(angle);
        float y = TOP_RAD * sin(angle);
        vertices.push_back (x);
        vertices.push_back (y);
        vertices.push_back (CONE_HEIGHT);
        angle += delta;

    }

    /* vertices at the bottom ring */
    for (int k = 0; k  < N_POINTS; k++)
    {
        float x = BOT_RAD * cos(angle);
        float y = BOT_RAD * sin(angle);
        vertices.push_back (x);
        vertices.push_back (y);
        vertices.push_back (-CONE_HEIGHT);
        angle += delta;
    }

    /* push the TOP CENTER point */
    vertices.push_back (0);  /* x */
    vertices.push_back (0); /* y */
    vertices.push_back (CONE_HEIGHT + TOP_RAD); /* z */


    /* fill in the vertices */

    for (int k = 0; k < N_POINTS; k++)
    {
        index.push_back (k);
        index.push_back (k + N_POINTS);
    }
    /* close the quad_strip_index for wall */
    index.push_back (0);
    index.push_back (N_POINTS);

    /* first index of the tri-fan */
    index.push_back (N_POINTS * 2);
    /* index of rest of the tri-fan */
    for (int k = 0; k < N_POINTS; k++)
        index.push_back (k);
    /* close the last arc of the tri-fan */
    index.push_back (0);


    /* copy the two CPU arrays to GPU */

    /* select the buffer */
    glBindBuffer (GL_ARRAY_BUFFER, v_buf);
    /* allocate in GPU and copy from CPU */
    glBufferData (GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
            vertices.data(), GL_STATIC_DRAW);
    /* deselect the buffer */
    glBindBuffer (GL_ARRAY_BUFFER, 0);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, i_buf);
    /* allocate in GPU and copy from CPU */
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLushort),
            index.data(), GL_STATIC_DRAW);
    /* deselect the buffer */
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CannonBarrel::render() const {
    /* select the buffs */
    glPushAttrib(GL_ENABLE_BIT);
    glDisableClientState(GL_COLOR_ARRAY);
    glBindBuffer (GL_ARRAY_BUFFER, v_buf);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, i_buf);

    int N = 2*N_POINTS + 2;
    glDrawElements (GL_QUAD_STRIP, 2*N_POINTS + 2, GL_UNSIGNED_SHORT, 0);
    glDrawElements (GL_TRIANGLE_FAN, N_POINTS + 2, GL_UNSIGNED_SHORT,
            (void *) (sizeof(GLushort) * N));

    /* deselect the buffs */
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    glPopAttrib();
}

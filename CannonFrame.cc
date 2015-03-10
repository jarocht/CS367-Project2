#include "GenericObject2.h"
#include <cmath>

/**
* Adi's code of a generic object. Pretty much can create anything. I haven't played around with it to see how it works.
* He said we would be able to use this to create some sort of frame for the cannon.
*/
GenericObject2::~GenericObject2() {
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &index_buffer);
    glDeleteBuffers(1, &color_buffer);
}

void GenericObject2::build(float pouter_top_radius, float pinner_top_radius,
        float pouter_bottom_radius, float pinner_bottom_radius,
        float pheight, int pnum_sides, float clr1, float clr2, float clr3, float shiny_quotient) {
    outer_top_radius = pouter_top_radius;
    inner_top_radius = pinner_top_radius;
    outer_bottom_radius = pouter_bottom_radius;
    inner_bottom_radius = pinner_bottom_radius;
    height = pheight;
    num_sides = pnum_sides;

    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &index_buffer);
    glGenBuffers(1, &color_buffer);

    float theta = 2 * M_PI/(num_sides);

    float angle = 0.0f;

    for (int k = 0; k < num_sides; k++){
        vertices.push_back(outer_top_radius * cos(angle));
        vertices.push_back(outer_top_radius * sin(angle));
        vertices.push_back(height/2);
        color.push_back (clr1);
        color.push_back (clr2);
        color.push_back (clr3);
        clr1 -= shiny_quotient*0.01;
        clr2 -= shiny_quotient*0.01;
        clr3 -= shiny_quotient*0.01;
        angle += theta;
    }

    angle = 0;
    for (int k = 0; k < num_sides; k++){
        vertices.push_back(inner_top_radius * cos(angle));
        vertices.push_back(inner_top_radius * sin(angle));
        vertices.push_back(height/2);
        color.push_back (clr1);
        color.push_back (clr2);
        color.push_back (clr3);
        clr1 -= shiny_quotient*0.01;
        clr2 -= shiny_quotient*0.01;
        clr3 -= shiny_quotient*0.01;
        angle  += theta;
    }

    angle = 0;
    for (int k = 0; k < num_sides; k++){
        vertices.push_back(outer_bottom_radius * cos(angle));
        vertices.push_back(outer_bottom_radius  * sin(angle));
        vertices.push_back(-height/2);
        color.push_back (clr1);
        color.push_back (clr2);
        color.push_back (clr3);
        clr1 -= shiny_quotient*0.01;
        clr2 -= shiny_quotient*0.01;
        clr3 -= shiny_quotient*0.01;
        angle += theta;
    }

    angle = 0;
    for (int k = 0; k < num_sides; k++){
        vertices.push_back(inner_bottom_radius  * cos(angle));
        vertices.push_back(inner_bottom_radius  * sin(angle));
        vertices.push_back(-height/2);
        color.push_back (clr1);
        color.push_back (clr2);
        color.push_back (clr3);
        clr1 -= shiny_quotient*0.01;
        clr2 -= shiny_quotient*0.01;
        clr3 -= shiny_quotient*0.01;
        angle += theta;
    }

    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(height/2);
    clr1 -= shiny_quotient*0.01;
    clr2 -= shiny_quotient*0.01;
    clr3 -= shiny_quotient*0.01;
    color.push_back (clr1);
    color.push_back (clr2);
    color.push_back (clr3);

    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(-height/2);
    clr1 -= shiny_quotient*0.01;
    clr2 -= shiny_quotient*0.01;
    clr3 -= shiny_quotient*0.01;
    color.push_back (clr1);
    color.push_back (clr2);
    color.push_back (clr3);

    for (int k = num_sides - 1; k > -1; k--){
        indices.push_back(k);
        indices.push_back(k+num_sides);
    }
    indices.push_back(num_sides - 1);
    indices.push_back(2*num_sides - 1);

    for (int k = 0; k < num_sides; k++){
        indices.push_back(k + 2*num_sides);
        indices.push_back(k + 3*num_sides);
    }
    indices.push_back(2*num_sides);
    indices.push_back(3*num_sides);

    for (int k = 0; k < num_sides; k++){
        indices.push_back (k);
        indices.push_back(k + 2*num_sides);
    }
    indices.push_back(0);
    indices.push_back(2*num_sides);

    for (int k = num_sides - 1; k > -1; k--){
        indices.push_back (k + num_sides);
        indices. push_back (k + 3*num_sides);
    }
    indices.push_back(num_sides-1);
    indices.push_back(4*num_sides - 1);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer (GL_ARRAY_BUFFER, color_buffer);
    glBufferData (GL_ARRAY_BUFFER,
            color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort),indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void GenericObject2::render() const {
    glPushAttrib(GL_ENABLE_BIT);
    //glDisableClientState(GL_COLOR_ARRAY);
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer (GL_ARRAY_BUFFER, color_buffer);
    glColorPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glDrawElements(GL_QUAD_STRIP, 2*num_sides + 2, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_QUAD_STRIP, 2*num_sides + 2, GL_UNSIGNED_SHORT,
            (void*)(sizeof(GLushort)*(2*num_sides + 2)));
    glDrawElements(GL_QUAD_STRIP, 2*num_sides + 2, GL_UNSIGNED_SHORT,
            (void*)(sizeof(GLushort)*2*(2*num_sides + 2)));
    glDrawElements(GL_QUAD_STRIP, 2*num_sides + 2, GL_UNSIGNED_SHORT,
            (void*)(sizeof(GLushort)*3*(2*num_sides + 2)));
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    glPopAttrib();
}

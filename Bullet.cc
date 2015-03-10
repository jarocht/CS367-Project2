#include "Bullet.h"
#include <cmath>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>


/**
* Dulimarta's code for a sphere. Like before the semi-circle would be too much of a pain so decided on cannonball
*/
void Bullet::build (int latRings, int lonRings) {
    numLatitudeRings = latRings;
    numLongitudeRings = lonRings;

    /* build a UNIT sphere */

    float dLat = M_PI / latRings;
    float dLon = 2 * M_PI / lonRings;
    float ang = dLat;
    int vnum = 0;
    for (int k = 1; k < latRings; k++) {
        float latRad = sin(ang);
        float z = cos(ang);
        float deg = 0;
        for (int m = 0; m < lonRings; m++) {
            vertices.push_back(latRad * cos(deg));
            vertices.push_back(latRad * sin(deg));
            vertices.push_back(z);
            glm::vec3 norm = glm::normalize(glm::vec3{cos(deg), sin(deg), z});
            normals.push_back(norm.x);
            normals.push_back(norm.y);
            normals.push_back(norm.z);
            vnum++;
            deg += dLon;
        };
        /* index to go back to the first vertex on this ring */
        ang += dLat;
    }
    vertices.push_back(0.0f); /* north pole */
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    normals.push_back(0.0f); /* north pole */
    normals.push_back(0.0f);
    normals.push_back(1.0f);
    vertices.push_back(0.0f); /* south pole */
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);
    normals.push_back(0.0f); /* north pole */
    normals.push_back(0.0f);
    normals.push_back(-1.0f);

    /* generate the indices of the quad strips */
    int ringStart = 0;
    for (int k = 0; k < latRings - 2; k++) {
        for (int m = 0; m < lonRings; m++) {
            indices.push_back(ringStart + m);
            indices.push_back(ringStart + m + lonRings);
        }
        indices.push_back (ringStart);
        indices.push_back (ringStart + lonRings);
        ringStart += lonRings;
    }
    start_of_northpole = indices.size();
    int N_vertices = vertices.size()/3;

    /* indices for triangle fan of north pole */
    indices.push_back(N_vertices - 2); /* tip of the cone */
    for (int k = 0; k < lonRings; k++) {
        indices.push_back (k);
    }
    indices.push_back (0); /* close the triangle fan */

    /* indices for triangle fan of south pole */
    indices.push_back(N_vertices - 1); /* tip of the cone */
    for (int k = 0; k < lonRings; k++) {
        /* vertices must be pushed back in reverse order
           so the south pole cone is CCW
         */
        indices.push_back (N_vertices - 3 - k);
    }
    indices.push_back (N_vertices - 3); /* close the triangle fan */

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
            vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat),
            normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort),
            indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Bullet::render() const {
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glNormalPointer(GL_FLOAT, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    GLushort *ptr = 0;
    /* we use -2 below because the two poles are rendered
     * using triangle fans
     */
    for (int k = 0; k < numLatitudeRings - 2; k++) {
        /* each quad strip must be rendered using a separate call */
        glDrawElements(GL_QUAD_STRIP, 2 * numLongitudeRings + 2,
                GL_UNSIGNED_SHORT, ptr);
        ptr += 2*numLongitudeRings + 2;
    }
    /* render the north pole */
    ptr = (GLushort *) (start_of_northpole * sizeof(GLushort));
    glDrawElements(GL_TRIANGLE_FAN, numLongitudeRings + 2, GL_UNSIGNED_SHORT, ptr);

    /* render the south pole */
    ptr += numLongitudeRings + 2;
    glDrawElements(GL_TRIANGLE_FAN, numLongitudeRings + 2, GL_UNSIGNED_SHORT, ptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    glPopAttrib();
}

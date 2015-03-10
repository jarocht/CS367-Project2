#ifndef BULLET_H
#define BULLET_H
#include <vector>
#include <GL/glew.h>
using namespace std;

class Bullet {
private:
    GLuint vertex_buffer, index_buffer, normal_buffer;
    vector<GLushort> indices;
    vector<GLfloat> vertices, normals;
    int numLongitudeRings, numLatitudeRings;
    int start_of_northpole;

public:
    void build(int latRings, int lonRings);
    void render() const;
};
#endif

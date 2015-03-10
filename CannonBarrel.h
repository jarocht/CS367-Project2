#ifndef CANNON_BARREL_H
#define CANNON_BARREL_H

#include <vector>

using namespace std;

class CannonBarrel {
private:
    const int N_POINTS = 13;
    const float TOP_RAD = 1.0f;
    const float BOT_RAD = 0.8f;
    const float CONE_HEIGHT = 0.5f;
    GLuint v_buf, i_buf;
    vector<float> vertices;
    vector<GLushort> index;

public:
    ~CannonBarrel();
    void build();
    void render() const;
};
#endif

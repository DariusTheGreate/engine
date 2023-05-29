#pragma once
#include <Colider.h>
class Colider;

constexpr double EPA_TOLERANCE = 0.0001;
constexpr size_t EPA_MAX_NUM_FACES = 64;
constexpr size_t EPA_MAX_NUM_LOOSE_EDGES = 32;
constexpr size_t EPA_MAX_NUM_ITERATIONS = 64;

struct Simplex
{
    //for N 
    union
    {
        //std::array<glm::vec3, 4> points;
        glm::vec3 points[4] = {};
        struct {
            glm::vec3 c;
            glm::vec3 b;
            glm::vec3 a;
            glm::vec3 d;
        };
    };

    size_t sz = 0;
public:
    void push_point(glm::vec3 p);

    glm::vec3& last_point();

    glm::vec3& operator [](int i);

    glm::vec3 crossABA(const glm::vec3& a, const glm::vec3& b);

    bool update(glm::vec3& dir);

    void update_simplex3(glm::vec3& dir);

    /*
    The tetrahedron case is the most complex, but almost entirely made up of triangle cases. We don’t need to test for the origin below the tetrahedron for the same reason as before.
    We only need to determine which face, if any, the origin is in the direction of. If there is one, we’ll go back to the triangle case with that face as the simplex, but if not,
    we know it must be inside the tetrahedron and we’ll return true.
    */
    bool update_simplex4(glm::vec3& dir);

    glm::vec3 EPA(Colider* coll1, Colider* coll2);
};

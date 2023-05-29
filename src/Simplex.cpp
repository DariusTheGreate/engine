#include "Simplex.h"

void Simplex::push_point(glm::vec3 p)
{
    points[sz++] = p;
}

glm::vec3& Simplex::last_point()
{
    assert(sz > 0);
    return points[sz - 1];
}

glm::vec3& Simplex::operator [](int i)
{
    return points[i];
}

glm::vec3 Simplex::crossABA(const glm::vec3& a, const glm::vec3& b)
{
    return glm::cross(glm::cross(a, b), a);
}

bool Simplex::update(glm::vec3& dir)
{
    if (sz == 3) {
        update_simplex3(dir);
    }
    else if (update_simplex4(dir)) {
        return true;
    }

    return false;
}

void Simplex::update_simplex3(glm::vec3& dir) {
    glm::vec3 n = glm::cross(b - a, c - a);
    glm::vec3 AO = -a;

    sz = 2;
    if (glm::dot(glm::cross(b - a, n), AO) > 0) {
        c = a;
        dir = crossABA(b - a, AO);
        return;
    }
    if (glm::dot(glm::cross(n, c - a), AO) > 0) {
        b = a;
        dir = crossABA(c - a, AO);
        return;
    }

    sz = 3;
    if (glm::dot(n, AO) > 0) {
        d = c;
        c = b;
        b = a;
        dir = n;
        return;
    }

    d = b;
    b = a;

    dir = -n;
    return;
}

/*
The tetrahedron case is the most complex, but almost entirely made up of triangle cases. We don’t need to test for the origin below the tetrahedron for the same reason as before.
We only need to determine which face, if any, the origin is in the direction of. If there is one, we’ll go back to the triangle case with that face as the simplex, but if not,
we know it must be inside the tetrahedron and we’ll return true.
*/
bool Simplex::update_simplex4(glm::vec3& dir) {
    glm::vec3 ABC = glm::cross(b - a, c - a);
    glm::vec3 ACD = glm::cross(c - a, d - a);
    glm::vec3 ADB = glm::cross(d - a, b - a);

    glm::vec3 AO = -a;
    sz = 3;

    if (glm::dot(ABC, AO) > 0) {
        d = c;
        c = b;
        b = a;
        dir = ABC;
        return false;
    }
    if (glm::dot(ACD, AO) > 0) {
        b = a;
        dir = ACD;
        return false;
    }
    if (glm::dot(ADB, AO) > 0) {
        c = d;
        d = b;
        b = a;
        dir = ADB;
        return false;
    }

    return true;
}

glm::vec3 Simplex::EPA(Colider* coll1, Colider* coll2) {
    glm::vec3 faces[EPA_MAX_NUM_FACES][4];

    faces[0][0] = a;
    faces[0][1] = b;
    faces[0][2] = c;
    faces[0][3] = glm::normalize(glm::cross(b - a, c - a));
    faces[1][0] = a;
    faces[1][1] = c;
    faces[1][2] = d;
    faces[1][3] = glm::normalize(cross(c - a, d - a)); 
    faces[2][0] = a;
    faces[2][1] = d;
    faces[2][2] = b;
    faces[2][3] = glm::normalize(cross(d - a, b - a));
    faces[3][0] = b;
    faces[3][1] = d;
    faces[3][2] = c;
    faces[3][3] = glm::normalize(cross(d - b, c - b));

    int num_faces = 4;
    int closest_face;

    for (int iterations = 0; iterations < EPA_MAX_NUM_ITERATIONS; iterations++) {
        float min_dist = glm::dot(faces[0][0], faces[0][3]);
        closest_face = 0;
        for (int i = 1; i < num_faces; i++) {
            float dist = glm::dot(faces[i][0], faces[i][3]);
            if (dist < min_dist) {
                min_dist = dist;
                closest_face = i;
            }
        }

        glm::vec3 search_dir = faces[closest_face][3];
        glm::vec3 p = coll2->support(glm::vec4{ search_dir.x, search_dir.y, search_dir.z, 0 }) - coll1->support(-glm::vec4{ search_dir.x, search_dir.y, search_dir.z, 0 });

        if (glm::dot(p, search_dir) - min_dist < EPA_TOLERANCE) {
            return faces[closest_face][3] * dot(p, search_dir);
        }

        glm::vec3 loose_edges[EPA_MAX_NUM_LOOSE_EDGES][2];
        int num_loose_edges = 0;

        for (int i = 0; i < num_faces; i++)
        {
            if (glm::dot(faces[i][3], p - faces[i][0]) > 0)
            {
                for (int j = 0; j < 3; j++) 
                {
                    glm::vec3 current_edge[2] = { faces[i][j], faces[i][(j + 1) % 3] };
                    bool found_edge = false;
                    for (int k = 0; k < num_loose_edges; k++) 
                    {
                        if (loose_edges[k][1] == current_edge[0] && loose_edges[k][0] == current_edge[1]) {
                            loose_edges[k][0] = loose_edges[num_loose_edges - 1][0];
                            loose_edges[k][1] = loose_edges[num_loose_edges - 1][1];
                            num_loose_edges--;
                            found_edge = true;
                            k = num_loose_edges; 
                        }
                    }

                    if (!found_edge) { 
                        if (num_loose_edges >= EPA_MAX_NUM_LOOSE_EDGES) break;
                        loose_edges[num_loose_edges][0] = current_edge[0];
                        loose_edges[num_loose_edges][1] = current_edge[1];
                        num_loose_edges++;
                    }
                }

                faces[i][0] = faces[num_faces - 1][0];
                faces[i][1] = faces[num_faces - 1][1];
                faces[i][2] = faces[num_faces - 1][2];
                faces[i][3] = faces[num_faces - 1][3];
                num_faces--;
                i--;
            }
        }

        for (int i = 0; i < num_loose_edges; i++)
        {
            if (num_faces >= EPA_MAX_NUM_FACES) break;
            faces[num_faces][0] = loose_edges[i][0];
            faces[num_faces][1] = loose_edges[i][1];
            faces[num_faces][2] = p;
            faces[num_faces][3] = glm::normalize(cross(loose_edges[i][0] - loose_edges[i][1], loose_edges[i][0] - p));

            float bias = 0.000001;
            if (glm::dot(faces[num_faces][0], faces[num_faces][3]) + bias < 0) {
                glm::vec3 temp = faces[num_faces][0];
                faces[num_faces][0] = faces[num_faces][1];
                faces[num_faces][1] = temp;
                faces[num_faces][3] = -faces[num_faces][3];
            }
            num_faces++;
        }
    } 
    return faces[closest_face][3] * dot(faces[closest_face][0], faces[closest_face][3]);
}

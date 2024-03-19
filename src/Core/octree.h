#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <Engine/AABB.h>

template <typename T>
class Node {
private:
    std::vector<T*> objects;
    std::array<Node*, 7> nodes;
    MeshAABB region;

    Node(std::vector<T*>& o, std::array(Node*)& n, MeshAABB& r) {
        objects = o;
        nodes = n;
        region = r;
    }
    Node(std::vector<T*>& o, MeshAABB& r) {
        objects = o;
        region = r;
    }

    Node(glm::vec3 min, glm::vec3 max) {
        region = MeshAABB(min, max);
    }

public:
    bool isPointsInside(const T& object, const MeshAABB& r) {
        for (size_t i = 0; i < T.getCorners().size(); i++) {
            if (!r.isInside(T.getCorners()[i])) {
                return false;
            }
        }
    }

    bool node_search(T& o) {
        for (size_t i = 0; i < nodes.size(); i++) {
            if (isPointsInside(o, nodes[i].region)) {
                nodes[i].push_back(o);
                return true;
            }
            return false;
        }
    }
    void divide() {
        if (objects.size() > 10) {
            nodes[0] = Node(region.min, region.center);
            nodes[1] = Node(region.center, region.max);
            nodes[2] = Node(nodes[0].getCorners()[1], nodes[1].getCorners()[1]);
            nodes[3] = Node(nodes[0].getCorners()[2], nodes[1].getCorners()[2]);
            nodes[4] = Node(nodes[0].getCorners()[3], nodes[1].getCorners()[3]);
            nodes[5] = Node(nodes[0].getCorners()[4], nodes[1].getCorners()[4]);
            nodes[6] = Node(nodes[0].getCorners()[5], nodes[1].getCorners()[5]);
            nodes[7] = Node(nodes[0].getCorners()[6], nodes[1].getCorners()[6]);
            for (size_t i = 0; i < nodes.size(); i++) {
                for (size_t j = 0; j < objects.size(); j++) {
                    if (isPointsInside(objects[j], nodes[i].region)) {
                        nodes[i].push_back(objects[j]);
                        objects[j] = nullptr;
                    }
                }
            }
        }

    }
};
template <typename T>
class Octree {
private:
    Node* root;
public:
    Octree() {
        root = new(Node);
    }
    bool update_object(T& o, Node& r, bool flag) {
        if (flag) {
            return;
        }
        while (!r.node_search(o)) {
            if (r.nodes[0] == nullptr) {
                return;
            }
            for (size_t i = 0; i < r.nodes.size(); i++) {
                if (update_object(o, r.nodes[i]), flag) {
                    flag = 1;
                    return;
                }
            }
            return;
        }
    }

    void update(Node& n) {
        for (size_t i = 0; i < n.nodes.size(); i++) {
            for (size_t j = 0; j < n.objects.size(); j++) {
                if (n.objects[j].isChanged == 1) {
                    update_object(n.objects[j], root, 0);
                }
            }
            if (n.nodes(i) != nullptr) {
                update(n.nodes(i));
            }
        }
    }
};
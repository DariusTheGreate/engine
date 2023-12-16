#include <Core/Printer.h>


std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::mat2& m) {
    size_t size = 2;
    os << "(";
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            os << m[c][r];
            if (c < (size - 1)) os << ", ";
        }
        if (r < (size - 1)) os << std::endl;
    }
    return os << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::mat3& m) {
    size_t size = 3;
    os << "(";
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            os << m[c][r];
            if (c < (size - 1)) os << ", ";
        }
        if (r < (size - 1)) os << std::endl;
    }
    return os << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
    size_t size = 4;
    os << "(";
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            os << m[c][r];
            if (c < (size - 1)) os << ", ";
        }
        if (r < (size - 1)) os << std::endl;
    }
    return os << ")";
}

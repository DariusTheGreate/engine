#pragma once

#include <utility>
#include <cmath>
#include "glm/glm.hpp"
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>

std::ostream& operator<<(std::ostream& os, const glm::vec2& v);

std::ostream& operator<<(std::ostream& os, const glm::vec3& v);

std::ostream& operator<<(std::ostream& os, const glm::vec4& v);

std::ostream& operator<<(std::ostream& os, const glm::mat2& m);

std::ostream& operator<<(std::ostream& os, const glm::mat3& m);

std::ostream& operator<<(std::ostream& os, const glm::mat4& m);


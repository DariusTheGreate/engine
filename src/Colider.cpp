#include "Colider.h"

Colider::Colider(glm::vec3 size_in, Transform& tr_in, int tag_in, bool active_in) : size(size_in), tr(tr_in), tag(tag_in), active(active_in)
{
}

glm::vec3 Colider::get_position() const 
{
	return tr.position;
}

glm::vec3 Colider::get_size() const 
{
	return size;
}

glm::vec3& Colider::get_size_ref()
{
	return size;
}

std::vector<glm::vec3> Colider::get_points() {
	std::vector<glm::vec3> points;
	glm::mat4 transformation = tr.get_quatmat();
	glm::vec4 position = { tr.position.x, tr.position.y, tr.position.z, 0 };

	points[0] = transformation * glm::vec4(size.x, size.y, size.z, 0) + position;
	points[1] = transformation * glm::vec4(size.x, size.y, -size.z, 0) + position;
	points[2] = transformation * glm::vec4(size.x, -size.y, size.z, 0) + position;
	points[3] = transformation * glm::vec4(size.x, -size.y, -size.z, 0) + position;
	points[4] = transformation * glm::vec4(-size.x, size.y, size.z, 0) + position;
	points[5] = transformation * glm::vec4(-size.x, size.y, -size.z, 0) + position;
	points[6] = transformation * glm::vec4(-size.x, -size.y, size.z, 0) + position;
	points[7] = transformation * glm::vec4(-size.x, -size.y, -size.z, 0) + position;

	return points;
}

bool Colider::contains_point(const glm::vec3& point) {
	glm::mat4 transformation = glm::inverse(tr.get_quatmat());
	glm::vec3 v3 = (point - tr.position);
	glm::vec3 box_point = transformation * glm::vec4{ v3.x, v3.y, v3.z, 0 };
	glm::vec3 new_half_lengths = size + glm::vec3(0.01, 0.01, 0.01);

	if (box_point.x < -new_half_lengths.x || box_point.x > new_half_lengths.x) {
		return false;
	}

	if (box_point.y < -new_half_lengths.y || box_point.y > new_half_lengths.y) {
		return false;
	}

	if (box_point.z < -new_half_lengths.z || box_point.z > new_half_lengths.z) {
		return false;
	}

	return true;
}

glm::vec3 Colider::supportOriented(glm::vec4 dir) {
	dir = glm::inverse(tr.get_quatmat()) * dir; 

	glm::vec4 result;

	//glm::quat quatinverse = {tr.q.w, -tr.q.x, -tr.q.y, -tr.q.z}; 
	//dir = quatinverse * dir;

	result.x = (dir.x > 0) ? size.x : 0;
	result.y = (dir.y > 0) ? size.y : 0;
	result.z = (dir.z > 0) ? size.z : 0;
	result.w = 0;
	
	/*result.x = (dir.x < 0) ? -size.x / 2 : size.x / 2;
	result.y = (dir.y > 0) ? -size.y/2 : size.y/2;
	result.z = (dir.z > 0) ? -size.y/2 : size.y/2;
	*/
	//dont forget to add radius

	//return result;
	return tr.get_quatmat() * result + glm::vec4{ tr.position.x, tr.position.y, tr.position.z, 0 };
}

glm::vec3 Colider::support(glm::vec4 dir) {
	glm::vec4 result;
	result.x = (dir.x > 0) ? maxX() : minX();
	result.y = (dir.y > 0) ? maxY() : minY();
	result.z = (dir.z > 0) ? maxZ() : minZ();
	result.w = 0;
	
	return result;
}

glm::vec3 dotSupport(glm::vec3 dir, std::vector<glm::vec3> vertices)
{
	//auto vertices = get_points();
	auto largestDot = glm::dot(vertices[0], dir);
	glm::vec3 largestVert = vertices[0];

	for(auto& i : vertices){
		auto currDot = glm::dot(i, dir);
		if(currDot > largestDot)
			largestVert = i;
	}

	return largestVert;
}

// TODO(darius) very sensitive to float precision => ergo quaternion values. Most popular approach, but i hate it
//https://www.youtube.com/watch?v=ajv46BSqcK4&ab_channel=Reducible
bool Colider::gjk(Colider* coll1, Colider* coll2) {
	Simplex plex;
	glm::vec3 dir =  coll2->colider_position() - coll1->colider_position();

	plex.c = (coll1->support({ dir.x, dir.y, dir.z, 0 }) - coll2->support(glm::vec4{ -dir.x, -dir.y, -dir.z, 0 }));
	if (glm::dot(plex.c, dir) < 0) { return false; }
	dir = -plex.c; 
	plex.sz++;

	plex.b = (coll1->support(glm::vec4{ dir.x, dir.y, dir.z, 0 }) - coll2->support(glm::vec4{ -dir.x, -dir.y, -dir.z, 0 }));
	if (glm::dot(plex.b, dir) < 0) { return false; }
	dir = plex.crossABA(plex.c - plex.b, -plex.b);

	plex.sz++;

	for (int iterations = 0; iterations < 1000; iterations++)
	{
		if (dir.length() < 0.0001f)
			return false;

		plex.a = (coll1->support(glm::vec4{ dir.x, dir.y, dir.z, 0 }) - coll2->support(-glm::vec4{ dir.x, dir.y, dir.z, 0 }));

		if (glm::dot(plex.a, dir) < 0) 
			return false; 

		plex.sz++;

		if (plex.update(dir)) {
			epa_collision_value = plex.EPA(coll1, coll2);
			return true;
		}
	}

	return false;
}

bool Colider::check_collision(const Colider& c) const
{
	auto cpos = c.get_pos();
	auto csize = c.get_size();

	//cringe AND Float comparasion UB
	return (minX() <= c.maxX()) && (c.minX() <= maxX()) && (minY() <= c.maxY()) && (c.minY() <= maxY()) && (minZ() <= c.maxZ()) && (c.minZ() <= maxZ());
}

glm::vec3 Colider::get_pos() const 
{
	return tr.position;
}

glm::vec3& Colider::get_pos_ref() 
{
	return tr.position;
}

int Colider::get_tag() const
{
	return tag;
}

float Colider::minX() const
{
	glm::vec3 position = colider_position();
	return glm::min(position.x, position.x + size.x);
}

float Colider::minY() const 
{
	glm::vec3 position = colider_position();
	return glm::min(position.y, position.y + size.y);
}

float Colider::minZ() const
{
	glm::vec3 position = colider_position();
	return glm::min(position.z, position.z + size.z);
}

float Colider::maxX() const
{
	glm::vec3 position = colider_position();
	return glm::max(position.x, position.x + size.x);
}

float Colider::maxY() const
{
	glm::vec3 position = colider_position();
	return glm::max(position.y, position.y + size.y);
}

float Colider::maxZ() const
{
	glm::vec3 position = colider_position();
	return glm::max(position.z, position.z + size.z);
}

glm::vec3 Colider::get_render_start_point() const
{
	glm::vec3 position = colider_position();
	return glm::vec3{position.x, position.y, position.z};
}

glm::vec3& Colider::get_render_shift() 
{
	return shift;
}

glm::vec3 Colider::colider_position() const
{
	return tr.position - shift;
}

glm::vec3 Colider::get_epa()
{
	auto res = epa_collision_value;
	epa_collision_value = { 0,0,0 };
	return res;
}

bool Colider::is_active() const {
	return active;
}

Transform& Colider::get_transform()
{
	return tr;
}

#include "Colider.h"

Colider::Colider(const glm::vec3& size_in, Transform& tr_in, int tag_in, bool active_in) : size(size_in), tr(tr_in), tag(tag_in), active(active_in)
{
}

glm::vec3 Colider::get_position() const 
{
	return tr.getPosition();
}

glm::vec3 Colider::get_size() const 
{
	return size;
}

void Colider::set_size(const glm::vec3& v)
{
	size = v;
}

glm::vec3 Colider::get_shift() const
{
	return shift;
}

void Colider::set_shift(const glm::vec3& v)
{
	shift = v;
}

glm::vec3& Colider::get_size_ref()
{
	return size;
}

std::vector<glm::vec3> Colider::get_points() {
	std::vector<glm::vec3> points;
	points.resize(8);
	glm::mat4 transformation = tr.matrix;
	glm::vec4 position = { tr.getPosition().x, tr.getPosition().y, tr.getPosition().z, 0};

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
	glm::mat4 transformation = glm::inverse(tr.matrix);
	glm::vec3 v3 = (point - tr.getPosition());
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

glm::vec3 Colider::supportOriented(const glm::vec4& dir) {
	glm::vec4 dirTr = glm::inverse(tr.matrix) * dir; 

	glm::vec4 result;

	//glm::quat quatinverse = {tr.q.w, -tr.q.x, -tr.q.y, -tr.q.z}; 
	//dir = quatinverse * dir;

	dirTr = dir * tr.matrix;

	glm::vec4 sz = tr.matrix * glm::vec4(maxX(), maxY(), maxZ(), 0);

	result.x = (dirTr.x > 0) ? sz.x : 0;
	result.y = (dirTr.y > 0) ? sz.y : 0;
	result.z = (dirTr.z > 0) ? sz.z : 0;
	result.w = 0;
	
	/*result.x = (dir.x < 0) ? -size.x / 2 : size.x / 2;
	result.y = (dir.y > 0) ? -size.y/2 : size.y/2;
	result.z = (dir.z > 0) ? -size.y/2 : size.y/2;
	*/
	//dont forget to add radius

	return result;
	//return tr.matrix * result + glm::vec4{ tr.getPosition().x, tr.getPosition().y, tr.getPosition().z, 0};
}

glm::vec3 Colider::support(const glm::vec4& dir) {
	glm::vec4 result;
	result.x = (dir.x > 0) ? maxX() : minX();
	result.y = (dir.y > 0) ? maxY() : minY();
	result.z = (dir.z > 0) ? maxZ() : minZ();
	result.w = 0;
	
	return result;
}

glm::vec3 dotSupport(const glm::vec3& dir, std::vector<glm::vec3> vertices)
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
		if (dir.length() < 0.0001f) {
			collision_state = false;
			return false;
		}

		plex.a = (coll1->support(glm::vec4{ dir.x, dir.y, dir.z, 0 }) - coll2->supportOriented(-glm::vec4{ dir.x, dir.y, dir.z, 0 }));

		if (glm::dot(plex.a, dir) < 0) {
			collision_state = false;
			return false;
		}

		plex.sz++;

		if (plex.update(dir)) {
			epa_collision_value = plex.EPA(coll1, coll2);
			collision_state = true;
			return true;
		}
	}

	collision_state = false;
	return false;
}

//NOTE(it only pushes *this colider)
//NOTE(u dont check collision state for rigid body update)
//NOTE(it has weird logic)
//TODO(darius) fixit or switch to physX
glm::vec3 Colider::check_collision(const Colider& c) const
{
	//cringe AND Float comparasion UB
	glm::vec3 res = {0,0,0};

	float lesserDiff = FLT_MAX;
	float tmp = 0;


	if ((minX() <= c.maxX()) && (c.minX() <= maxX()) && (minY() <= c.maxY()) && (c.minY() <= maxY()) && (minZ() <= c.maxZ()) && (c.minZ() <= maxZ()))
	{
		/*std::cout << minX() << " " << maxX() << "\n";
		std::cout << c.minX() << " " << c.maxX() << "\n";
		std::cout << minY() << " " << maxY() << "\n";
		std::cout << c.minY() << " " << c.maxY() << "\n";
		*/

		if (maxX() >= c.minX())
		{
			tmp = -(maxX() - c.minX()) - collisionEpsilon;
			//std::cout << "tmpx" << tmp << "\n";                          // // // ÍÅ ÇÀÁÓÄÜ ÐÀÑÊÎÌÅÍÒÈÒÜ
			if (std::abs(lesserDiff) > std::abs(tmp)) 
			{
				lesserDiff = std::abs(tmp);
				res = {0,0,0};
				res.x = -lesserDiff;
			}
		}

		if (minX() <= c.maxX())
		{
			tmp = (c.maxX() - minX()) + collisionEpsilon;
			//std::cout << "tmpx2" << tmp << "\n";
			if (std::abs(lesserDiff) > std::abs(tmp))
			{
				lesserDiff = std::abs(tmp);
				res = { 0,0,0 };
				res.x = lesserDiff;
			}
		}
	
		//bottom - up
		if (minY() <= c.maxY()) 
		{
			tmp = (c.maxY() - minY()) + collisionEpsilon;
			//std::cout << "tmpx2" << tmp << "\n";
			if (std::abs(lesserDiff) > std::abs(tmp))
			{
				lesserDiff = std::abs(tmp);
				res = { 0,0,0 };
				res.y = lesserDiff;
			}
		}

		//up - botton
		if (c.minY() <= maxY()) 
		{
			tmp = -(maxY() - c.minY()) - collisionEpsilon;
			//std::cout << "tmpx" << tmp << "\n";                          // // // ÍÅ ÇÀÁÓÄÜ ÐÀÑÊÎÌÅÍÒÈÒÜ
			if (std::abs(lesserDiff) > std::abs(tmp))
			{
				lesserDiff = std::abs(tmp);
				res = { 0,0,0 };
				res.y = -lesserDiff;
			}
		}
		if (minZ() <= c.maxZ())
		{
			tmp = (c.maxZ() - minZ()) + collisionEpsilon;
			//std::cout << "tmpx2" << tmp << "\n";
			if (std::abs(lesserDiff) > std::abs(tmp))
			{
				lesserDiff = std::abs(tmp);
				res = { 0,0,0 };
				res.z = lesserDiff;
			}
		}
		if (c.minZ() <= maxZ())
		{
			tmp = -(maxZ() - c.minZ()) - collisionEpsilon;
			//std::cout << "tmpx" << tmp << "\n";                          // // // ÍÅ ÇÀÁÓÄÜ ÐÀÑÊÎÌÅÍÒÈÒÜ
			if (std::abs(lesserDiff) > std::abs(tmp))
			{
				lesserDiff = std::abs(tmp);
				res = { 0,0,0 };
				res.z = -lesserDiff;
			}
		}
	}

	return res;
}

glm::vec3 Colider::get_pos() const 
{
	return tr.getPosition();
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
	return tr.getPosition() + shift;
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

bool* Colider::get_collision_state()
{
	return &collision_state;
}

Transform& Colider::get_transform()
{
	return tr;
}

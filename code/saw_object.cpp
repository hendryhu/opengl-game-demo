#include "saw_object.h"

namespace game {

	SawObject::SawObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
		: GameObject(position, geom, shader, texture) {

	}

	void SawObject::Update(double delta_time) {
		float new_angle = GetRotation() + turn_speed * (float)delta_time;
		new_angle = Helper::normalizeAngleToPositive(new_angle);
		SetRotation(new_angle);
	}

}
#include "enemy_arm_object.h"

namespace game {

	EnemyArmObject::EnemyArmObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, bool swing)
		: GameObject(position, geom, shader, texture) {
		this->swing = swing;

		SetScale(1);
		model_offset_x = GetScale() / 2;
		child_attach_offset_x = GetScale() / 2;
	}

	void EnemyArmObject::Update(double delta_time) {

		if (swing) {
			if (swinging_cw) {	// clockwise
				swing_angle -= turn_speed * (double)delta_time;
				if (swing_angle < -max_swing_angle) {
					swing_angle = -max_swing_angle;
					swinging_cw = false;
				}
			}
			else {	// counter clockwise
				swing_angle += turn_speed * (double)delta_time;
				if (swing_angle > max_swing_angle) {
					swing_angle = max_swing_angle;
					swinging_cw = true;
				}
			}
			SetRotation(swing_angle + parent_angle);
		}
		else {
			turnTowardsTarget(delta_time);
		}
		
		// Update all child objects
		for (auto child : child_objects_) {
			EnemyArmObject* current = dynamic_cast<EnemyArmObject*>(child);
			current->SetPosition(GetPosition() + GetBearing() * child_attach_offset_x + current->getModelPosition());
			current->setTargetPosition(target_position);
			current->setModelRotation(GetRotation());
			child->Update(delta_time);
		}

		if (saw_object != nullptr) {
			saw_object->SetPosition(GetPosition() + GetBearing() * child_attach_offset_x);
			saw_object->Update(delta_time);
		}
	}

	glm::vec3 EnemyArmObject::getModelPosition() {
		return GetBearing() * model_offset_x;
	}

	void EnemyArmObject::turnTowardsTarget(double delta_time) {
		glm::vec3 self_direction = GetBearing();
		glm::vec3 target_vector = target_position - GetPosition();

		float angle_difference = Helper::angleBetweenVectors(self_direction, target_vector);
		float angle_displacement = turn_speed * static_cast<float>(delta_time);

		if (angle_difference < 0.0f) {		// cw
			float new_angle = GetRotation() + std::max(angle_difference, -angle_displacement);
			new_angle = Helper::normalizeAngleToPositive(new_angle);
			SetRotation(new_angle);
		}
		else if (angle_difference > 0.0f) {	// ccw
			float new_angle = GetRotation() + std::min(angle_difference, angle_displacement);
			new_angle = Helper::normalizeAngleToPositive(new_angle);
			SetRotation(new_angle);
		}
	}
}
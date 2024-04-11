#include <iostream>

#include "enemy_game_object.h"

namespace game {

	EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player)
		: GameObject(position, geom, shader, texture) {
		object_type = ObjectType::Enemy;
		player_ = player;
		SetScale(size);
		target_position = GetPosition();
		setHitboxRadius(this->GetScale() / 2);
	}

	void EnemyGameObject::Update(double delta_time) {
		glm::vec3 player_position = player_->GetPosition();
		float player_distance = glm::length(player_->GetPosition() - GetPosition());

		switch (state) {
			
		// Idle
		case EnemyState::IDLE:
			if (player_distance <= idle_detection_radius) {	// detects player
				state = EnemyState::ENGAGE;
				target_position = player_->GetPosition();
				break;
			}

			if (Helper::roll(wander_chance)) {
				glm::vec3 target_displacement = Helper::generate2dPosition(-wander_range, wander_range, -wander_range, wander_range);
				target_position = position_ + target_displacement;
				state = EnemyState::WANDER;
				break;
			}
			break;

		// Wander
		case EnemyState::WANDER:
			if (player_distance <= idle_detection_radius) {	// detects player
				state = EnemyState::ENGAGE;
				target_position = player_->GetPosition();
				break;
			}
			if (glm::length(target_position - GetPosition()) <= 0.05f) {		// arrived at target
				state = EnemyState::IDLE;
				break;
			}
			moveTowardsTarget(delta_time, false);
			
			break;

		case EnemyState::ALERT:
			alert_timer.Update(delta_time);
			if (alert_timer.timesUp()) {
				state = EnemyState::IDLE;
				break;
			}
			if (player_distance <= alert_detection_radius) {	// detects player
				state = EnemyState::ENGAGE;
				target_position = player_->GetPosition();
				break;
			}
			turnTowardsTarget(delta_time);
			break;

		// Engage
		case EnemyState::ENGAGE:
			engageAction(delta_time);
			if (player_distance > engage_detection_radius) {	// player leaves detect zone
				state = EnemyState::WANDER;
				break;
			}
			break;
		
		case EnemyState::CHARGE:
			charge(delta_time);
			break;
		}

		GameObject::Update(delta_time);
	}


	void EnemyGameObject::takeDamage(float damage, glm::vec3 damage_position) {
		GameObject::takeDamage(damage);
		if (state != EnemyState::ENGAGE) {
			target_position = damage_position;
			state = EnemyState::ALERT;
			alert_timer.setTime(alert_time);
		}	
	}


	void EnemyGameObject::engageAction(double delta_time) {
		target_position = player_->GetPosition();
		moveTowardsTarget(delta_time, true);
	}


	void EnemyGameObject::charge(double delta_time) {
		state = EnemyState::ENGAGE;
	}


	void EnemyGameObject::moveTowardsTarget(double delta_time, bool ram_into_target) {
		// turns toward target
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

		float target_distance = glm::length(target_vector);

		// base angle of the isosceles triangle formed by distance traveled and turn radius
		float base_angle = (glm::pi<float>() - abs(angle_displacement)) / 2;
		// radius of the turn if current speed and turn speed is used
		float turn_radius = (speed * static_cast<float>(delta_time)) * sin(base_angle) / sin(abs(angle_displacement));
		
		// stop moving if unable to reach target by current turning speed
		if (ram_into_target) {
			// stop until 90 degrees to target
			if (turn_radius > target_distance && abs(angle_difference) >= (glm::pi<float>() * 90 / 360)) {
				target_distance = 0.0f;
			}
		} else {
			// stop until facing target
			if (turn_radius > target_distance && abs(angle_difference) >= 0.001f) {
				target_distance = 0.0f;
			}
		}
		
		// moves toward target
		glm::vec3 new_position = GetPosition() + self_direction * glm::min(target_distance, speed * static_cast<float>(delta_time));
		SetPosition(new_position);
	}


	void EnemyGameObject::turnTowardsTarget(double delta_time) {
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


	bool EnemyGameObject::isFacingTarget(float max_angle_difference) {
		glm::vec3 self_direction = GetBearing();
		glm::vec3 target_vector = target_position - GetPosition();

		float angle_difference = abs(Helper::angleBetweenVectors(self_direction, target_vector));

		return (angle_difference <= max_angle_difference);
	}


	glm::vec3 EnemyGameObject::getPlayerSpeedIntersactionPoint(glm::vec3 start_position, float self_speed, float time_offset) {
		glm::vec3 player_position = player_->GetPosition() + player_->GetVelocity() * time_offset;
		glm::vec3 player_destination = player_position + player_->GetVelocity();
		glm::vec3 player_velocity = player_->GetVelocity();

		float a = pow(player_velocity.x, 2) + pow(player_velocity.y, 2) - pow(self_speed, 2);
		float b = 2 * (player_velocity.x * (player_position.x - start_position.x) + player_velocity.y * (player_position.y - start_position.y));
		float c = pow(player_position.x - start_position.x, 2) + pow(player_position.y - start_position.y, 2);

		float t1 = (-b + sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);
		float t2 = (-b - sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);

		float t = max(t1, t2);

		if (t > 0) {
			// intersaction exist
			return player_position + player_velocity * t;
		}
		else {
			// intersaction doesn't exist
			return player_destination;
		}
	}

} // namespace game
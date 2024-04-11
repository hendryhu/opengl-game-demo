#include "ranged_enemy_object.h"

namespace game {

	RangedEnemyObject::RangedEnemyObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player, std::vector<GameObject*>* bullet_vector)
		: EnemyGameObject(position, geom, shader, texture, player) {
		type = EnemyType::RANGED_ENEMY;

		size = RANGED_ENEMY_SIZE;
		SetScale(size);
		setHitboxRadius(this->GetScale() / 2);
		setHealth(RANGED_ENEMY_HEALTH);

		melee_damage = RANGED_ENEMY_MELEE_DAMAGE;

		weight = RANGED_ENEMY_WEIGHT;

		idle_detection_radius = RANGED_ENEMY_IDLE_DETECTION_RADIUS;
		engage_detection_radius = RANGED_ENEMY_ENGAGE_DETECTION_RADIUS;

		idle_vision_cone = 0.0f;	// in radians. from 0 rad
		engage_vision_cone = 0.0f;	// in radians, from 0 rad

		speed = RANGED_ENEMY_SPEED;
		turn_speed = RANGED_ENEMY_TURN_SPEED;	// in radians

		wander_chance = RANGED_ENEMY_WANDER_CHANCE;
		wander_range = RANGED_ENEMY_WANDER_RANGE;

		bullet_objects_ = bullet_vector;
	}

	void RangedEnemyObject::Update(double delta_time) {

		fire_cooldown_timer.Update(delta_time);

		EnemyGameObject::Update(delta_time);
	}


	void RangedEnemyObject::engageAction(double delta_time) {
		float player_distance = glm::length(player_->GetPosition() - GetPosition());
		glm::vec3 player_direction = glm::normalize(player_->GetPosition() - GetPosition());

		if (player_distance < min_engage_range) {
			target_position = player_->GetPosition() - player_direction * min_engage_range;
			moveTowardsTarget(delta_time, true);
		}
		else if (player_distance > max_engage_range) {
			target_position = player_->GetPosition() - player_direction * max_engage_range;
			moveTowardsTarget(delta_time, true);
		}
		else {
			// find player bullet intersaction point
			target_position = getPlayerSpeedIntersactionPoint(GetPosition(), bullet->GetSpeed(), 0.0f);
			
			turnTowardsTarget(delta_time);
			if (isFacingTarget(glm::pi<float>() * 1 / 4)) {
				fire();
			}
		}
	}


	void RangedEnemyObject::fire()
	{
		// if the fire cooldown timer is still running, return nullptr
		if (fire_cooldown_timer.timesUp()) {
			// deep copy the bullet object
			BulletObject* new_bullet = new BulletObject(*bullet);

			// calculate bullet position, should be at the tip of the turret
			glm::vec3 bullet_position = GetPosition() + glm::vec3(glm::cos(angle_), glm::sin(angle_), 0.0f) * hitbox_radius;
			bullet_position.z = 0.0f;
			new_bullet->Shoot(bullet_position, GetBearing(), this);
			new_bullet->SetScale(5.0);
			new_bullet->SetRotation(angle_ - glm::pi<float>() / 2.0f);

			// add to bullet vector
			bullet_objects_->push_back(new_bullet);

			// reset the timer
			fire_cooldown_timer.setTime(1 / rate_of_fire);
		}
	}
	

} // namespace game
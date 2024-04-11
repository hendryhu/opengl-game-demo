#include "charge_enemy_object.h"

namespace game {

	ChargeEnemyObject::ChargeEnemyObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player, std::vector<GameObject*>* bullet_vector)
		: EnemyGameObject(position, geom, shader, texture, player) {
		type = EnemyType::MELEE_ENEMY;

		size = CHARGE_ENEMY_SIZE;
		SetScale(size);
		setHitboxRadius(this->GetScale() / 2);
		setHealth(CHARGE_ENEMY_HEALTH);

		melee_damage = CHARGE_ENEMY_MELEE_DAMAGE;

		weight = CHARGE_ENEMY_WEIGHT;

		idle_detection_radius = CHARGE_ENEMY_IDLE_DETECTION_RADIUS;
		engage_detection_radius = CHARGE_ENEMY_ENGAGE_DETECTION_RADIUS;

		idle_vision_cone = 0.0f;	// in radians. from 0 rad
		engage_vision_cone = 0.0f;	// in radians, from 0 rad

		speed = CHARGE_ENEMY_SPEED;
		turn_speed = CHARGE_ENEMY_TURN_SPEED;	// in radians

		wander_chance = CHARGE_ENEMY_WANDER_CHANCE;
		wander_range = CHARGE_ENEMY_WANDER_RANGE;

		bullet_objects_ = bullet_vector;
	}

	void ChargeEnemyObject::Update(double delta_time) {
		charge_prepare_timer.Update(delta_time);
		charge_timer.Update(delta_time);
		charge_cooldown_timer.Update(delta_time);
		fire_cooldown_timer.Update(delta_time);
		EnemyGameObject::Update(delta_time);
	}


	void ChargeEnemyObject::engageAction(double delta_time) {
		float player_distance = glm::length(player_->GetPosition() - GetPosition());
		glm::vec3 player_direction = glm::normalize(player_->GetPosition() - GetPosition());
		

		if (!fire_cooldown_timer.timesUp() || player_distance <= charge_range) {
			// charge
			target_position = getPlayerSpeedIntersactionPoint(GetPosition(), charge_speed, charge_prepare_time);
			if (player_distance > charge_range) {
				moveTowardsTarget(delta_time, true);
			}
			else {
				if (isFacingTarget(0.2f) && charge_cooldown_timer.timesUp()) {
					charge_prepare_timer.setTime(charge_prepare_time);
					charge_timer.setTime(charge_time + charge_prepare_time);
					state = EnemyState::CHARGE;
				}
				else {
					turnTowardsTarget(delta_time);
				}
			}
		}
		else if (player_distance <= max_engage_range) {
			// find player bullet intersaction point
			target_position = getPlayerSpeedIntersactionPoint(GetPosition(), bullet->GetSpeed(), 0.0f);

			turnTowardsTarget(delta_time);
			if (isFacingTarget(0)) {
				fire();
			}
		}
		else {
			// move towards player
			target_position = player_->GetPosition() - player_direction * max_engage_range;
			moveTowardsTarget(delta_time, true);
		}
	}


	void ChargeEnemyObject::fire()
	{
		// if the fire cooldown timer is still running, return nullptr
		if (fire_cooldown_timer.timesUp()) {
			// deep copy the bullet object
			BulletObject* new_bullet = new BulletObject(*bullet);

			// calculate bullet position, should be at the tip of the turret
			glm::vec3 bullet_position = GetPosition() + glm::vec3(glm::cos(angle_), glm::sin(angle_), 0.0f) * hitbox_radius;
			bullet_position.z = 0.0f;
			new_bullet->Shoot(bullet_position, GetBearing(), this);
			new_bullet->SetScale(10.0);
			new_bullet->SetRotation(angle_ - glm::pi<float>() / 2.0f);

			// add to bullet vector
			bullet_objects_->push_back(new_bullet);

			// reset the timer
			fire_cooldown_timer.setTime(1 / rate_of_fire);
		}
	}


	void ChargeEnemyObject::charge(double delta_time) {
		if (!charge_timer.timesUp()) {
			// charging
			if (charge_prepare_timer.timesUp()) {
				// not preparing for a charge
				SetPosition(GetPosition() + charge_speed * GetBearing() * static_cast<float>(delta_time));
			}
			else {
				// preparing for a charge
				SetPosition(GetPosition() - (speed / 2) * GetBearing() * static_cast<float>(delta_time));

			}
		}
		else {
			stopCharging();
		}
	}


	void ChargeEnemyObject::stopCharging() {
		charge_prepare_timer.setTime(0.0f);
		charge_timer.setTime(0.0f);
		charge_cooldown_timer.setTime(charge_cooldown_time);
		state = EnemyState::ENGAGE;
	}


} // namespace game
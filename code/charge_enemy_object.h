#ifndef CHARGE_ENEMY_OBJECT_H_
#define CHARGE_ENEMY_OBJECT_H_

#include "enemy_game_object.h"

namespace game {

	class ChargeEnemyObject : public EnemyGameObject {

	public:
		ChargeEnemyObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player, std::vector<GameObject*>* bullet_vector);

		void Update(double delta_time) override;

		// getters
		inline float getChargeDamage() override { return charge_damage; };

		// setters
		void setBullets(BulletObject* bullet) {
			this->bullet = bullet;
		}

		// identify
		inline bool isChargeType() override { return true; };

		// engage action
		void engageAction(double delta_time) override;
		void fire();
		void charge(double delta_time) override;

		// helpers
		inline bool isCharging() override { return (!charge_timer.timesUp()); };
		void stopCharging() override;

	private:
		float charge_range = CHARGE_ENEMY_CHARGE_RANGE;
		float charge_prepare_time = CHARGE_ENEMY_CHARGE_PREPARE_TIME;	// seconds
		float charge_time = CHARGE_ENEMY_CHARGE_TIME;	// seconds
		float charge_cooldown_time = CHARGE_ENEMY_CHARGE_COOLDOWN_TIME;	// seconds
		float charge_speed = CHARGE_ENEMY_CHARGE_SPEED;
		float charge_damage = CHARGE_ENEMY_CHARGE_DAMAGE;

		float max_engage_range = CHARGE_ENEMY_MAX_ENGAGE_RANGE;

		float rate_of_fire = CHARGE_ENEMY_RATE_OF_FIRE;

		// Bullet
		BulletObject* bullet = nullptr;  // bullet object to be fired (every time a bullet is fired, a copy of this object is created)
		std::vector <GameObject*>* bullet_objects_;
		
		// Timer
		Timer charge_prepare_timer;
		Timer charge_timer;
		Timer charge_cooldown_timer;
		Timer fire_cooldown_timer;

	}; // class MeleeEnemyObject

} // namespace game

#endif
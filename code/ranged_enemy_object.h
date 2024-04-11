#ifndef RANGED_ENEMY_OBJECT_H_
#define RANGED_ENEMY_OBJECT_H_

#include "enemy_game_object.h"

namespace game {

	class RangedEnemyObject : public EnemyGameObject {

	public:
		RangedEnemyObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player, std::vector<GameObject*>* bullet_vector);

		void Update(double delta_time) override;

		// Setters
		void setBullets(BulletObject* bullet) {
			this->bullet = bullet;
		}

		// identify
		inline bool isRangedType() override { return true; };

		// engage action
		void engageAction(double delta_time) override;
		void fire();

	private:
		float min_engage_range = RANGED_ENEMY_MIN_ENGAGE_RANGE;
		float max_engage_range = RANGED_ENEMY_MAX_ENGAGE_RANGE;

		float rate_of_fire = RANGED_ENEMY_RATE_OF_FIRE;

		// Timer
		Timer fire_cooldown_timer;

		// Bullet
		BulletObject* bullet = nullptr;  // bullet object to be fired (every time a bullet is fired, a copy of this object is created)
		std::vector <GameObject*>* bullet_objects_;

	}; // class RangedEnemyObject

} // namespace game

#endif
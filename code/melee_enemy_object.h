#ifndef MELEE_ENEMY_OBJECT_H_
#define MELEE_ENEMY_OBJECT_H_

#include "enemy_game_object.h"
#include "enemy_arm_object.h"
#include "saw_object.h"

namespace game {

	class MeleeEnemyObject : public EnemyGameObject {

	public:
		MeleeEnemyObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player, GLuint arm_texture, GLuint saw_texture);

		void Update(double delta_time) override;

		// identify
		inline bool isMeleeType() override { return true; };

		// engage action
		void engageAction(double delta_time) override;

		// getter
		inline SawObject* getSaw() { return saw_object; };

	private:
		SawObject* saw_object;

	}; // class MeleeEnemyObject

} // namespace game

#endif
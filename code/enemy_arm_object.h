#ifndef ENEMY_ARM_OBJECT_H_
#define ENEMY_ARM_OBJECT_H_

#include "game_object.h"
#include "saw_object.h"

namespace game {

	class EnemyArmObject : public GameObject {

	public:
		EnemyArmObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, bool swing = false);

		void Update(double delta_time) override;

		// getters
		glm::vec3 getModelPosition();

		// setters
		inline void addArm(EnemyArmObject* arm) { child_objects_.push_back(arm); };
		inline void addSaw(SawObject* saw) { saw_object = saw; };
		inline void setTargetPosition(glm::vec3 target_position) { this->target_position = target_position; };
		inline void setModelRotation(float angle) { parent_angle = angle; };

		// Helper
		bool isArm() override { return true; };
		void turnTowardsTarget(double delta_time);

	private:		
		bool swing = false;
		float max_swing_angle = 2.0f;
		float swinging_cw = true;	// swing direction
		float swing_angle = 0.0f;	// counter

		float model_offset_x;
		float child_attach_offset_x;

		float turn_speed = ARM_TURN_SPEED;

		glm::vec3 target_position = glm::vec3(0, 0, 0);
		float parent_angle = 0;

		SawObject* saw_object = nullptr;

	}; // class MeleeEnemyObject

} // namespace game

#endif
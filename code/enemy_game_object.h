#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_

#include "config.h"

#include "player_game_object.h"

namespace game {

	class EnemyGameObject : public GameObject {

	public:
		EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player);

		void Update(double delta_time) override;

		// getters
		inline EnemyType getType() { return type; };
		inline EnemyState getState() { return state; };
		inline float getIdleDetectionRadius() { return idle_detection_radius; };
		inline float getEngageDetectionRadius() { return engage_detection_radius; };
		inline virtual float getChargeDamage() { return 0.0f; };

		// setters
		void takeDamage(float damage, glm::vec3 damage_position);

		// identify
		inline virtual bool isMeleeType() { return false; };
		inline virtual bool isRangedType() { return false; };
		inline virtual bool isChargeType() { return false; };

		// engage action
		virtual void engageAction(double delta_time);
		virtual void charge(double delta_time);

		// movement
		void moveTowardsTarget(double delta_time, bool ram_into_target);
		void turnTowardsTarget(double delta_time);

		// Helpers
		bool isEnemy() override { return true; };

		bool isFacingTarget(float max_angle_difference);
		// finds the intersaction with the player given a start position and speed
		// returns player movement destination if intersaction not found
		glm::vec3 getPlayerSpeedIntersactionPoint(glm::vec3 start_position, float self_speed, float time_offset);	
		inline virtual bool isCharging() { return false; };
		inline virtual void stopCharging() {};

	protected:
		EnemyType type;	// enemy type

		EnemyState state = EnemyState::IDLE;	// enemy state

		PlayerGameObject* player_;	// player reference

		// properties
		float size = 1.0f;

		float idle_detection_radius = 5.0f;
		float alert_detection_radius = 10.0f;
		float engage_detection_radius = 15.0f;

		float idle_vision_cone = 0.0f;	// in radians. from 0 rad
		float alert_vision_cone = 0.0f;	// in radians. from 0 rad
		float engage_vision_cone = 0.0f;	// in radians, from 0 rad

		float speed = 4.0f;
		float turn_speed = 3.0f;	// in radians

		glm::vec3 target_position;

		float wander_chance = 0.5f;
		float wander_range = 10.0f;

		float alert_time = 5.0f;
		Timer alert_timer;

	}; // class EnemyGameObject

} // namespace game

#endif
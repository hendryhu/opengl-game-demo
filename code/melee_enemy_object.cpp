#include "melee_enemy_object.h"

namespace game {

	MeleeEnemyObject::MeleeEnemyObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, PlayerGameObject* player, GLuint arm_texture, GLuint saw_texture)
		: EnemyGameObject(position, geom, shader, texture, player) {
		type = EnemyType::MELEE_ENEMY;

		size = MELEE_ENEMY_SIZE;
		SetScale(size);
		setHitboxRadius(this->GetScale() / 2);
		setHealth(MELEE_ENEMY_HEALTH);

		melee_damage = MELEE_ENEMY_MELEE_DAMAGE;

		weight = MELEE_ENEMY_WEIGHT;

		idle_detection_radius = MELEE_ENEMY_IDLE_DETECTION_RADIUS;
		engage_detection_radius = MELEE_ENEMY_ENGAGE_DETECTION_RADIUS;

		idle_vision_cone = 0.0f;	// in radians. from 0 rad
		engage_vision_cone = 0.0f;	// in radians, from 0 rad

		speed = MELEE_ENEMY_SPEED;
		turn_speed = MELEE_ENEMY_TURN_SPEED;	// in radians

		wander_chance = MELEE_ENEMY_WANDER_CHANCE;
		wander_range = MELEE_ENEMY_WANDER_RANGE;

		// first arm
		EnemyArmObject* arm = new EnemyArmObject(GetPosition(), getGeometry(), getShader(), arm_texture, false);
		child_objects_.push_back(arm);

		// second arm
		EnemyArmObject* second_arm = new EnemyArmObject(GetPosition(), getGeometry(), getShader(), arm_texture, true);
		arm->addArm(second_arm);

		// saw
		saw_object = new SawObject(GetPosition(), getGeometry(), getShader(), saw_texture);
		saw_object->SetScale(2);
		saw_object->setHitboxRadius(saw_object->GetScale() / 2);
		saw_object->setWeight(SAW_WEIGHT);
		second_arm->addSaw(saw_object);
		child_objects_.push_back(saw_object);
	}

	void MeleeEnemyObject::Update(double delta_time) {

		// Update all child objects
		for (auto child : child_objects_) {
			if (child->isArm()) {
				EnemyArmObject* current = dynamic_cast<EnemyArmObject*>(child);
				current->SetPosition(GetPosition() + current->getModelPosition());
				current->setTargetPosition(target_position);
				current->setModelRotation(GetRotation());
			}
		}

		EnemyGameObject::Update(delta_time);
	}


	void MeleeEnemyObject::engageAction(double delta_time) {
		target_position = player_->GetPosition();
		moveTowardsTarget(delta_time, true);
	}


} // namespace game
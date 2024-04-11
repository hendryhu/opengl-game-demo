#include "explosion.h"

namespace game {
	
	Explosion::Explosion(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, int type, float cycle, float speed, float gravity, float scale)
		: GameObject(position, geom, shader, texture)
	{
		object_type = ObjectType::Explosion;
		despawn_time = cycle;
		timer.setTime(despawn_time);
		particles = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), geom, shader, texture, this, type, cycle, speed, gravity);
		particles->SetScale(scale);
	}

	Explosion::~Explosion() {
		delete particles;
	}

	const double Explosion::GetTimer(void) {
		return timer.getTime();
	}

	void Explosion::Update(double delta_time) {
		particles->Update(delta_time);
		timer.Update(delta_time);
	}

	void Explosion::Render(glm::mat4 view_matrix, double current_time) {
		particles->Render(view_matrix, current_time);
	}

	bool Explosion::timesUp() {
		return timer.timesUp();
	}
}

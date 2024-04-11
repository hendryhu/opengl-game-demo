#include "ui_object.h"

namespace game {

	UIObject::UIObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* player)
		: GameObject(position, geom, shader, texture) {
		object_type = ObjectType::UI;
		player_ = player;
	}
	
	UIObject::~UIObject(){
		delete health_bar;
		delete arrow;
	};

	void UIObject::setUp(Geometry* health_bar_geom, GLuint arrow_texture, Shader* arrow_shader, GameObject** win_1, GameObject** win_2, GameObject** win_3, GameObject** win_4) {
		health_bar = new HealthBar(health_bar_position, health_bar_geom, getShader(), getTexture(), player_);
		health_bar->SetScale(1.0f);

		arrow = new Arrow(arrow_position, getGeometry(), getShader(), arrow_texture, player_, win_1, win_2, win_3, win_4);
		arrow->SetScale(4.0f);
	}

	void UIObject::Update(double delta_time) {
		// move with player
		SetPosition(player_->GetPosition());

		health_bar->SetPosition(GetPosition() + health_bar->getModelPosition());

		arrow->SetPosition(GetPosition() + arrow->getModelPosition());
		arrow->Update(delta_time);
	}

	void UIObject::Render(glm::mat4 view_matrix, double current_time) {
		health_bar->Render(view_matrix, current_time);
		arrow->Render(view_matrix, current_time);
	}

} // namespace game
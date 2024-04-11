#include "timer.h"

namespace game {

	Timer::Timer() {};

	void Timer::Update(double delta_time) {
		time -= delta_time;
		
		if (time < 0.0f) {
			time = 0.0f;
		}
	}

} // namespace game
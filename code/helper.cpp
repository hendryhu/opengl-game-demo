#include "helper.h"

namespace game {

	float Helper::angleBetweenVectors(glm::vec3 a, glm::vec3 b) {
		float angleA = std::atan2(a.y, a.x);
		float angleB = std::atan2(b.y, b.x);

		float difference = angleB - angleA;

		difference = normalizeAngle(difference);

		return difference;
	}

	
	float Helper::normalizeAngle(float angle) {
		// normalize
		float pi = glm::pi<float>();
		while (angle <= pi) {
			angle += 2 * pi;
		}
		while (angle > pi) {
			angle -= 2 * pi;
		}
		return angle;
	}


	float Helper::normalizeAngleToPositive(float angle) {
		// normalize
		float pi = glm::pi<float>();
		while (angle <= 0) {
			angle += 2 * pi;
		}
		while (angle > (2 * pi)) {
			angle -= 2 * pi;
		}
		return angle;
	}


	void Helper::setSeed() {
		std::srand(static_cast<unsigned>(std::time(nullptr)));
	}

	glm::vec3 Helper::generate2dPosition(float xLower, float xUpper, float yLower, float yUpper) {
		// use default_random_engine as generator
		default_random_engine generator(rand());
		// define range for x
		uniform_real_distribution<float> distributionX(xLower, xUpper);
		// generate x
		float x = distributionX(generator);
		// define range for y
		uniform_real_distribution<float> distributionY(yLower, yUpper);
		// generate y
		float y = distributionY(generator);
		glm::vec3 output = glm::vec3(x, y, 0);
		return output;
	}


	glm::vec3 Helper::generate2dPosition(glm::vec3 start_position, float min_range, float max_range) {
		// use default_random_engine as generator
		default_random_engine generator(rand());
		// define range for spawn range
		uniform_real_distribution<float> distributionRange(min_range, max_range);
		// generate spawn range
		float range = distributionRange(generator);
		// define range for angle
		uniform_real_distribution<float> distributionAngle(0.0f, 2 * glm::pi<float>());
		// generate angle
		float angle = distributionAngle(generator);
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 output = glm::vec3(rotMat * glm::vec4(range, 0.0f, 0.0f, 1.0f));
		output += start_position;
		return output;
	}


	float Helper::generateFloat(float lowerbound, float upperbound) {
		// use default random engine
		default_random_engine generator(rand());
		// define range
		uniform_real_distribution<float> distribution(lowerbound, upperbound);
		// generate
		float output = distribution(generator);
		return output;
	}


	int Helper::generateInt(int lowerbound, int upperbound) {
		// use default random engine
		default_random_engine generator(rand());
		// define range
		uniform_int_distribution<int> distribution(lowerbound, upperbound);
		// generate
		int output = distribution(generator);
		return output;
	}


	bool Helper::roll(float successChance) {
		// clamp value between 0 - 100
		if (successChance < 0.0f) {
			successChance = -1.0f;	// so that 0% chance will not return true
		} else if (successChance > 100.0f) {
			successChance = 100.0f;
		}

		return (generateFloat(0, 100) <= successChance);
	}

} // namespace game
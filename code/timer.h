#ifndef TIMER_H_
#define TIMER_H_

namespace game {

	class Timer {

	public:
		Timer();

		void Update(double delta_time);

		// Getter
		inline float getTime() { return time; };
		inline bool timesUp() { return time <= 0.0f; };

		// Setter
		inline void setTime(float new_time) { time = new_time; };
		inline void increaseTime(float increment) { time += increment; };
		inline void decreaseTime(float decrement) { increaseTime(-decrement); };

	private:
		float time = 0.0f;	// in seconds

	}; // class Timer

} // namespace game

#endif
#pragma once
#include <chrono>
#include "Function/Message/Message.h"
namespace TinyRenderer
{
	class StopWatch
	{
	public:
		enum class State { STOPPED, RUNNING, PAUSED };

		StopWatch() :begin_time_(std::chrono::system_clock::time_point()), accumulated_time_(std::chrono::system_clock::duration()), state_(State::PAUSED) {};
		~StopWatch() {};
		void Start()
		{
			begin_time_ = std::chrono::system_clock::now();
			state_ = State::RUNNING;
		}

		void Pause()
		{
			if (state_ == State::PAUSED || state_ == State::STOPPED)
			{
				LOG_WARN("Stopwatch is already paused or stopped");
				return;
			}
			auto now = std::chrono::system_clock::now();
			accumulated_time_ += now - begin_time_;
			state_ = State::PAUSED;
		}

		// ÔÝÍ£ºó¼ÌÐø
		void Restart()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is already running");
				return;
			}
			Start();
		}

		void Reset()
		{
			state_ = State::STOPPED;
			begin_time_ = std::chrono::system_clock::time_point();
			accumulated_time_ = std::chrono::system_clock::duration();
		}
		//ÄÉÃë
		std::int64_t nanoseconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//×ªÄÉÃë
			return std::chrono::duration_cast<std::chrono::nanoseconds>(accumulated_time_).count();
		}
		//Î¢Ãë
		std::int64_t microseconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//×ªÎ¢Ãî
			return std::chrono::duration_cast<std::chrono::microseconds>(accumulated_time_).count();
		}
		//ºÁÃë
		std::int64_t milliseconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//×ªºÁÃî
			return std::chrono::duration_cast<std::chrono::milliseconds>(accumulated_time_).count();
		}
		//Ãë
		std::int64_t seconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//×ªÃî
			return std::chrono::duration_cast<std::chrono::seconds>(accumulated_time_).count();
		}

		inline State getState() const { return state_; }
	private:
		std::chrono::system_clock::time_point begin_time_;
		std::chrono::system_clock::duration accumulated_time_;
		State state_;
	};
} // namespace TinyRenderer
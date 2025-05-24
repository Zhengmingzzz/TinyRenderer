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

		// 暂停后继续
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
		//纳秒
		std::int64_t nanoseconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//转纳秒
			return std::chrono::duration_cast<std::chrono::nanoseconds>(accumulated_time_).count();
		}
		//微秒
		std::int64_t microseconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//转微妙
			return std::chrono::duration_cast<std::chrono::microseconds>(accumulated_time_).count();
		}
		//毫秒
		std::int64_t milliseconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//转毫妙
			return std::chrono::duration_cast<std::chrono::milliseconds>(accumulated_time_).count();
		}
		//秒
		std::int64_t seconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//转妙
			return std::chrono::duration_cast<std::chrono::seconds>(accumulated_time_).count();
		}

		inline State getState() const { return state_; }
	private:
		std::chrono::system_clock::time_point begin_time_;
		std::chrono::system_clock::duration accumulated_time_;
		State state_;
	};
} // namespace TinyRenderer
#pragma once
#include <chrono>
#include "Function/Message/Message.h"
namespace TinyRenderer
{
	class StopWatch
	{
		enum class State { STOPPED, RUNNING, PAUSED };
	public:
		StopWatch() :begin_time_(std::chrono::system_clock::time_point()), accumulated_time_(std::chrono::system_clock::duration()), state_(State::PAUSED) {};
		~StopWatch() {};
		void Start()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is already running");
				return;
			}
			begin_time_ = std::chrono::system_clock::now();
			state_ = State::RUNNING;
		}
		void Stop()
		{
			if (state_ == State::PAUSED || state_ == State::STOPPED)
			{
				LOG_WARN("Stopwatch is already stopped");
				return;
			}
			auto now = std::chrono::system_clock::now();
			accumulated_time_ += now - begin_time_;
			state_ = State::STOPPED;
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

		void Restart()
		{
			Start();
		}

		void Reset()
		{
			state_ = State::PAUSED;
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
			//×ªÎ¢Ãî
			return std::chrono::duration_cast<std::chrono::milliseconds>(accumulated_time_).count();
		}
		//Ãë
		std::int64_t seconds()
		{
			if (state_ == State::RUNNING)
			{
				LOG_WARN("Stopwatch is running, please stop it first");
			}
			//×ªÎ¢Ãî
			return std::chrono::duration_cast<std::chrono::seconds>(accumulated_time_).count();
		}
	private:
		std::chrono::system_clock::time_point begin_time_;
		std::chrono::system_clock::duration accumulated_time_;
		State state_;
	};

	#define StopWatch_Begin(arg_name) \
			StopWatch stopWatch_##arg_name; \
			stopWatch_##arg_name.Start();

	#define StopWatch_End(arg_name) \
			stopWatch_##arg_name.Stop();

	#define StopWatch_Pause(arg_name) \
			stopWatch_##arg_name.Pause();

	#define StopWatch_Reset(arg_name) \
			stopWatch_##arg_name.Reset();

	#define StopWatch_microseconds(arg_name) \
			LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name.microseconds() << " microseconds")

			
	#define StopWatch_milliseconds(arg_name) \
			LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name.milliseconds() << " milliseconds")

	#define StopWatch_seconds(arg_name) \
			LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name.seconds() << " seconds")

	#define StopWatch_nanoseconds(arg_name) \
	LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name.nanoseconds() << " nanoseconds")

} // namespace TinyRenderer
#pragma once
#include <cstddef>
#include <mutex>

// void* operator new(size_t size);
// void operator delete(void* ptr, size_t size) noexcept;
// void* operator new[](size_t size);
// void operator delete[](void* ptr, size_t size) noexcept;

namespace TinyRenderer
{
	/// <summary>
	/// 定义引擎初始化顺序
	/// </summary>
	class Application
	{
	public:
		virtual Application* instance() = 0;
		/// <summary>
		/// 引擎的主函数
		/// </summary>
		/// <returns></returns>
		virtual int Main(int argc, char** argv) = 0;
		virtual bool Run() = 0;
		virtual bool InitialEngine() = 0;
		virtual bool TerminalEngine() = 0;

		virtual ~Application() = default;
	protected:
		//应用程序单例
		static Application* instance_;
		Application() {};


	};

	class WindowsApplication : public Application
	{
	public:
		Application* instance() override;
		int Main(int argc, char** argv) override;

		
	private:
		bool Run() override;
		bool InitialEngine() override;
		bool TerminalEngine() override;

		~WindowsApplication() override
		{};
	};
}
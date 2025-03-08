#pragma once
namespace TinyRenderer
{
	#define DEBUG_MODE 1
	/// <summary>
	/// 实现引擎组件间初始化/销毁顺序 定义引擎当前的模式(DEBUG/RELEASE)
	/// </summary>
	class Application
	{
	public:
		/// <summary>
		/// 获取程序单例
		/// </summary>
		/// <returns></returns>
		virtual Application* getInstance() = 0;
		/// <summary>
		/// 程序主函数
		/// </summary>
		/// <returns></returns>
		virtual int Main(int argc, void* argv) = 0;
		virtual bool run() = 0;
		virtual bool initialEngine() = 0;
		virtual bool terminalEngine() = 0;

		virtual ~Application() = default;
	protected:
		//由getInstance调用后分配内存
		static Application* Instance;
		Application() {};

	};

	class WindowsApplication : public Application
	{
	public:
		Application* getInstance() override;
		int Main(int argc, void* argv) override;

		
	private:
		bool run() override;
		bool initialEngine() override;
		bool terminalEngine() override;

		~WindowsApplication() override
		{};
	};
}
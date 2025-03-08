#pragma once
namespace TinyRenderer
{
	#define DEBUG_MODE 1
	/// <summary>
	/// ʵ������������ʼ��/����˳�� �������浱ǰ��ģʽ(DEBUG/RELEASE)
	/// </summary>
	class Application
	{
	public:
		/// <summary>
		/// ��ȡ������
		/// </summary>
		/// <returns></returns>
		virtual Application* getInstance() = 0;
		/// <summary>
		/// ����������
		/// </summary>
		/// <returns></returns>
		virtual int Main(int argc, void* argv) = 0;
		virtual bool run() = 0;
		virtual bool initialEngine() = 0;
		virtual bool terminalEngine() = 0;

		virtual ~Application() = default;
	protected:
		//��getInstance���ú�����ڴ�
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
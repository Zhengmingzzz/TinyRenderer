#pragma once
namespace TinyRenderer
{
	#define DEBUG_MODE 1
	/// <summary>
	/// ���������ʼ��˳��
	/// </summary>
	class Application
	{
	public:
		virtual Application* GetInstance() = 0;
		/// <summary>
		/// �����������
		/// </summary>
		/// <returns></returns>
		virtual int Main(int argc, char** argv) = 0;
		virtual bool Run() = 0;
		virtual bool InitialEngine() = 0;
		virtual bool TerminalEngine() = 0;

		virtual ~Application() = default;
	protected:
		//Ӧ�ó�����
		static Application* instance;
		Application() {};

	};

	class WindowsApplication : public Application
	{
	public:
		Application* GetInstance() override;
		int Main(int argc, char** argv) override;

		
	private:
		bool Run() override;
		bool InitialEngine() override;
		bool TerminalEngine() override;

		~WindowsApplication() override
		{};
	};
}
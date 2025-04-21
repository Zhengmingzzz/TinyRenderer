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
	/// ���������ʼ��˳��
	/// </summary>
	class Application
	{
	public:
		virtual Application* instance() = 0;
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
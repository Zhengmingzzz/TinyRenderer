#include "example.h"
#include "Platform/FileServer/FileServer.h"
#include "Platform/Path.h"
#include "Function/Message/Message.h"
#include <iostream>
#include "Function/Event/Delegate.h"
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include "Function/StopWatch/StopWatchManager.h"
//#include "Core/ResourceManager/ResourceManager.h"
#include "Core/MemoryManager/MemoryManager.h"
#include "Core/ThreadPool/ThreadPool.h"
#include "Function/Counter/CounterManager.h"


using json = nlohmann::json;
using namespace TinyRenderer;
using namespace std;

int func(int a, int b)
{
	cout << a << b << endl;
	return a;
}

static float s_func(float a) {
	cout << a << endl;
	return a;
}

class A
{
public:
	char C_Method()
	{
		cout << "���Ǹ���Ա����" << endl;
		return 'A';
	}
	static void S_Method()
	{
		cout << "���Ǹ���̬����" << endl;
	}

	virtual int V_Method(char b)
	{
		cout << "���Ǹ�A���麯��" << endl;
		return 0;
	}
};

class B : public A
{
public:
	int V_Method(char b) override
	{
		cout << "���Ǹ�B���麯��" << endl;
		return 0;
	}
};

void delegate_test()
{
	//delegate_test();
	// ʹ��ί�����͵���ȫ�ֺ���
	Delegate<int(int, int)> d1 = Delegate<int(int, int)>::Bind(func);
	d1(1, 5);
	// ���þ�̬����
	Delegate<float(float)> d2 = Delegate<float(float)>::Bind(s_func);
	d2(1.2);
	// ����lambda���ʽ
	int i_c = 5;
	Delegate<void(int)> d3 = Delegate<void(int)>::Bind([i_c](int a) {
		cout << a + i_c << endl;
		});
	d3(100);

	// ���ó�Ա����
	A a;
	auto d4 = Delegate<char()>::Bind(&a, &A::C_Method);
	cout << d4() << endl;

}

void json_test()
{
	// ���� JSON ����
	json j = {
		{"name", "John Doe"},
		{"age", 30},
		{"is_student", false},
		{"courses", {"Math", "Science"}}
	};

	// ����Ƕ�� JSON ����
	json j2;
	j2["name"] = "j2";
	j2["parent"] = "j";

	json j3;
	j3["name"] = "j3";
	j3["parent"] = "j2";

	j2["cihld"] = j3;
	j["child"] = j2;

	j["age"] = 31;  // �޸�����

	// ������� JSON ����
	cout << j.dump(4) << endl;

	// ������ͨ��nlohmann/json������JSON�ַ���
	std::string json_str = j.dump(4); // ���ɸ�ʽ����JSON�ַ���

	// ���浽�ļ�
	Path p = FileServer::getRootPath() / "example" / "data.json";
	std::ofstream out_file(p.toString(), ios::out);
	if (out_file.is_open()) {
		out_file << json_str;
		out_file.close();
		std::cout << "JSON�ļ��ѱ���" << std::endl;
	}
	else {
		std::cerr << "�޷����ļ�" << std::endl;
	}
}

void threadpool_test() {
	ThreadPool::instance().startUp(2);
	TaskResult<void> res1 = ThreadPool::instance().enqueue([]{
		cout << ("����ִ������1") << endl;
	}, TaskPriority::Medium);
	TaskResult<void> res2 = ThreadPool::instance().enqueue([]{
		cout << ("����ִ������2")<<endl;
	}, TaskPriority::Medium);
	TaskResult<void> res3 = ThreadPool::instance().enqueue([]{
		cout << ("����ִ������3")<<endl;
	}, TaskPriority::Medium);
	TaskResult<void> res4 = ThreadPool::instance().enqueue([]{
		cout << ("����ִ������4") << endl;
	}, TaskPriority::Low);

	ThreadPool::instance().enqueue([]{ThreadPool::instance().adjust_workers();}, TaskPriority::High);

	ThreadPool::instance().shutDown();
}


#include <iostream>
#include <vector>
#include <random>
void SimpleMemoryTest() {
	// ���ò���
	constexpr bool use_custom_alloc = true;  // trueʹ��newElement��falseʹ�ñ�׼new
	constexpr int test_cycles = 100000;       // ����ѭ������
	constexpr int max_block_size = 128;     // ����ڴ���С

	struct MemBlock {
		void* ptr;
		size_t size;
		bool is_custom;
	};

	std::vector<MemBlock> blocks;            // �ڴ���¼��
	std::mt19937 gen(std::random_device{}()); // �����������
	std::uniform_int_distribution<size_t> size_dist(1, max_block_size);
	std::bernoulli_distribution alloc_dist(0.6); // 60%�������

	// �ڴ����ѭ��
	for (int i = 0; i < test_cycles; ++i) {
		if (use_custom_alloc) {
			StopWatch_Start(mem_tick_time);
			MemoryManager::instance().tick();
			StopWatch_Pause(mem_tick_time);
		}
		if (alloc_dist(gen)) { // �����ڴ�
			size_t size = size_dist(gen);
			void* ptr = nullptr;

			if (use_custom_alloc) {
				ptr = newElement<char>(size); // �Զ��������
			} else {
				ptr = new char[size];         // ��׼����
			}

			if (ptr) {
				blocks.push_back({ptr, size, use_custom_alloc});
			}
		} else if (!blocks.empty()) { // �ͷ��ڴ�
			auto& block = blocks.back();

			if (block.is_custom) {
				deleteElement(static_cast<char*>(block.ptr)); // �Զ����ͷ�
			} else {
				delete[] static_cast<char*>(block.ptr);       // ��׼�ͷ�
			}

			blocks.pop_back();
		}
	}

	// ��������ڴ棨��ȫ��ʩ��
	for (auto& block : blocks) {
		if (block.is_custom) {
			deleteElement(static_cast<char*>(block.ptr));
		} else {
			delete[] static_cast<char*>(block.ptr);
		}
	}

	std::cout << "\n=== ���Խ�� ===\n"
			  << "�ܲ�������: " << test_cycles << "\n"
			  << "�ڴ�й©���: " << blocks.size() << " ��δ�ͷſ�\n";
}

void mem_time_test() {
	CounterManager::instance().startUp();
	StopWatchManager::instance().startUp();

	StopWatch_Start(mem_test);
	SimpleMemoryTest();
	StopWatch_Pause(mem_test);
	StopWatch_Microseconds(mem_test);
	StopWatch_Microseconds(mem_tick_time);
	Counter_GetCnt(mem_new_cnt);
	StopWatch_Microseconds(mem_new_time);
	StopWatch_Microseconds(mem_total_allocation_time);
	Counter_GetCnt(mem_alloc_newPage_cnt);
	StopWatch_Microseconds(mem_alloc_newPage_time);
	Counter_GetCnt(mem_try_allocate_block_cnt);
	Counter_GetCnt(mem_failed_allocate_block);
	Counter_GetCnt(mem_successful_allocate_block);
	StopWatch_Microseconds(mem_try_allocate_time);
	StopWatch_Microseconds(mem_getblock_time);
	StopWatch_Microseconds(mem_delete_time);
	StopWatchManager::instance().shutDown();
	CounterManager::instance().shutDown();
}

void Example::Main() {
	MemoryManager::instance().startUp();
	ThreadPool::instance().startUp();

	mem_time_test();

	ThreadPool::instance().shutDown();
	MemoryManager::instance().shutDown();
}
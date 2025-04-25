#include "example.h"
#include "Platform/FileServer/FileServer.h"
#include "Platform/Path.h"
#include "Function/Message/Message.h"
#include <iostream>
#include "Function/Event/Delegate.h"
#include "glm/glm.hpp"
#include "Function/CommonType/json.h"
#include <fstream>
#include "Function/StopWatch/StopWatchManager.h"
//#include "Core/ResourceManager/ResourceManager.h"
#include "Core/MemoryManager/MemoryManager.h"
#include "Core/ThreadPool/ThreadPool.h"
#include "Function/Counter/CounterManager.h"


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
	Delegate<int(int, int)> d1 = Delegate<int(int, int)>::bind(func);
	d1(1, 5);
	// ���þ�̬����
	Delegate<float(float)> d2 = Delegate<float(float)>::bind(s_func);
	d2(1.2);
	// ����lambda���ʽ
	int i_c = 5;
	Delegate<void(int)> d3 = Delegate<void(int)>::bind([i_c](int a) {
		cout << a + i_c << endl;
		});
	d3(100);

	// ���ó�Ա����
	std::shared_ptr<A> a = make_shared<A>();
	auto d4 = Delegate<char()>::bind<A>(a, &A::C_Method);
	cout << d4() << endl;
	cout << static_cast<bool>(d4) << endl;
	a.reset();
	d4();

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
	Path p = FileServer::get_rootPath() / "example" / "data.json";
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


#include <vector>
#include <random>
#include <iostream>

void SimpleMemoryTest() {
    // ���ò���
    constexpr bool use_custom_alloc = true;  // �л�����������
    constexpr int test_cycles = 1000000;     // �ܲ��Դ������������ܵ�����
    constexpr int max_block_size = 256;      // ��������С[3,5](@ref)

    struct MemBlock {
        void* ptr;
        size_t size;
        bool is_custom;
    };

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> size_dist(1, max_block_size);
    std::uniform_int_distribution<int> alloc_count_dist(1, 20);  // ÿ�η���1-5��[2](@ref)
    std::uniform_int_distribution<int> free_count_dist(0, 3);   // ÿ���ͷ�0-3��
    std::bernoulli_distribution action_dist(0.7);               // 70%����ִ�в���
    std::vector<MemBlock> blocks;

    for (int i = 0; i < test_cycles; ++i) {
        // �Զ�����������ڴ���������
        if (use_custom_alloc) {
        	StopWatch_Start(tick_time);
            MemoryManager::instance().tick();  // ģ���ڴ�����
        	StopWatch_Pause(tick_time);
        }

        if (action_dist(gen)) {
            // ����׶Σ�ִ��1-5���������
            int alloc_times = alloc_count_dist(gen);
            for (int j = 0; j < alloc_times; ++j) {
                size_t size = size_dist(gen);
                void* ptr = use_custom_alloc ?
                    newElement(char,size) :
                    new char[size];

                if (ptr) {
                    blocks.push_back({ptr, size, use_custom_alloc});
                }
            }
        } else {
            // �ͷŽ׶Σ�ִ��0-3������ͷ�
            int free_times = std::min(free_count_dist(gen),
                                     static_cast<int>(blocks.size()));
            for (int j = 0; j < free_times; ++j) {
                auto& block = blocks.back();
                if (block.is_custom) {
                    deleteElement(static_cast<char*>(block.ptr));
                } else {
                    delete[] static_cast<char*>(block.ptr);
                }
                blocks.pop_back();
            }
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

    std::cout << "=== ѹ�����Խ�� ===\n"
              << "�ܵ�������: " << test_cycles << "\n"
              << "��ֵ�ڴ����: " << blocks.capacity() << "\n"
              << "����й©����: " << blocks.size() << "\n";
}

void mem_time_test() {
	CounterManager::instance().startUp();
	StopWatchManager::instance().startUp();

	StopWatch sw;
	sw.Start();
	SimpleMemoryTest();
	sw.Pause();
	std::cout << "mem_test cost: " << sw.microseconds() << " microseconds" << std::endl;
	StopWatch_Microseconds(tick_time);
	StopWatch_Microseconds(alloc_time);
	StopWatch_Microseconds(deallocate_time);

	StopWatchManager::instance().shutDown();
	CounterManager::instance().shutDown();
}

void Example::Main() {
	MemoryManager::instance().startUp();
	//ThreadPool::instance().startUp();

	mem_time_test();

	//ThreadPool::instance().shutDown();
	MemoryManager::instance().shutDown();
}
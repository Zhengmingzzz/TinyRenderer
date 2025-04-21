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
		cout << "这是个成员函数" << endl;
		return 'A';
	}
	static void S_Method()
	{
		cout << "这是个静态函数" << endl;
	}

	virtual int V_Method(char b)
	{
		cout << "这是个A的虚函数" << endl;
		return 0;
	}
};

class B : public A
{
public:
	int V_Method(char b) override
	{
		cout << "这是个B的虚函数" << endl;
		return 0;
	}
};

void delegate_test()
{
	//delegate_test();
	// 使用委托类型调用全局函数
	Delegate<int(int, int)> d1 = Delegate<int(int, int)>::Bind(func);
	d1(1, 5);
	// 调用静态函数
	Delegate<float(float)> d2 = Delegate<float(float)>::Bind(s_func);
	d2(1.2);
	// 调用lambda表达式
	int i_c = 5;
	Delegate<void(int)> d3 = Delegate<void(int)>::Bind([i_c](int a) {
		cout << a + i_c << endl;
		});
	d3(100);

	// 调用成员函数
	A a;
	auto d4 = Delegate<char()>::Bind(&a, &A::C_Method);
	cout << d4() << endl;

}

void json_test()
{
	// 创建 JSON 对象
	json j = {
		{"name", "John Doe"},
		{"age", 30},
		{"is_student", false},
		{"courses", {"Math", "Science"}}
	};

	// 测试嵌套 JSON 对象
	json j2;
	j2["name"] = "j2";
	j2["parent"] = "j";

	json j3;
	j3["name"] = "j3";
	j3["parent"] = "j2";

	j2["cihld"] = j3;
	j["child"] = j2;

	j["age"] = 31;  // 修改年龄

	// 输出整个 JSON 对象
	cout << j.dump(4) << endl;

	// 假设已通过nlohmann/json库生成JSON字符串
	std::string json_str = j.dump(4); // 生成格式化的JSON字符串

	// 保存到文件
	Path p = FileServer::getRootPath() / "example" / "data.json";
	std::ofstream out_file(p.toString(), ios::out);
	if (out_file.is_open()) {
		out_file << json_str;
		out_file.close();
		std::cout << "JSON文件已保存" << std::endl;
	}
	else {
		std::cerr << "无法打开文件" << std::endl;
	}
}

void threadpool_test() {
	ThreadPool::instance().startUp(2);
	TaskResult<void> res1 = ThreadPool::instance().enqueue([]{
		cout << ("正在执行任务1") << endl;
	}, TaskPriority::Medium);
	TaskResult<void> res2 = ThreadPool::instance().enqueue([]{
		cout << ("正在执行任务2")<<endl;
	}, TaskPriority::Medium);
	TaskResult<void> res3 = ThreadPool::instance().enqueue([]{
		cout << ("正在执行任务3")<<endl;
	}, TaskPriority::Medium);
	TaskResult<void> res4 = ThreadPool::instance().enqueue([]{
		cout << ("正在执行任务4") << endl;
	}, TaskPriority::Low);

	ThreadPool::instance().enqueue([]{ThreadPool::instance().adjust_workers();}, TaskPriority::High);

	ThreadPool::instance().shutDown();
}


#include <iostream>
#include <vector>
#include <random>
void SimpleMemoryTest() {
	// 配置参数
	constexpr bool use_custom_alloc = true;  // true使用newElement，false使用标准new
	constexpr int test_cycles = 100000;       // 测试循环次数
	constexpr int max_block_size = 128;     // 最大内存块大小

	struct MemBlock {
		void* ptr;
		size_t size;
		bool is_custom;
	};

	std::vector<MemBlock> blocks;            // 内存块记录器
	std::mt19937 gen(std::random_device{}()); // 随机数生成器
	std::uniform_int_distribution<size_t> size_dist(1, max_block_size);
	std::bernoulli_distribution alloc_dist(0.6); // 60%分配概率

	// 内存测试循环
	for (int i = 0; i < test_cycles; ++i) {
		if (use_custom_alloc) {
			StopWatch_Start(mem_tick_time);
			MemoryManager::instance().tick();
			StopWatch_Pause(mem_tick_time);
		}
		if (alloc_dist(gen)) { // 分配内存
			size_t size = size_dist(gen);
			void* ptr = nullptr;

			if (use_custom_alloc) {
				ptr = newElement<char>(size); // 自定义分配器
			} else {
				ptr = new char[size];         // 标准分配
			}

			if (ptr) {
				blocks.push_back({ptr, size, use_custom_alloc});
			}
		} else if (!blocks.empty()) { // 释放内存
			auto& block = blocks.back();

			if (block.is_custom) {
				deleteElement(static_cast<char*>(block.ptr)); // 自定义释放
			} else {
				delete[] static_cast<char*>(block.ptr);       // 标准释放
			}

			blocks.pop_back();
		}
	}

	// 清理残留内存（安全措施）
	for (auto& block : blocks) {
		if (block.is_custom) {
			deleteElement(static_cast<char*>(block.ptr));
		} else {
			delete[] static_cast<char*>(block.ptr);
		}
	}

	std::cout << "\n=== 测试结果 ===\n"
			  << "总操作次数: " << test_cycles << "\n"
			  << "内存泄漏检测: " << blocks.size() << " 个未释放块\n";
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
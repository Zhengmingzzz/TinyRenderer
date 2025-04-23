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
	Delegate<int(int, int)> d1 = Delegate<int(int, int)>::bind(func);
	d1(1, 5);
	// 调用静态函数
	Delegate<float(float)> d2 = Delegate<float(float)>::bind(s_func);
	d2(1.2);
	// 调用lambda表达式
	int i_c = 5;
	Delegate<void(int)> d3 = Delegate<void(int)>::bind([i_c](int a) {
		cout << a + i_c << endl;
		});
	d3(100);

	// 调用成员函数
	std::shared_ptr<A> a = make_shared<A>();
	auto d4 = Delegate<char()>::bind<A>(a, &A::C_Method);
	cout << d4() << endl;
	cout << static_cast<bool>(d4) << endl;
	a.reset();
	d4();

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
	Path p = FileServer::get_rootPath() / "example" / "data.json";
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


#include <vector>
#include <random>
#include <iostream>

void SimpleMemoryTest() {
    // 配置参数
    constexpr bool use_custom_alloc = true;  // 切换分配器类型
    constexpr int test_cycles = 1000000;     // 总测试次数（根据性能调整）
    constexpr int max_block_size = 256;      // 最大分配块大小[3,5](@ref)

    struct MemBlock {
        void* ptr;
        size_t size;
        bool is_custom;
    };

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> size_dist(1, max_block_size);
    std::uniform_int_distribution<int> alloc_count_dist(1, 20);  // 每次分配1-5次[2](@ref)
    std::uniform_int_distribution<int> free_count_dist(0, 3);   // 每次释放0-3次
    std::bernoulli_distribution action_dist(0.7);               // 70%概率执行操作
    std::vector<MemBlock> blocks;

    for (int i = 0; i < test_cycles; ++i) {
        // 自定义分配器的内存整理周期
        if (use_custom_alloc) {
        	StopWatch_Start(tick_time);
            MemoryManager::instance().tick();  // 模拟内存整理
        	StopWatch_Pause(tick_time);
        }

        if (action_dist(gen)) {
            // 分配阶段：执行1-5次随机分配
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
            // 释放阶段：执行0-3次随机释放
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

    // 清理残留内存（安全措施）
    for (auto& block : blocks) {
        if (block.is_custom) {
            deleteElement(static_cast<char*>(block.ptr));
        } else {
            delete[] static_cast<char*>(block.ptr);
        }
    }

    std::cout << "=== 压力测试结果 ===\n"
              << "总迭代次数: " << test_cycles << "\n"
              << "峰值内存块数: " << blocks.capacity() << "\n"
              << "最终泄漏块数: " << blocks.size() << "\n";
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
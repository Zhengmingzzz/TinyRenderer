#include "example.h"
#include "Function/Message/Message.h"
#include <iostream>
#include "Core/Event/Delegate.h"
#include "glm/glm.hpp"
#include "Function/CommonType/json.h"
#include <fstream>
#include "Function/StopWatch/StopWatchManager.h"
#include "Core/MemoryManager/MemoryManager.h"
#include "Core/ThreadPool/ThreadPool.h"
#include "Function/Components/login/login.h"
#include "Function/Components/Transform/Transform.h"
#include "Function/Counter/CounterManager.h"
#include "Function/Framework/Component/Component.h"
#include "Function/Framework/GameObject/GameObject.h"


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
		cout << "????????????" << endl;
		return 'A';
	}
	static void S_Method()
	{
		cout << "????????????" << endl;
	}

	virtual int V_Method(char b)
	{
		cout << "?????A???麯??" << endl;
		return 0;
	}
};

class B : public A
{
public:
	int V_Method(char b) override
	{
		cout << "?????B???麯??" << endl;
		return 0;
	}
};

void delegate_test()
{
	//delegate_test();
	// ???????????????????
	Delegate<int(int, int)> d1 = Delegate<int(int, int)>::bind(func);
	d1(1, 5);
	// ??????????
	Delegate<float(float)> d2 = Delegate<float(float)>::bind(s_func);
	d2(1.2);
	// ????lambda????
	int i_c = 5;
	Delegate<void(int)> d3 = Delegate<void(int)>::bind([i_c](int a) {
		cout << a + i_c << endl;
		});
	d3(100);

	// ???ó??????
	std::shared_ptr<A> a = make_shared<A>();
	auto d4 = Delegate<char()>::bind<A>(a, &A::C_Method);
	cout << d4() << endl;
	cout << static_cast<bool>(d4) << endl;
	a.reset();
	d4();

}

void threadpool_test() {
	ThreadPool::get_instance().startUp(2);
	TaskResult<void> res1 = ThreadPool::get_instance().enqueue(
            TaskPriority::Medium,
            []{ cout << ("???????????1") << endl;});

	TaskResult<void> res2 = ThreadPool::get_instance().enqueue(
            TaskPriority::Medium,
            []{ cout << ("???????????2")<<endl; });

	TaskResult<void> res3 = ThreadPool::get_instance().enqueue(
            TaskPriority::Medium,
            []{ cout << ("???????????3")<<endl; });

	TaskResult<void> res4 = ThreadPool::get_instance().enqueue(
            TaskPriority::Low,
            []{ cout << ("???????????4") << endl; });

	ThreadPool::get_instance().enqueue(TaskPriority::High, []{ThreadPool::get_instance().adjust_workers();});

	ThreadPool::get_instance().shutdown();
}


#include <vector>
#include <random>
#include <iostream>

void SimpleMemoryTest() {
    // ???ò???
    constexpr bool use_custom_alloc = false;  // ?л???????????
    constexpr int test_cycles = 10000;     // ???????????????????????
    constexpr int max_block_size = 256;      // ????????С[3,5](@ref)

    struct MemBlock {
        void* ptr;
        size_t size;
        bool is_custom;
    };

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> size_dist(1, max_block_size);
    std::uniform_int_distribution<int> alloc_count_dist(1, 20);  // ??η???1-5??[2](@ref)
    std::uniform_int_distribution<int> free_count_dist(0, 3);   // ??????0-3??
    std::bernoulli_distribution action_dist(0.7);               // 70%??????в???
    std::vector<MemBlock> blocks;

    for (int i = 0; i < test_cycles; ++i) {
        if (action_dist(gen)) {
            // ?????Σ????1-5?????????
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
            // ????Σ????0-3????????
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

    // ?????????棨????????
    for (auto& block : blocks) {
        if (block.is_custom) {
            deleteElement(static_cast<char*>(block.ptr));
        } else {
            delete[] static_cast<char*>(block.ptr);
        }
    }

    std::cout << "=== ????????? ===\n"
              << "?????????: " << test_cycles << "\n"
              << "?????????: " << blocks.capacity() << "\n"
              << "????й?????: " << blocks.size() << "\n";
}

void mem_time_test() {
	MemoryManager::get_instance().startUp();
	ThreadPool::get_instance().startUp();

	CounterManager::get_instance().startUp();
	StopWatchManager::get_instance().startUp();

	StopWatch sw;
	sw.Start();
	SimpleMemoryTest();
	sw.Pause();
	std::cout << "mem_test cost: " << sw.microseconds() << " microseconds" << std::endl;

	StopWatchManager::get_instance().shutdown();
	CounterManager::get_instance().shutdown();


	ThreadPool::get_instance().shutdown();
	MemoryManager::get_instance().shutdown();
}

// void asyncIO_test() {
// 	auto res = ResourceManager::get_instance().async_load<json>(FileServer::get_rootPath()/"logs"/"DebugMemoryLog"/"MemoryManager_log.json");
// 	while (!res.is_done()) {
// 		std::cout << "?????????ɡ?????"<<endl;
// 	}
// 	auto js = res.get();
// 	cout << js.dump(4) << endl;
//
// }


#include "Function/Framework/Object/Object.h"
using namespace TinyRenderer;
void Example::Main() {
	// MemoryManager::get_instance().startUp();
	// GameObject* go = newElement(GameObject);
	// deleteElement(go);
	// MemoryManager::get_instance().shutdown();
	// exit(0);
}
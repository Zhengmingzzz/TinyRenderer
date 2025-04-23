//
// Created by Administrator on 25-3-27.
//
// -- coding: utf-8 --
#pragma once
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include "Function/CommonType/json.h"
#include "TaskResult.h"
#include "Function/Message/Message.h"

namespace TinyRenderer {
    enum class TaskPriority : uint8_t {
        High        = 0,    // 最高优先级[5,7](@ref)
        MediumHigh  = 1,    // 中上优先级
        Medium      = 2,    // 中优先级
        MediumLow   = 3,    // 中下优先级
        Low         = 4     // 最低优先级
    };

    class ThreadPool {
    public:
        // 全局线程池单例（避免重复创建）
        static ThreadPool& instance();

        // 开启一个工作线程
        void launch_worker_thread();

        // 关闭一个空闲工作线程
        void close_idle_worker() ;

        void startUp(size_t thread_count = 2, size_t min_thread_cnt = 2, size_t max_thread_cnt = std::thread::hardware_concurrency());

        void shutDown();

        template<class F, class... Args>
        auto enqueue(F&& f, TaskPriority priority, Args&&... args) -> TaskResult<typename std::result_of<F(Args...)>::type> {
            using return_type = typename std::result_of<F(Args...)>::type;

            // 用智能指针管理packaged_task，避免生命周期问题
            auto task = std::make_shared<std::packaged_task<return_type(void)>>(
                // 返回一个可调用对象
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );


            // 创建TaskResult，获取future对象
            std::future<return_type> fut_res =  task->get_future();

            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                // 若当前线程已经停止，却再次调用入队
                if(threadpool_stop.load(std::memory_order_seq_cst)) {
                    LOG_ERROR("enqueue on stopped queue");
                }

                // 将任务封装为void()类型并加入队列
                tasks.emplace(Task{ [task]() { (*task)();}, priority });
            }

            // 唤醒一个等待线程[4,7](@ref)
            condition.notify_one();
            return fut_res;
        }

        // 由引擎主循环根据当前帧率调节线程数量，动态调整线程数
        void adjust_workers() ;

        ~ThreadPool()= default;

        struct Task {
            std::function<void()> func;
            TaskPriority priority;
        };
        // 定义任务比较器（优先级小的优先）
        struct TaskComparator {
            bool operator()(const Task& a, const Task& b) {
                return a.priority > b.priority; // 小顶堆逻辑
            }
        };

        struct ThreadBlock {
            std::thread thread;
            std::atomic<bool> stop{false};
            std::atomic<bool> isWorking{false};

            explicit  ThreadBlock()= default;

            explicit ThreadBlock(std::function<void()> func) {
                thread = std::thread([func]() {
                    func();
                });
            };

            void shutDown() {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        };

    private:
        // 线程的最大/小数量
        static size_t max_thread_count;
        static size_t min_thread_count;
        // 工作线程列表，使用智能指针保证其生命周期
        std::list<std::shared_ptr<ThreadBlock>> workers;
        // 优先队列，工作线程会自动从中取任务
        std::priority_queue<Task, std::vector<Task>, TaskComparator> tasks;
        // 队列锁，操作工作/任务队列时需要上锁 以免读取到脏数据
        // 读取完应该尽快释放，任务中的条件变量依靠这个锁才能取任务
        std::mutex queue_mutex;
        // 实例化锁，防止多线程多次实例化单例对象
        static std::mutex instance_mutex;
        std::condition_variable condition;
        // 停止标识符
        std::atomic<bool> threadpool_stop{false};
    };
}
//
// Created by Administrator on 25-4-4.
//
#include "ThreadPool.h"

namespace TinyRenderer {
    size_t ThreadPool::max_thread_count;
    size_t ThreadPool::min_thread_count;
    std::mutex ThreadPool::instance_mutex;

    ThreadPool& ThreadPool::instance() {
        static ThreadPool* pool = nullptr;
        // 双重锁定
        if (pool == nullptr) {
            // 只允许一个线程进入 创建实例
            std::lock_guard lock(instance_mutex);
            if (pool == nullptr) {
                pool = new ThreadPool();
            }
        }

        return *pool;
    }

    void ThreadPool::launch_worker_thread() {
        // 使用智能指针管理新线程块的生命周期
        std::shared_ptr<ThreadBlock> thread_block = std::make_shared<ThreadBlock>();

        auto lambda = [this, weak_block = std::weak_ptr(thread_block)]() {
            // 工作线程执行流程
            while (true) {
                // 创建一个空任务，等待适当条件后获取任务并执行
                Task task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);

                    // 如果 block已经被删除||停止线程||任务队列不为空 ，则继续执行
                    auto predicate = [this, weak_block] {
                        // 先判断当前弱指针是否有效
                        if (auto block = weak_block.lock()) {
                            return threadpool_stop.load(std::memory_order_seq_cst) ||
                               block->stop.load(std::memory_order_acquire) ||
                               !tasks.empty();
                        }
                        LOG_ERROR("the thread_block is delete before thread return");
                        // 如果无效则直接退出条件变量
                        return true;
                    };
                    condition.wait(lock, predicate);

                    // 无任务且要求停止，则停止线程
                    if (threadpool_stop.load(std::memory_order_seq_cst) && tasks.empty()) {
                        return;
                    }
                    // 如果当前对象已经被删除||要求关闭当前线程，则停止
                    if (auto block = weak_block.lock(); !block || block->stop.load(std::memory_order_acquire))
                        return;

                    // 从队列拿出任务
                    task.func = std::move(tasks.top().func);
                    tasks.pop();
                }
                // 获取资源所有权，避免突然被删除资源
                if (auto temp_ptr = weak_block.lock()) {
                    weak_block.lock()->isWorking.store(true, std::memory_order_release);
                    // 执行任务
                    try {
                        task.func();
                    }
                    catch(...) {
                        LOG_ERROR("a task throw error");
                    }
                    weak_block.lock()->isWorking.store(false, std::memory_order_release);
                }
                else {
                    LOG_ERROR("the thread_block is delete before preform task");
                }
            }
        };

        thread_block->thread = std::move(std::thread(std::move(
            [lambda]() {
                lambda();
            })));
        {
            std::lock_guard lock(instance_mutex);
            workers.push_back(std::move(thread_block));
        }
    }

    void ThreadPool::close_idle_worker() {
        // 遍历所有线程
        for (auto it = workers.begin(); it != workers.end(); ++it) {
            std::shared_ptr<ThreadBlock> block = *it;
            // 查看是否有空闲的
            if (block->isWorking.load(std::memory_order_acquire) == false) {
                // 设置空闲限制停止执行标识
                block->stop.store(true, std::memory_order_release);
                condition.notify_all();

                // 调用TextureBlock关闭函数
                block->shutDown();
                {
                    std::lock_guard lock(instance_mutex);
                    workers.erase(it);
                }
                break;
            }
        }
    }

    void ThreadPool::startUp(size_t thread_count, size_t min_thread_cnt, size_t max_thread_cnt) {
        thread_count = std::min(thread_count, max_thread_cnt);
        min_thread_count = std::min(min_thread_cnt, (size_t)0);

        this->max_thread_count = max_thread_cnt;
        this->min_thread_count = min_thread_cnt;
        for (size_t i = 0; i < thread_count; ++i)
        {
            launch_worker_thread();
        }
    }

    void ThreadPool::shutDown(){
        threadpool_stop.store(true, std::memory_order_seq_cst);

        condition.notify_all();

        for (auto& worker : workers) {
            // 等待所有线程执行完
            worker->shutDown();
        }
        workers.clear();

        delete &instance();
    }

    void ThreadPool::adjust_workers() {
        size_t task_count=0;
        size_t worker_count=0;
        {
            // 调整线程数量时上锁，防止读取脏数据
            std::lock_guard<std::mutex> lock(queue_mutex);
            task_count = tasks.size(); // 需要加锁获取
            // 线程数 = 主线程 + 其他线程数
            worker_count = workers.size() + 1;
        }

        // 如果需要的任务数量大于工作线程数
        if (task_count > worker_count * 2 && worker_count < max_thread_count)
            launch_worker_thread();
        // 如果工作线程数量大于任务数量 并且 工作线程数量大于最小的线程数量
        else if (worker_count > task_count && worker_count > min_thread_count)
            close_idle_worker();
    }



}
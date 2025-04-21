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
        // ˫������
        if (pool == nullptr) {
            // ֻ����һ���߳̽��� ����ʵ��
            std::lock_guard lock(instance_mutex);
            if (pool == nullptr) {
                pool = new ThreadPool();
            }
        }

        return *pool;
    }

    void ThreadPool::launch_worker_thread() {
        // ʹ������ָ��������߳̿����������
        std::shared_ptr<ThreadBlock> thread_block = std::make_shared<ThreadBlock>();

        auto lambda = [this, weak_block = std::weak_ptr(thread_block)]() {
            // �����߳�ִ������
            while (true) {
                // ����һ�������񣬵ȴ��ʵ��������ȡ����ִ��
                Task task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);

                    // ��� block�Ѿ���ɾ��||ֹͣ�߳�||������в�Ϊ�� �������ִ��
                    auto predicate = [this, weak_block] {
                        // ���жϵ�ǰ��ָ���Ƿ���Ч
                        if (auto block = weak_block.lock()) {
                            return threadpool_stop.load(std::memory_order_seq_cst) ||
                               block->stop.load(std::memory_order_acquire) ||
                               !tasks.empty();
                        }
                        LOG_ERROR("the thread_block is delete before thread return");
                        // �����Ч��ֱ���˳���������
                        return true;
                    };
                    condition.wait(lock, predicate);

                    // ��������Ҫ��ֹͣ����ֹͣ�߳�
                    if (threadpool_stop.load(std::memory_order_seq_cst) && tasks.empty()) {
                        return;
                    }
                    // �����ǰ�����Ѿ���ɾ��||Ҫ��رյ�ǰ�̣߳���ֹͣ
                    if (auto block = weak_block.lock(); !block || block->stop.load(std::memory_order_acquire))
                        return;

                    // �Ӷ����ó�����
                    task.func = std::move(tasks.top().func);
                    tasks.pop();
                }
                // ��ȡ��Դ����Ȩ������ͻȻ��ɾ����Դ
                if (auto temp_ptr = weak_block.lock()) {
                    weak_block.lock()->isWorking.store(true, std::memory_order_release);
                    // ִ������
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
        // ���������߳�
        for (auto it = workers.begin(); it != workers.end(); ++it) {
            std::shared_ptr<ThreadBlock> block = *it;
            // �鿴�Ƿ��п��е�
            if (block->isWorking.load(std::memory_order_acquire) == false) {
                // ���ÿ�������ִֹͣ�б�ʶ
                block->stop.store(true, std::memory_order_release);
                condition.notify_all();

                // ����TextureBlock�رպ���
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
            // �ȴ������߳�ִ����
            worker->shutDown();
        }
        workers.clear();

        delete &instance();
    }

    void ThreadPool::adjust_workers() {
        size_t task_count=0;
        size_t worker_count=0;
        {
            // �����߳�����ʱ��������ֹ��ȡ������
            std::lock_guard<std::mutex> lock(queue_mutex);
            task_count = tasks.size(); // ��Ҫ������ȡ
            // �߳��� = ���߳� + �����߳���
            worker_count = workers.size() + 1;
        }

        // �����Ҫ�������������ڹ����߳���
        if (task_count > worker_count * 2 && worker_count < max_thread_count)
            launch_worker_thread();
        // ��������߳����������������� ���� �����߳�����������С���߳�����
        else if (worker_count > task_count && worker_count > min_thread_count)
            close_idle_worker();
    }



}
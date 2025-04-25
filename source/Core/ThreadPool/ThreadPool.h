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
        High        = 0,    // ������ȼ�[5,7](@ref)
        MediumHigh  = 1,    // �������ȼ�
        Medium      = 2,    // �����ȼ�
        MediumLow   = 3,    // �������ȼ�
        Low         = 4     // ������ȼ�
    };

    class ThreadPool {
    public:
        // ȫ���̳߳ص����������ظ�������
        static ThreadPool& instance();

        // ����һ�������߳�
        void launch_worker_thread();

        // �ر�һ�����й����߳�
        void close_idle_worker() ;

        void startUp(size_t thread_count = 2, size_t min_thread_cnt = 2, size_t max_thread_cnt = std::thread::hardware_concurrency());

        void shutDown();

        template<class F, class... Args>
        auto enqueue(F&& f, TaskPriority priority, Args&&... args) -> TaskResult<typename std::result_of<F(Args...)>::type> {
            using return_type = typename std::result_of<F(Args...)>::type;

            // ������ָ�����packaged_task������������������
            auto task = std::make_shared<std::packaged_task<return_type(void)>>(
                // ����һ���ɵ��ö���
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );


            // ����TaskResult����ȡfuture����
            std::future<return_type> fut_res =  task->get_future();

            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                // ����ǰ�߳��Ѿ�ֹͣ��ȴ�ٴε������
                if(threadpool_stop.load(std::memory_order_seq_cst)) {
                    LOG_ERROR("enqueue on stopped queue");
                }

                // �������װΪvoid()���Ͳ��������
                tasks.emplace(Task{ [task]() { (*task)();}, priority });
            }

            // ����һ���ȴ��߳�[4,7](@ref)
            condition.notify_one();
            return fut_res;
        }

        // ��������ѭ�����ݵ�ǰ֡�ʵ����߳���������̬�����߳���
        void adjust_workers() ;

        ~ThreadPool()= default;

        struct Task {
            std::function<void()> func;
            TaskPriority priority;
        };
        // ��������Ƚ��������ȼ�С�����ȣ�
        struct TaskComparator {
            bool operator()(const Task& a, const Task& b) {
                return a.priority > b.priority; // С�����߼�
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
        // �̵߳����/С����
        static size_t max_thread_count;
        static size_t min_thread_count;
        // �����߳��б�ʹ������ָ�뱣֤����������
        std::list<std::shared_ptr<ThreadBlock>> workers;
        // ���ȶ��У������̻߳��Զ�����ȡ����
        std::priority_queue<Task, std::vector<Task>, TaskComparator> tasks;
        // ����������������/�������ʱ��Ҫ���� �����ȡ��������
        // ��ȡ��Ӧ�þ����ͷţ������е����������������������ȡ����
        std::mutex queue_mutex;
        // ʵ����������ֹ���̶߳��ʵ������������
        static std::mutex instance_mutex;
        std::condition_variable condition;
        // ֹͣ��ʶ��
        std::atomic<bool> threadpool_stop{false};
    };
}
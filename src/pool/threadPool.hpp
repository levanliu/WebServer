#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include <iostream>
#include <future>


class ThreadPool
{
public:
    using Task = std::function<void()>;
    // the constructor function only be used for direct initialization
    explicit ThreadPool(std::size_t numThreads)
        : mStop(false), mNumThreads(numThreads)
    {
        for (std::size_t i = 0; i < numThreads; i++)
        {
            mWorkers.emplace_back([this]
                                  {
                while (true)
                {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{ mQueueMutex };
                        //释放锁，并将调用线程置于休眠状态，直到满足以下条件之一  (mStop变为true或者任务容器mTasks不为空)
                        //在满足条件之后，并且收到notify_one()或者notify_all()将会重新去获得锁。
                        mCondVar.wait(lock, [this] { return mStop || !mTasks.empty(); });
                        if (mStop && mTasks.empty())
                            return;
                        task = std::move(mTasks.front());
                        mTasks.pop();
                    }
                    task();
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                } });
        }

        // Start the manager thread
        mManager = std::thread([this]
                               {
            while (!mStop)
            {
                std::unique_lock<std::mutex> lock{ mQueueMutex };
                //释放锁，并将调用线程置于休眠状态 1 秒，获得锁
                mCondVar.wait_for(lock, std::chrono::seconds(1));
                if (mStop)
                    return;

                auto tasksSize = mTasks.size();
                auto workersSize = mWorkers.size();
                auto threadsRatio = tasksSize / (workersSize + 1);

                // check if the number of tasks is greater than
                // the number of workers and if the task-to-worker ratio is greater than 2
                if (tasksSize > workersSize && threadsRatio > 2)
                {
                    std::cout << "Adding new worker thread\n";
                    mWorkers.emplace_back([this] {
                        while (true)
                        {
                            Task task;
                            {
                                std::unique_lock<std::mutex> lock{ mQueueMutex };
                                mCondVar.wait(lock, [this] { return mStop || !mTasks.empty(); });
                                if (mStop && mTasks.empty())
                                    return;
                                task = std::move(mTasks.front());
                                mTasks.pop();
                            }
                            task();
                        }
                    });
                }
                // check if the number of tasks is less than half the number of workers
                // and if the number of workers is greater than the initial number of threads
                else if (tasksSize < workersSize / 2 && workersSize > mNumThreads)
                {
                    std::cout << "Removing worker thread\n";
                    // Here we detach the thread so that it continues executing on its own
                    // and can be cleaned up by the operating system
                    mWorkers.back().detach();
                    mWorkers.pop_back();
                }
            } });
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock{mQueueMutex};
            mStop = true;
        }
        // Notify all threads and the manager thread to stop
        mCondVar.notify_all();
        // Wait for manager thread to finish
        mManager.join();
        // Wait for all worker threads to finish
        for (auto &worker : mWorkers)
            if (worker.joinable())
                worker.join();
    }

    /** F &&f, Args &&...args work for perfect forwarding
     */
    template <typename F, typename... Args>
    auto enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        // typedef for the return type of the function or function object F when it is called with the arguments Args....
        using return_type = typename std::result_of<F(Args...)>::type;
        // creates a std::shared_ptr to a std::packaged_task that wraps a callable object f and its arguments args....
        // wraps a callable object (such as a function, function pointer, or function object) and allows you to invoke it later and obtain its result through a std::future.
        // The template parameter return_type() represents the return type of the callable object.
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock{mQueueMutex};
            if (mStop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            //*task is dereferences
            // non-copyable or non-movable element.
            mTasks.emplace([task]()
                           { (*task)(); });
        }
        // Notify one of the worker threads that a new task is available
        mCondVar.notify_one();
        return res;
    }

private:
    std::vector<std::thread> mWorkers; // workers container.
    std::queue<Task> mTasks;           // tasks queue
    std::thread mManager;              // manager thread
    std::mutex mQueueMutex;            // queue mutex
    std::condition_variable mCondVar;  // condition variable
    bool mStop;
    std::size_t mNumThreads; // the number of elements in a container
};

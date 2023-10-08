#ifndef __THREAD_H__
#define __THREAD_H__

#include <memory>
#include <thread>
#include <cstring>
#include <functional>
#include <vector>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <unordered_map>
#include <queue>
#include <semaphore.h>
#include <atomic>
#include <mutex>

namespace hps {

class Semaphore {
public:
  using ptr = std::shared_ptr<Semaphore>;
  
  Semaphore(int count = 1);
  ~Semaphore();
  void wait();
  void notify();

private:
  Semaphore(const Semaphore&) = delete;
  Semaphore(const Semaphore&&) = delete;
  Semaphore& operator=(const Semaphore&) = delete;
private:
  sem_t m_semaphore;
};

template<class T>
class MutexLockImpl {
public:
MutexLockImpl(T& mutex):m_mutex(mutex) {
  lock();
  isLock = true;
}

~MutexLockImpl() {
  unlock();
}

void lock() {
  if (!isLock) {
    m_mutex.lock();
    isLock = true; 
  }
}

void unlock() {
  if (isLock) {
    m_mutex.unlock();
    isLock = false;
  }
}

private:
  T& m_mutex;
  bool isLock = false;
};

class MutexLock {
public:
  typedef MutexLockImpl<MutexLock> Lock;
  MutexLock() {
    pthread_mutex_init(&m_mutex, nullptr);
  }

  void lock() {
    pthread_mutex_lock(&m_mutex);
  }

  void unlock() {
    pthread_mutex_unlock(&m_mutex);
  }

  ~MutexLock() {
    pthread_mutex_destroy(&m_mutex);
  }
private:
  pthread_mutex_t m_mutex;
};

template<class T>
class WriteLockImpl {
  typedef WriteLockImpl WriteLock;
public:
  WriteLockImpl() {
    m_mutex.rdlock();
    isLock = true;
  }

  ~WriteLockImpl() {
    unlock();
    isLock = false;
  }

  void lock() {
    if (!isLock) {
      m_mutex.rdlock();
    }
    isLock = true;
  }

  void unlock() {
    if (!isLock) {
      m_mutex.unlock();
    }
    isLock = false;
  }

private:
  T& m_mutex;
  bool isLock = false;
};

template<class T>
class ReadLockImpl {
  typedef ReadLockImpl ReadLock;
public:
  ReadLockImpl() {
    m_mutex.rdlock();
    isLock = true;
  }

  ~ReadLockImpl() {
    unlock();
    isLock = false;
  }

  void lock() {
    if (!isLock) {
      m_mutex.rdlock();
    }
    isLock = true;
  }

  void unlock() {
    if (!isLock) {
      m_mutex.unlock();
    }
    isLock = false;
  }

private:
  T& m_mutex;
  bool isLock = false;
};

class ReadWriteMutex {
public:
  ReadWriteMutex() {
    pthread_rwlock_init(&m_mutex, nullptr);
  }
  
  ~ReadWriteMutex() {
    pthread_rwlock_destroy(&m_mutex);
  }
  
  void rdlock() {
    pthread_rwlock_rdlock(&m_mutex);
  }

  void wrlock() {
    pthread_rwlock_wrlock(&m_mutex); 
  }

  void unlock() {
    pthread_rwlock_unlock(&m_mutex);
  }
private:
  pthread_rwlock_t m_mutex;
};


class Thread {
public:
  using ptr = std::shared_ptr<std::thread>;
  using Func = std::function<void()> ;
  typedef MutexLock MutexType;

  explicit Thread(Func fc, std::string name = "");
  ~Thread();
  
  void start();
  void join();
  static size_t getNowId() {  
    return m_id ++; 
  }

private:
  static uint32_t m_id;
  uint32_t m_tId;  // 线程ID
  std::string m_name;   // 线程名
  ptr m_thread;         // 真正的线程存放
  Func m_fc;            // 执行的函数
};

class ThreadPool {
public:
  using TaskFunc = std::function<void()>;
  using ptr = std::shared_ptr<ThreadPool>;
  
  typedef MutexLock MutexType;

  // 将任务封装成一个块
  struct Task {
    TaskFunc tfc; // 具体执行的任务
    Task& operator=(Task& oth) {
      tfc = oth.tfc;
      return *this;
    }
  };

public:
  explicit ThreadPool(int threadNum = 1);// 线程数默认为1
  ~ThreadPool();
  
  void start();// 启动线程池
  void addThread(uint32_t id); // 往线程池里面添加新的线程
  void work(uint32_t id); // 往线程池里面添加新的线程
  void addTask(Task task); // 以task staruc的形式添加任务
  void addTask(TaskFunc fc); // 以lamada的形式增加任务
  bool changeThreadNum(); //修改当前线程池的大小, 如果当前的任务数量大于cpu核心数量的话直接使用核心数的最大值
private:
  // 线程池相关
  std::unordered_map<uint32_t, std::shared_ptr<Thread> > m_threads;
  int m_threadNum;

  // 任务队列相关
  int m_maxTaskNum;
  int m_taskNum = 0;
  std::queue<Task> m_TaskQue;

  Semaphore m_semaphore;
  MutexType m_mutex;
};

}
#endif
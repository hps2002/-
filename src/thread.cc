#include "thread.h"
#include <cassert>
#include <unistd.h>

namespace hps {

uint32_t Thread::m_id = 0;

static unsigned int cpuNum;

struct _INIT {
  _INIT() {
    cpuNum = sysconf(_SC_NPROCESSORS_ONLN);
  }
};

static _INIT init;

Semaphore::Semaphore(int count) {
  if (sem_init(&this -> m_semaphore, 0, count)) {
    throw std::logic_error("sem_init error");
  }
}

Semaphore::~Semaphore() {
  sem_destroy(&this -> m_semaphore);
}

void Semaphore::wait() {
  if (sem_wait(&m_semaphore)) {
    throw std::logic_error("sem_wait error");
  }
}
void Semaphore::notify() {
  if (sem_post(&m_semaphore)) {
    throw std::logic_error("sem_post error");
  }
}

Thread::Thread(Func fc, std::string name):m_name(name), m_fc(std::move(fc))
{
  // std::cout << "Thread:" << m_id << ", " << "name:" << m_name << std::endl;
}

Thread::~Thread() {
  m_thread.reset();
  // std::cout << "~Thread:" << m_id << ", " << "name:" << m_name << std::endl;
}

// 启动线程
void Thread::start() {
  this -> m_thread = std::make_shared<std::thread>(this -> m_fc);
  // this -> m_thread -> detach(); // 脱离成为单独的线程
}

void Thread::join() {
  this -> m_thread -> join();
}

ThreadPool::ThreadPool(int threadNum):m_threadNum(threadNum){
}

ThreadPool::~ThreadPool() {
  for (auto& thr : this -> m_threads) {
    thr.second -> join();
  }
}


void ThreadPool::start() {
  for (int i = 0; i < this -> m_threadNum; i ++) {
    this -> addThread(Thread::getNowId());
  }
}

void ThreadPool::addThread(uint32_t id)
{
  this -> m_threads[id] = (std::make_shared<Thread>([this, id]{
    work(id);
  }, "No:" + std::to_string(id)));
  this -> m_threads[id] -> start();
  this -> m_semaphore.wait();
}

void ThreadPool::work(uint32_t id) {
  this -> m_semaphore.notify();
  while (true) {
    ThreadPool::Task task;
    this -> changeThreadNum();
    // 等待任务出现
    while (this -> m_taskNum <= 0) {
      if (this -> m_taskNum) {
        break;
      }
    }

    {
      hps::MutexLock lock();
      std::cout << "Thread: " << id << ", running task:";
      task = m_TaskQue.front();
      m_TaskQue.pop();
      this -> m_taskNum --;
      task.tfc();
    }
  }
}

void ThreadPool::addTask(Task task) {
  this -> m_TaskQue.push(task);
  this -> m_taskNum ++;
}

bool ThreadPool::changeThreadNum() {
  if (m_threads.size() >= cpuNum) return true;
  if (m_TaskQue.size() >= cpuNum) {
      {
        try {
          hps::MutexLock lock();
          this -> addThread(Thread::getNowId());
        } catch(std::exception& e) {
          std::cout << "Error: changeThreadNum has faild";
          return false;
        }
      }
  }
  return true;
}

}
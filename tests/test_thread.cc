#include "../src/thread.h"
#include <iostream>
#include <unistd.h>
#include <cassert>
#include <sys/wait.h>

void TestFunc() {
  std::cout << "Test begin." << std::endl;
  int cnt = 0;
  for (int i = 0; i < 5000000; i ++) {
    cnt ++;
  }
  std::cout << "cnt: " << cnt << std::endl;
  std::cout << "Test end!" << std::endl;
}

void baseLine() {
  std::cout << "BaseLine begin." << std::endl;
  hps::ThreadPool pool(5); // 创建线程数量，建议是根据自己机器的核心数设置
  hps::ThreadPool::Task task; // 每条线程中需要执行的任务都是一个闭包。
	pool.start();

  // 往线程池里面添加任务
  int taskNum = 10;
  for (int i = 0; i < taskNum; i ++) {
    task.tfc = TestFunc;
    pool.addTask(task);
  }
  std::cout << "BaseLine End." << std::endl;
}

int num = 0;

void test_lock_func() {
	std::cout << "In test_lock_func" << std::endl;
	for (int i = 0; i < 500000; i ++) {
    num ++;
  }
}

void test_Lock() {
  hps::ThreadPool p(2);	
  p.start();
  hps::ThreadPool::Task task;
  task.tfc = test_lock_func;
  p.addTask([](){
    std::cout << "Hello ThreadPool!" << std::endl;
  });
  p.addTask(task);
  sleep(2);
	std::cout << num << std::endl;
  sleep(3);
  p.addTask([](){
    std::cout << "test cpu" << std::endl;
  });
}

int main() {
  // baseLine();
  test_Lock();
  return 0;
}

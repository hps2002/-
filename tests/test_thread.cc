#include "../src/thread.h"
#include <iostream>
#include <unistd.h>
#include <cassert>

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
  pool.start(); // 启动线程池

  // 往线程池里面添加任务
  int taskNum = 10;
  for (int i = 0; i < taskNum; i ++) {
    task.tfc = TestFunc;
    pool.addTask(task);
  }
  std::cout << "BaseLine End." << std::endl;
}

int main() {
  baseLine();
  return 0;
}
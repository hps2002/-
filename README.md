# 项目描述
实现了一个简单线程池库，可以自定义的控制线程数，自定义添加任务。

# 具体工作
使用哈希表存放线程，key = 线程id（不是线程号），value = 指向线程的指针。线程池的具体实现：封装了一个线程类，线程池中支持手动的增加和减小线程数量。每个线程获取任务的方式：外部通过 `addTask` 的方式提交任务，线程通过互斥量占有任务的方式从队列中拿出任务。

任务存在形式：每个任务可以看作是一个闭包，通过Task块进行传递任务，在线程中执行。

# 接下来的工作
1、增加线程数量增长机制，根据当前的任务队列数量增加线程数量（以硬件核心数决定）
2、泛化任务提交的模式，重载外部出以 `lamada` 或者 `struct` 任意一种形式提交任务。
3、优化线程在处理任务的时候来的不必要性能开销。

# 使用方法
方法一：将 `.h` 文件和 `.cc` 文件引入项目中，与项目一起进行编译

方法二：通过 `cmake` 等项目管理工具单独进行编译然后链接到项目中

作者在 `CMakeList` 中写的是 `tests` 的的编译。您完全可以忽略其他文件直接使用 `src` 里面的代码，直接进行编译或者移植到您的项目中

# 示例代码
```c++
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
```
## 样例运行结果
![](/doc/result.png)

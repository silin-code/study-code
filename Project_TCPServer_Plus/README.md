<!--文件层级
Project_TCPServer_PLUS/
├── CMakeLists.txt       # 全局编译脚本，一键构建
├── config/
│   └── Config.hpp         # 【唯一切换文件】POLL/EPOLL 宏定义//因为我把Poll,Select,EPoll一起写了
├── src/
│   ├── log/             # 日志模块｜独立解耦、线程安全、分级输出
│   │   ├── LogLevel.hpp   # 日志级别枚举定义
│   │   ├── Logger.hpp     # 日志单例类 声明
│   │   └── Logger.cpp   # 日志具体实现
│   ├── net/             # 网络基础层｜原生Linux Socket 全套封装
│   │   ├── TcpSocket.hpp  # 封装socket/bind/listen/accept/recv/send 系统调用
│   │   └── TcpSocket.cpp
│   ├── reactor/         # 多路复用核心层｜IO模型抽象 + 具体实现
│   │   ├── EventDef.hpp   # 事件宏：读事件/写事件/异常事件
│   │   ├── EventLoop.hpp  # 多路复用抽象基类（统一接口）
│   │   ├── PollLoop.hpp   # Poll 模型 子类实现
│   │   ├── EpollLoop.hpp  # Epoll 模型 子类实现
│   │   └── LoopFactory.hpp # 工厂类：根据配置自动生成 Poll/Epoll 对象
│   ├── connection/      # 客户端连接管理层｜单个连接封装
│   │   ├── Connection.hpp # 封装客户端fd、读写缓冲区、事件回调
│   │   └── Connection.cpp
│   ├── thread/          # 线程模块｜温习 std::thread 用法
│   │   ├── Thread.hpp
│   │   └── Thread.cpp
│   ├── utils/           # 通用工具｜错误处理、类型转换、公共函数
│   │   ├── Utils.hpp
│   │   └── Utils.cpp
│   └── main.cpp         # 程序入口｜启动服务、初始化模块、主事件循环
└── README.md            # 编译命令、运行教程、知识点复盘、切换模型教程 -->

<!-- TCP服务器的过程
socket() 创建监听fd 
→ setsockopt() 端口复用
→ bind() 绑定IP+端口
→ listen() 开启监听
→ 多路复用监听 监听fd读事件
→ 监听到事件 → accept() 生成客户端fd
→ 客户端fd设为非阻塞
→ 注册客户端读事件到多路复用
→ 客户端发数据 → recv() 读取
→ 业务处理 → send() 响应
→ 客户端断开 → close释放fd + 容器删除连接 -->

<!-- 项目的Linux 核心系统调用（逐个精讲）
基础 Socket：socket / bind / listen / accept / close
数据收发：recv / send
IO 控制：fcntl 非阻塞设置
网络配置：setsockopt / htons / inet_ntoa
多路复用：
poll() 结构体、事件掩码、超时
epoll_create() / epoll_ctl() / epoll_wait() 三大核心函数 -->

<!-- 项目C++STL库的装载内容
容器	           底层结构	      本项目存储内容	               核心作用
std::vector	      连续动态数组	  存储 pollfd 数组、事件集合	顺序存储、快速遍历
std::unordered_map	  哈希表	  key: int(客户端fd)  value: Connection*	高速增删查客户端连接 O (1)
std::map	      红黑树 (平衡二叉树)	备用有序映射演示	有序键值存储 O (logn)
std::function	  可调用对象包装器	存储读写事件回调函数	解耦事件与业务逻辑 -->

<!-- 模块依赖关系（低耦合，易维护）
全局配置 Config.h → 控制所有多路复用模块
日志模块 → 被所有模块依赖，统一错误 / 运行日志输出
Socket 基础层 → 为连接层、反应堆提供网络能力
反应堆多路复用 → 核心调度，监听所有文件描述符事件
连接管理层 → 管理每个客户端生命周期
主线程 + 事件循环 → 服务整体运行驱动 -->
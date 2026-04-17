<!-- -- http-server-cpp/        # 项目根目录
├── include/            # 后端头文件
│   └── HttpServer.h    # 服务器类声明
├── src/                # 后端C++源码
│   ├── HttpServer.cpp  # 你已写的代码 + 补全分离逻辑
│   └── main.cpp        # 程序入口
├── web/                # 🎯 前端独立文件夹（前后端分离核心）
│   └── index.html      # 前端页面（独立编写，C++不硬编码）
└── Makefile            # 一键编译 
    README.md           # 说明文档 -->


<!--新增Cookie和Session模块
 http-server-cpp/
├── include/
│   ├── HttpServer.h      // 你原来的文件，我帮你改几行
│   ├── CookieUtil.h      // 新增：Cookie 工具类
│   └── SessionManager.h  // 新增：Session 管理器
├── src/
│   ├── HttpServer.cpp    // 你原来的文件，核心修改 handleClient 函数
│   ├── main.cpp          // 不变
│   ├── CookieUtil.cpp    // 新增：Cookie 工具实现
│   └── SessionManager.cpp// 新增：Session 管理器实现
├── web/
│   ├── index.html        // 不变（也可以删掉，我们直接生成首页）
│   └── login.html        // 新增：登录页面
└── Makefile              // 不用改，原来的就可以编译所有文件 -->
# C++ 原生HTTP服务器
- 环境：Linux远端服务器
- 无第三方依赖，纯Socket实现
- 严格遵循HTTP 1.1协议格式
- 美化前端页面，显示Hello World + 完整请求信息
- 前后端分离
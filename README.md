# TinyRenderer
    使用C++实现的简单渲染器 用于个人学习
    目前有两个构建版本 Debug和Release
    使用的是Games104提到的5层架构设计
    欢迎各位前来互相探讨交流
## 分层架构设计
    1. 工具层 
    2. 功能层 实现游戏世界的动态逻辑与交互规则，使虚拟世界具备“可玩性”，包括渲染、动画、物理等模块
    3. 核心层 游戏引擎的底层基础设施，提供高性能、平台无关的通用工具和算法，支撑上层功能的稳定运行
    4. 资源层 对游戏开发中涉及的所有外部资源进行统一管理和优化，使其能被引擎高效使用
    5. 平台层 消除各个平台的差异

## 各个层的模块
#### 工具层
        
#### 功能层
1. 基于Object的引擎框架

#### 核心层
1. 基于池的内存管理器
2. 含有优先级的线程池 
#### 资源层
1. 资源管理器
2. GUID资源唯一标识符
#### 平台层
1. 配置管理类 存储引擎的各个路径，由平台无关的filesystem管理

# 0.0.0.1

- 提供IR API，提供构图功能
- 提供模型在线编译功能
- 提供模型加载、执行、卸载等模型管理功能



**约束**

1. ROM仅支持安装鸿蒙3及其以后版本的华为手机NPU平台
2. 支持特性列表：


    | 特性名称       | 支持 |
    | -------------- | ---- |
    | AIPP           | 是   |
    | 内存零拷贝      | 是   |
    | 量化           | 是[^1]   |
    | 可变Shape[^2]      | 否   |
    | 异构           | 否   |
    | FFTL           | 否   |


[^1]:权值量化不支持
[^2]:包括静态Shape和动态Shape
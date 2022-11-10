- [HUAWEI HiAI Foundation介绍](#HUAWEI HiAI Foundation介绍)
- [编译](#编译)
    - [编译环境配置](#编译环境配置)
    - [编译执行](#编译执行)
    - [编译输出](#编译输出)
- [APP集成](#APP集成)
- [版本说明](#版本说明)
- [许可证](#许可证)


## HUAWEI HiAI Foundation介绍


华为终端有强大的AI能力，为了让开发者能够更简单快捷的应用华为终端的AI能力，HUAWEI HiAI基于“芯、端、云”三层开放架构，构筑全面开放的智慧生态，让开发者能够快速地利用华为强大的AI处理能力，为用户提供更好的智慧应用体验。

HUAWEI HiAI Foundation作为芯片使能的基础平台，提供了300+算子的支持，具备业界最佳的模型兼容性，为众多业务场景提供了更高性能更低功耗的计算环境。

![img](https://alliance-communityfile-drcn.dbankcdn.com/FileServer/getFile/cmtyPub/011/111/111/0000000000011111111.20210708144053.05364942099834414782288116808750:50520707094033:2800:E71B980B17A91C503882D5D8A2E8AE307EAFAF0F17176182128D36A683C8FEE6.png?needInitFileName=true?needInitFileName=true) 

HUAWEI HiAI Foundation衔接智慧业务和计算芯片，提供轻量化模型计算能力，支持华为MindSpore、TensorFlow、Caffe、Paddle、ONNX、AndroidNN等框架的对接，支持芯片内多计算单元的异构计算，为开发者提供模型量化，模型转换，性能调优，维测等高效工具链。

HiAI DDK开发套件将其AI推理能力封装在动态库文件里，通过此方式集成在用户app中，因此HiAI DDK支持跨平台交叉编译，目前支持情况如下所示：

| 硬件平台   |                                                              | 状态 |
| :--------- | :----------------------------------------------------------- | :--- |
| Kirin 平台 | Kirin 810 、Kirin 820、Kirin 970、Kirin 980 、Kirin 985 、<br />Kirin 990 、Kirin 990 5G 、Kirin 990E 、Kirin 9000E 、Kirin 9000 | ✔️    |

（[硬件平台和手机型号对应关系表](./doc/MappingHardWarePlatformAndPhoneModel.md)）

## 编译

#### 编译环境配置

编译所需的第三方工具和对应的推荐版本如下：

| 编译工具      | 推荐版本 | 下载链接                                                     |
| -------------- | -------- | ------------------------------------------------------------ |
| protobuf       | 3.9.0    | https://github.com/protocolbuffers/protobuf/releases/download/v3.9.0/protobuf-cpp-3.9.0.tar.gz |
| libboundscheck | 1.1.11   | https://github.com/openeuler-mirror/libboundscheck/archive/refs/tags/v1.1.11.zip |
| android-ndk    | r20b     | https://dl.google.com/android/repository/android-ndk-r20b-linux-x86_64.zip |
| cmake          | 3.21.4   | https://github.com/Kitware/CMake/releases/download/v3.21.4/cmake-3.21.4-linux-x86_64.tar.gz |
| ninja          | 1.10.2   | https://github.com/ninja-build/ninja/releases/download/v1.10.2/ninja-linux.zip |

只支持在Linux平台Ubuntu 16/18操作系统下进行编译，暂不支持在windows下编译。


HiAI DDK编译依赖第三方库，编译过程中需要下载所需的第三方软件包，请确保可连接网络，脚本会下载所需的工具，下载耗时因网络环境而异。

下载过程中会在`HiAIFoundation`下新建一个`buildtools`目录存放下载文件和解压后文件。

本项目支持如下两种方式编译：

1、本地无编译环境

​	如果本地是一个全新的环境，没有所需的编译工具，可以不修改配置文件，直接跳到“编译执行”小节，执行命令即可。


2、通过配置文件(build.conf)进行自定义配置

​	如果本地已经下载好编译工具，需要在配置文件`build.conf`对应的工具名称后面正确填写对应的工具路径，具体配置项如下所示：
 （注意：配置文件中"="前的变量名请勿修改，编译脚本中会引用到，其中有一部分配置与平台相关，会在配置详情中标注）

   如果当前环境中只有部分编译工具，可以只填写当前已有的编译工具路径，剩余的配置路径置为空，我们在`build.py`脚本中会自动检测并下载。


1. 配置`NDK`路径（下载文件较大，建议使用本地配置）例如：
   ```
   # 使用自定义本地配置
   ANDROID_NDK_PATH    =/your/project/path/HiAIFoundation/buildtools/android-ndk-r20b
   # 使用编译脚本提供的下载配置
   ANDROID_NDK_PATH    =
   ```

2. 配置`cmake`路径，要填写到cmake二进制文件，例如：
   ```
   CMAKE_TOOLCHAIN_FILE=/your/project/path/HiAIFoundation/buildtools/cmake-3.21.4-linux-x86_64/bin/cmake
   ```

3. 配置`Ninja`路径，要填写到ninja二进制文件，例如：
   ```
   NINJA_PATH        =/your/project/path/HiAIFoundation/buildtools/ninja
   ```

4. 配置`ABI`，此选项用于区分最终的目标文件是基于32bit架构还是64bit架构，此字段只支持三种选项，如下所示

   ```
   ARCH:armeabi-v7a
        arm64-v8a
        both        #  compile both armeabi-v7a and arm64-v8a
   ```


build.py会自动下载编译依赖的第三方库，如果用户自定义配置，请务必使用目录名（protobuf、libboundscheck），并且放在和src同级目录的third_party下（如下图所示）。

![image-20211207201758113](doc/images/image-20211207201758113.png)

#### 编译执行

HiAI DDK基于NDK + CMake + Ninja的命令行构建方式，在确认配置文件未修改或者配置完成的前提下，安装好python(2.X与3.X皆可)，并同时安装python包管理工具(pip)，即可执行编译命令，另外本项目还提供测试代码和编译测试代码的一键式脚本，

测试代码路径：`HiAIFoundation/tests/`
编译ddk so和编译运行测试代码合一脚本：`build.py`

详细命令如下：

   ```
   pip install six==1.14.0          # 安装python依赖包命令

   python build.py                  # 编译so和编译运行测试代码默认均执行
   python build.py --only_ddk       # 只编译so，不编译运行测试代码
   ```


#### 编译输出

编译成功之后会有如下输出：

32位：

![image-20211207200801818](doc/images/image-20211207200801818.png)

64位：

![image-20211207200646536](doc/images/image-20211207200646536.png)

也可以通过查看和HiAIFoundation同级目录下的`hiai_ddk`下是否有对应架构的so生成，来确定是否编译成功。

编译运行测试代码成功之后会有如下输出：

![image-20211228160620651](doc/images/image-20211228160620651.png)

### APP集成

APP开发过程参考[HUAWEI HiAI Foundation  开发指南](https://developer.huawei.com/consumer/cn/doc/development/hiai-Guides/dev-process-0000001052965551)，当前开源部分仅支持NPU推理，开源软件编译出libhiai_ir.so，libhiai.so需要配合DDK版本包中libhiai_hcl_model_runtime.so使用。

## 版本说明

版本说明请参阅[RELEASE](RELEASE.md)。

## 许可证

[Apache License 2.0](LICENSE)


<!--
 * @Author: your name
 * @Date: 2020-08-01 20:04:24
 * @LastEditTime: 2020-08-03 23:22:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \qt_project\VideoWidget\README.md
-->
# 视频播放器控件

一个基于FFmpeg的视频解码的Qt控件，支持cuda、cpu、qsv解码。视频采用opengl离屏渲染技术

## 编译环境

* windows 64bit
* ffmpeg-4.3 可到tag1.0下载
  * Version 选择版本4.3
  * Architecture 选择64bit
  * Linking Dev下载一次，Shared和Static选一个下载；我的工程是使用的Shared
  * release-v1.0下面有放好的该版本压缩包
* nvidia独立显卡
  * 驱动版本436.15或以上[https://www.nvidia.cn/Download/index.aspx?lang=cn](https://www.nvidia.cn/Download/index.aspx?lang=cn)
* cpu
  * 建议i5及以上，带核显
  * `需要将显示器接到核显。本程序接到独显，qsv解码会失败。在任务管理器中windows系统1909能看到核显解码引擎;1709以上能看到gpu利用`
  * 下载IntelMedia-SDK[https://github.com/gitHubwhl562916378/IntelCudaVideoDecodDetect/releases/tag/v1.0](https://github.com/gitHubwhl562916378/IntelCudaVideoDecodDetect/releases/tag/v1.0)
  * 安装好后，需要将C:\Program Files (x86)\IntelSWTools\Intel(R) Media SDK 2020 R1\Software Development Kit\include里面`再建一个mfx目录，把所有头文件放进去`
* Qt
  * 低版本注意配置qopenglwidget，高版本不用配置

## 编译

* 使用QtCreator打开，选择64bit。我用的是vs-2017编译器

## 其它

### 提醒

>* tag1.0下面有编译好的可运行包
>* 在其他使用ffmpeg4.3项目测试多路cuda解码会在av_send_packet崩溃，此项目还未测试

### 更新

#### 2020/8/1

>* 实现qsv、cuda、cpu解码
>* 实现opengl离屏渲染，视频播放

#### 2020/8/2

>* 使用glTextureSubImage2D优化渲染性能

#### 2020/8/3

>* 完成从cuda直接将nv12渲染到opnegl
>* 实现nvidia官方解码插件式加载，并运行成功

#### 2020/8/4

>* 将插件整合到工程，另一个渲染插件也整合到工程
>* 解决图像不能随窗口缩放问题

#### 2020/8/6

>* 解决视频重播放，或者一个视频窗口多次播放，在第二次之后的播放，视频y分量无法正常渲染的问题
>* 解决播放视频结束后opengl资源与cuda资源释放问题
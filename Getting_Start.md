# EZTCP 快速上手

## 一、接口设计

将网络传输尽量简化，实现只用输入ip与端口就能实现通信的效果是本项目编写之初设立的目标之一。

在本库中，发送数据就像寄送快递包裹，想发送任何数据，首先将数据打包，然后委托快递公司将包裹发送到目的地，包裹会投递到接收方等待接收方主动收取。

就像拿快递一样，接收方得到包裹后，需要将包裹解包，就可以得到其中的数据，如同从另一台电脑上搬过来的一样。

## 二、具体步骤

使用本库进行网络连接有以下几个步骤：

1. A方创建Listener对象，等候连接
2. B方创建Session对象，创建即连接到A方
3. A方调用Listener::accept方法，获得与B通信的Session对象
4. 想要发送数据，B方（或A方）必须先打包本地的数据，创建Package对象
5. B方（或A方）调用之前创建的Session对象的Session::send方法，发送包裹
6. A方（或B方）调用Session::recv方法，获得包裹
7. A方（或B方）调用包裹的Package::extract方法，解开包裹，获得数据
8. 重复4~7步，Session对象析构时会自动断开连接

## 三、接口类

如一中所说，本库中需要两种用途的类：快递包裹和快递公司

充当快递公司的类：
> + **EZTCP::Listener** 使用这个类创建监听，等候连接
> + **EZTCP::Session** 会话类，通过会话进行数据包的收发

充当快递包裹的类：
> + **EZTCP::Package** ―― 所有包裹类的抽象基类，提供统一界面，任何继承此类的子类都可以通过Session发送出去。
> + **EZTCP::MemPack** ―― 把整块连续的内存包装，使之符合包的界面，以通过Session发送，注：此类只是简单地包装内存空间，不负责内存空间的释放和管理。
> + **EZTCP::BlockPack** ―― 在MemPack的基础上加增内存管理的功能。此类是本库提供的基础功能部分。除继承基类的extract方法外，可以通过unpack直接获取接收数据存放的内存空间的指针，提高效率。
> + **EZTCP::FilePack** ―― 拓展功能，打包文件使之符合包的界面，为文件传输设计，此类调用底层系统API直接发送文件，使用此类在方便之余还可以提高文件发送效率。
> + **EZTCP::RecvPack** ―― 将文件的接收抽象为包，具有包的界面，可以通过引用直接再通过Session::send发送，实现转发；也可以类型转换到BlockPack和FilePack，直接接收成连续内存或存放至文件。

注：仅使用**BlockPack**和**RecvPack**就可以满足库的基本功能，**MemPack**用于提高发送现成内存空间的效率，**FilePack**用于提高发送文件的效率。

## 四、错误与异常

本库中总共会抛出两种异常，均继承自std::exception，定义在接口头文件 EZTCP.h 中

> + EZTCP\::ERR::SocketError
>> + what函数：返回一个系统内部标识错误的int类型数值（不同系统相同数值含义不同，WINDOWS参阅WSAGetLastError,LINUX参阅errno，正考虑改进这点）
>> + 可能的抛出地点：
>>> 1. Session、Listener 类的构造函数
>>> 2. Session::recv、Session::send
>>> 3. RecvPack::recv 以及 RecvPack 向其他两种包的类型转换过程中

> + EZTCP\::ERR::FileIOError
>> + what函数：返回一个系统内部标识错误的int类型数值（不同系统相同数值含义不同，WINDOWS参阅GetLastError,LINUX参阅errno）
>> + 可能的抛出地点：
>>> + 所有和 FilePack 创建、发送、接收相关的函数

## 五、另注：

其他注释详见.h和.cpp文件

###### 2019.9.3 任何问题请联系：顾宇浩(yhgu2000@outlook.com)
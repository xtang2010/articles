这里是一些关于QNX的材料，也是笔者多年从事QNX相关工作的一些体会。

1.《从 API 开始理解 QNX》主要讲了QNX进行消息传递的基础，“频道”，“链接”，消息“发送”，“接收”与“回复”等，以及QNX是怎样通过这些基础元素实现客户-服务器系统的。

2.《优先级反转那点事儿》则从嵌入式系统的优先级反转，进而解释了在QNX上消息传递为何会引起优先级反转，QNX是怎样防止其发生的。这个对于理解QNX上线程的优先级，以及系统优化非常重要。

3.《路径名空间与搜索》介绍了QNX的路径名管理，详细解释了QNX是如何从路径空间中找到为用户路径服务的资源管理器的。

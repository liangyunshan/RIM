# RIM

### 20180423
- 完善历史会话页面item功能;

### 20180422
- 添加历史会话页面记录保存和读取本地sql；
- 添加会话页面置顶、取消置顶功能;
- 增加RUpdate类别名设置，为sqlite不支持update操作别名；

### 20180419
- 添加分组排序功能;
- 修复快速拖动分组时分组显示在页面底部;

### 20140418
- 修复好友请求时同时添加双方会出现重复添加;
- 修复删除分组时，未能将好友更新至默认分组;

### 20180417
- 增加用户列表分组显示在线、总人数；
- 修复删除好友后，再次搜索好友时添加好友按钮不可用;
- 更新自己的昵称，主面板未同步更新;

### 20180414
- 调整数据语句拼写，增加表连接操作;


### 20180411
- 修复用户名输入框信息显示空的问题；
- 调整协议中有关用户头像的使用；

### 20180323
- 调整聊天信息存储数据库连接信息；
- 支持头像上传、下载；

### 20180322
- 增加文件服务器保存文件路径，修改文件保存时使用MD5值代替传统文件名；
- 将系统配置信息按照登陆账户存储；

### 20180320
- 实现客户端下载文件;

### 20180315
- 初步实现客户端向服务器发送文件

### 20180308
- 修复多线程访问，因未加互斥锁产生的奔溃问题;

### 20180305
- 添加刷新联系人列表功能；
- 重构部分聊天窗口代码；

### 20180302
- 服务器增加命令行启动；
- 增加本地解析服务器的配置信息；
- 调整服务器网络接口；

### 20180301
- 修改面板在自动贴边隐藏配置改变后的处理；
- 添加面板是否自动贴边隐藏配置功能；
- 根据自动贴边隐藏配置修改自动贴边隐藏功能；
- 添加查看联系人资料功能；
- 添加修改联系人备注功能；
- 添加删除联系人功能；
- 修复更改联系人备注信息，用户状态显示错误问题；

### 20180227
- 修复多屏与单屏切换时主面板位置显示错误问题；
- 修复多屏下面板窗口停靠问题；


### 20180226
- 扩充消息发送类型，增加消息确认(以后可进一步进行消息重发)；
- 调整用户资料编辑中输入验证；
- 添加用户更新系统头像后同步更新到面板中；

### 20180224
- 添加用户状态变更操作；
- 添加推送用户好友在线状态；
- 添加ToolItem在线状态；

### 20180223
- 添加用户登陆后抓取离线系统消息、聊天信息；
- 添加对聊天信息的缓存；

### 20180210
- 添加主面板用户登录状态显示与操作；
- 修复拖动分组时主面板跟着移动问题；
- 添加ToolItem中置联系人头像为灰色方法；

### 20180207
- 调整用户登陆列表，可以查看头像和账户信息；
- 增加主面板直接编辑昵称信息；
- 修复主面板最小化不成功问题；
- 添加删除用户好友；

### 20180203
- 解决点击通知信息崩溃的问题;
- 解决首次打开对话框发送第一次信息后，接收到信息为空的问题；

### 20180202
- 点击右下角闪烁提示框显示最新接收到的信息;
- 修复删除分组pageBug;
- 添加移动好友网路请求；
- 添加同意请求后，打开聊天窗口；

### 20180201
- 添加对分组联系的操作(修改备注、移动好友);
- 添加同意好友请求后，双向发送对方好友信息；
- 主面板添加提示消息入口；
- 修复回车键发送信息后，聊天输入框自动多出一行的问题；
- 修复聊天信息输入框中，删除清空后，接收到的信息为空的问题；
- 实现聊天记录界面鼠标滚动可以显示历史信息的功能；

### 20180131
- 初步实现简单聊天信息发送，验证了流程；

### 20180130
- 修复聊天输入框不能输入信息的问题；
- 根据好友列表，动态显示分组；
- 添加分组操作；
- 添加客户端发送聊天消息；

### 20180129
- 添加发送同意/拒绝好友请求，服务器实现对在线好友请求转发；
- 实现好友添加功能；
- 添加好友列表获取功能；
- 添加快捷键设置界面，实现截屏快捷键设置、发送信息快捷键设置;

### 20180127
- 添加系统通知显示列表;
- 修复若干细节问题;

### 20180126
- 实现服务器转发好友请求；
- 实现根据不同的状态，播放音频提示音；

### 20180125
- 添加搜索联系人，查看搜索结果;
- 调整SQL语句拼接，借鉴Hibernate面向对象查询；
- 添加发送好友请求；

### 20180124
- 窗口抖动;
- 按回车键发送聊天信息;
- 添加登陆、注册结果信息验证，实现完整流程；
- 添加对个人信息查看、更新操作；
- 添加屏蔽网络连接宏__NO_SERVER__
- 添加SQL语句拼接，简化SQL使用;

### 20180123
- 增加客户端和服务器端分包发送、接收、组包功能；
- 添加聊天记录数据库；
- 添加截图插件功能；

### 20180119
- 修改登陆界面，取消UI界面；
- 实现异步网络连接，网络错误处理。

### 20180118
- 调整JSON传输key值访问；

### 20180117
- 添加注册页面；
- 添加网络设置页面，并支持修改和本地加载；
- 初步实现用户注册，验证流程；

### 20180116
- 添加基础通信协议格式；
- 实现客户端和服务器端双向基础信息通信；

### 20180114
- 添加数据库表以及连接管理；
- 添加服务器网络接收，实现与客户端基本通信；

### 20180109
- 封装windows底层的socket，并支持linux
- 调整工程结果文件保存路径(bin、lib)

### 20180108
- 调整程序结构(改成子项目模式)

### 20180105
- 修复自定义弹出框关闭奔溃；
- 建立网路哦通信协议。
- 添加网络接收模块。
- 添加线程处理模块。

### 20180102
- 完善系统设置界面(区分基础设置和安全设置)
- 添加自定义提示框

### 20171229
- 添加系统设置页面

### 20171228
- 添加系统图像选择页面;
- 解决VS和MingW下编译警告问题;
- 添加程序图标和程序描述信息

### 20171227
- 添加个人信息编辑页面；
- 修改工具栏，支持定制显示；

### 20171226
- 添加个人信息显示页面，支持延迟显示；
- 添加对快捷键的保存和加载；

### 20171225
- 添加联系人搜索和结果显示页面；
- 修改编码为utf-8(BOM)以支持MSVC和MING编译器;

### 20171222
- 完成聊天界面的设计；
- 添加样式表

### 20171221
- 修改图标显示为圆角;
- 调整工具栏创建。


### 20171220
- 添加联系人群组、历史聊天信息界面;
- 添加聊天界面抽象窗口;

### 20171219
- 添加自定义工具页面，参照于QQ；
- 添加动作处理ActionManager类，管理创建的Menu、Action以及ToolButton;
- 完成联系人界面基本的功能。

### 20171215
- 完成主面板的布局;
- 创建顶层窗口，实现对无边框窗口的拖拽、阴影的绘制.

### 20171214
- 添加主面板页面;
- 完善退出、配置文件记录等功能;
- 建立客户端登录页面;
- 添加日志记录;
- 添加系统配置文件操作.

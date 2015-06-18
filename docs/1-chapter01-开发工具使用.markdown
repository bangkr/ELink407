# Elink407开发板的使用 #

## 串口工具的安装使用 ##

### 常用串口工具软件 ###

串口工具，也就是串行通信接口调试软件，不仅广泛应用于嵌入式控制领域的数据监控、数据采集、数据分析等工作,而且在嵌入式开发的过程中打印调试信息，和运行的结果，是嵌入式开发的必备工具之一。我们例程的运行结果大多是通过串口打印出来的，在这里特别注明的是通过Elink407的USART0，进行数据通信，因此你需要通过USB串口线连接到PC机，先在PC上安装USB转串口的驱动，然后在PC机运行一个终端仿真软件（或者叫串口工具）查看结果。我们常用的串口调试工具包括串口调试助手、SecureCRT、超级终端、串口调试大师、putty等， 在这里推荐使用SecureCRT作为串口工具,它比其他的一些d串口工具类的软件功能丰富，稳定好用。

### USB2UART驱动的安装 ###

嵌入式领域使用的最广泛的应该莫过于RS232串行通信接口，我们的Elink407跟PC通信方式也是RS232接口。传统的主板台式机都有这个接口，笔记本就很少再带有这些老式接口,那么有什么方法使得我们笔记本的USB跟开发板的RS232串口进行通讯呢？针对这种情况我们在开发板上加了一个USB转串口的芯片，然后在PC上安装该芯片的驱动，通过USB转串口的方式来解决这个问题。
下面是我们驱动程序所在的路径：

    Elink407\tools\usb2uart driver\PL-2303 Vista Driver Installer（Win7、8）.zip
    
    Elink407\tools\usb2uart driver\PL2303_Prolific_DriverInstaller_v1_9_0（WinXP）.zip
    
他们分别是WIN7,8 和XP的驱动，Linux平台上默认安装了该驱动。驱动的安装过程，只需要解压压缩包，双击安装的exe文件即可。

![](img/chapter01/1.2.1.png) 

### 串口工具的安装使用 ###
SecureCRT是一款支持SSH和串口的终端仿真程序，功能非常强大，我们只使用其中的串口部分即可。安装文件位置：

        光盘\Elink407\tools
        
解压  SecureCRT.rar文件，启动SecureCRT软件，使用时会弹出 快速连接 窗口，如下图： 

![](img/chapter01/1.3.1.png) 

![](img/chapter01/1.3.2.png) 

设置串口属性，在标签下点右键弹出回话选项，如图所示：

![](img/chapter01/1.3.5.png) 

设置串口属性，如图所示

![](img/chapter01/1.3.6.png) 

这里用户具体使用哪个端口，需要根据设备管理器里面当前存在的端口号进行设置，如果没有发现端口，查看usb转串口的驱动是否安装成功。

## 二进制程序烧写方法 ##
程序代码写完以后，通过编译器编译生成可以在该平台可以运行的指令数据，这些数据通常是HEX文件的方式，那么如何将这个HEX文件下载（烧录）到嵌入式平台中的flash中呢？下面我们就来介绍一下Elink407的烧写方法。STM32F4 的程序下载有多种方法：USB、串口、JTAG、SWD 等，这几种方式，都可以用来给 STM32F4 下载代码。

### 使用 STM32 STLINK Utility 工具烧写 ##
ST-LINK/V2是STM8和STM32微控制器系列的在线调试器和编程器。单线接口模块（SWIM）和串行线调试（SWD）接口用于与应用板上的STM8和STM32微控制器通讯。

STM8的应用使用USB全速接口与STVisualDevelop(STVD)，STVisualProgram(STVP)或IAREWSTM8等集成开发环境通讯。
STM32的应用使用USB全速接口与Atollic，IAR，Keil或TASKING等集成开发环境通讯。

**烧录工具安装**   
在目录Elink407\tools\stlink2 下可以找到安装包STM32 ST-LINK Utility_v3.3.0.exe应用程序，双击安装，按照提示依次选择“Next”->“Yes”->“Next”，在弹出的窗口点击“下一步”->“完成”，最后点击finish完成安装。安装完成后将STLINKV2插入电脑的USB接口，此时计算机会提示发现新硬件，并提示安装驱动，请选择自动安装。此时，ST-LINKV2的D2LED会不停的闪烁，当D2长亮时，说明ST-LINKV2可以使用了。此时，打开计算机的设备管理器会发现在“通用串行总线控制器”选项里面，多了一个器件，这就是ST-LINKV2。

**ST-LINKV2固件的升级**
ST-LINKV2可以在线升级固件，目前的最新固件为V2.J17.S4。更新固件的方法为：打开STM32ST-LINKUtility软件(安装光盘里面带的2个压缩文件解压后的软件)，将ST-LINKV2插入计算机，在菜单中选择“ST-LINK”菜单，选中第一个子菜单“Firmwareupdate”

![](img/chapter01/1.3.7.png) 

点击DeviceConnect按钮，此时对话框界面会提示当前固件版本及最新的固件版本，点击Yes按钮，固件就会自动升级

**驱动安装**
调试器这里我们采用ST-LINK，接口为USB，位于开发板屏幕下侧。驱动程序文件位于keil安装目录，如果采用默认安装，将位于Elink407\tools\stlink2\drivers\stsw-link009.zip压缩文件进行解压，进入在解压的目录，该目录双击脚本stlink_winusb_install.bat自动完成驱动安装。

**烧写**
将配套USB线连接在一端连接再PC另一端连接到开发板屏幕下侧的USB口，该USB接口专门用于烧写程序和调试代码使用。连接成功后红色电源灯会亮，如果电源不亮，请检查开发板J7跳线是否已经连接。
 打开ST-LINK，在标签栏选择“Target”->“Connect”。如连接成功，ST-LINK会读取到硬件信息，如图所示：
 
在ST-LINK界面选择	Binary File，在弹出的对话窗口选择打开Elink407_test.hex文件，如图-3所示：

最后选择“Target”->“program&verify…”烧写到开发板并运行测试

### 使用 STM32 and STM8 Flash loader demonstrator 工具烧写 ###
在Elink407\tools\目录中找到Flash loader demonstrator工具，双击运行安装；Flash loader demonstrator工具是通过串口下载烧录文件，因此，确保已经安装了usb转232的驱动，要将USB一端插入Elink407，一端插入电脑上，并在“设备管理器”中查看到虚拟串口的串口号。启动烧录工具，配置串口号，其余参数使用默认值即可。在点击next前，一定要将boot0引脚置高并复位。如果一切正常，软件会转向下一个页面。当然如果出错，则需要重复上述操作。

![](img/chapter01/2.2.1.jpg) 

如果芯片的flash未被读、写保护，那么“Remove protection”按键是灰色的；如果芯片的flash被保护了，那么，这个按钮可以取消保护，对flash进行写入操作。

![](img/chapter01/2.2.2.png) 

这个页面基本上是只读的，可以看到芯片的flash的基本信息显示   

![](img/chapter01/2.2.3.png) 

对flash的烧写与擦除就集中在这个页面里。在这个页面里我们可以擦除（erase）芯片现有的flash内容。我们可以下载（download）hex程序到芯片flash内，我们也可以使能/去除芯片的保护（disable/enable）功能，还是配置选项字的高级功能。
 
![](img/chapter01/2.2.4.png) 

这里简单说明一下，擦除功能可以选择全部擦除（All），也可以选择擦除部分扇区（selection），点击后即弹出相应区块的页面。

下载flash（download），软件支持英特尔hex，binary与S19摩托罗拉格式。其余部分我们初级使用默认即可。

上传芯片内部flash内容（upload），该功能可以将芯片内部的flash读出来，并转存为相应的格式，其格式支持上面提到三种。

选项字配置，这个配置我们暂时用不到，留到后期高级应用时，我们再回过头来仔细研究其强大的作用与深远的意义。

![](img/chapter01/2.2.5.png) 

如果进度条变成绿色的并提示下载成功，那么，恭喜你！程序已经成功下载至芯片内部了。这时，需要将boot0置低，复位微处理器，然后微处理器就执行新的程序了！

### 使用 FlyMcu 程序烧写 ###

打开FlyMcu,切换到STMISP tab页面  ，然后选中USB转串口COM PORT

![](img/chapter01/2.3.0.png) 

选择要烧录的HEX文件

![](img/chapter01/2.3.1.png) 

设置烧写方式，一定要选 DTR低电平复位，RTS高电平进BootLoader

![](img/chapter01/2.2.5.png) 

## 开发板测试程序烧写与使用 ##

![](img/chapter01/2.2.5.png) 

使用 STM32 STLINK Utility 工具烧写


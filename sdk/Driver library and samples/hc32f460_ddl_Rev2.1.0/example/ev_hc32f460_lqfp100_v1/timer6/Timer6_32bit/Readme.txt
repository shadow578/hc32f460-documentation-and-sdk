﻿================================================================================
                                样例使用说明
================================================================================
版本历史
日期             作者        IAR     MDK     GCC     描述
2018-11-28       CDT        7.70    5.16    8.3.1   first version
================================================================================
平台说明
================================================================================
GCC工程，由Eclipse IDE外挂GNU-ARM Toolchain，再结合pyOCD GDB Server实现工程的编译、
链接和调试。在用Eclipse导入工程后，请将xxxx_PyOCDDebug中pyocd-gdbserver和SVD文件
设置为正确的路径；请将xxxx_PyOCDDownload中pyocd设置为正确的路径。注意，这些路径不
能包含非英文字符。

功能描述
================================================================================
本样例主要展示MCU的TIMER6的级联功能，

说明：
配置timer6，hclk设置为168M，Timer61周期为0xFFFF，Timer62周期为500
Timer61计数溢出，产生OVF事件，触发Timer62向上计数加1
================================================================================
测试环境
================================================================================
测试用板:
---------------------
EV-HC32F460-LQFP100-050-V1.3

辅助工具:
---------------------
示波器

辅助软件:
---------------------
无

================================================================================
使用步骤
================================================================================
1） 打开工程编译并全速运行。
2） 在Timer62中断内翻转LED_GREEN，可以看到LED_GREEN闪烁。


================================================================================
注意
================================================================================


================================================================================

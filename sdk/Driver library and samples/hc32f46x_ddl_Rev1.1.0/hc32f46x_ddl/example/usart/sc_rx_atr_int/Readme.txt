﻿================================================================================
                                样例使用说明
================================================================================
版本历史
日期        版本    负责人         IAR     MDK   描述
2018-11-27  1.0     Hongjh         7.70    5.16  first version
================================================================================
功能描述
================================================================================
本样例主要展示USART外设配置为Smart Card模式时通过中断方式读取智能卡ATR数据。
说明：
无

================================================================================
测试环境
================================================================================
测试用板:
---------------------
EV-HC32F460-LQFP100-050-V1.1


辅助工具:
---------------------
无

辅助软件:
---------------------
串口助手软件

================================================================================
使用步骤
================================================================================
1）将智能卡插入测试板SOCKET。
2）打开串口助手软件，配置端口如下参数。
     波特率：115200
     数据位：8
     校验位：None
     停止位：1
3）打开工程并重新编译。
4）下载代码测试板，并且全速运行。
5）串口助手软件显示智能卡ATR数据，第一个数据十六进制值为0x3B，与智能卡TS字符匹配。

================================================================================
注意
================================================================================
如使用时更改USART通道，需同时修改UART外设通道，GPIO配置、外设Clock使能和配置
NVIC功能的相应事件号。

================================================================================

﻿================================================================================
                                样例使用说明
================================================================================
版本历史 
日期        版本    负责人         IAR     MDK   描述
2018-11-02  1.0     Chengy         7.70    5.16  first version
================================================================================
功能描述
================================================================================
本样例为flash 窗口保护，在保护区域外编程，触发中断。


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
无

================================================================================
使用步骤
================================================================================
1）打开工程并重新编译；
2）启动IDE的下载和调试功能；
3）全速运行，查看memory地址（0x0007D000），可观察到窗口内地址，正确编程；
4）按下SW2，在窗口外地址编程，可看到测试版上红灯亮（触发中断）。


================================================================================
注意
================================================================================


================================================================================

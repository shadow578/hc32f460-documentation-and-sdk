﻿================================================================================
                                样例使用说明
================================================================================
版本历史 
日期        版本    负责人         IAR     MDK   描述
2018-11-05  1.0     Chengy         7.70    5.16  first version
================================================================================
功能描述
================================================================================
本样例为flash引导交换。


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
1）打开efm_seqence_program工程，下载并全速运行（对flash全擦出）；
2）打开本样例工程，编译下载，全速运行，可观察到LED1（红灯）闪烁；
3）remove main.c, add 同路径下的main_sector1.c；
4）复位MCU，编译下载，全速运行，可观察到LED2（绿灯）闪烁；
5）反复复位MCU，可观察到LED1和LED2交替闪烁。


================================================================================
注意
================================================================================


================================================================================

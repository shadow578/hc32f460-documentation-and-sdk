﻿================================================================================
                                样例使用说明
================================================================================
版本历史
日期        版本    负责人         IAR     MDK   描述
2018-11-29   1.0     Husj          7.70    5.16  first version
================================================================================
功能描述
================================================================================
本样例主要展示MCU的TIMER6的3组定时器同步启动（输出PWM）、同步停止、同步清零功能
说明：
配置timer6，hclk设置为168M，timer6周期为400us
Timer61：PWMA（PE09），PWMB（PW08）
Timer62：PWMA（PE11），PWMB（PW10）
Timer63：PWMA（PE13），PWMB（PW12）
================================================================================
测试环境
================================================================================
测试用板:
---------------------
EV-HC32F460-LQFP100-050-V1.1

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
2） 使用示波器查看上述六个引脚的信号，可以看到六路PWM同步开始和停止，并且中心对称。

================================================================================
注意
================================================================================


================================================================================

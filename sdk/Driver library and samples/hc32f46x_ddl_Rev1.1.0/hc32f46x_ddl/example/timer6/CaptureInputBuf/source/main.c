/*******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
 *
 * This software is owned and published by:
 * Huada Semiconductor Co.,Ltd ("HDSC").
 *
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
 * BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
 *
 * This software contains source code for use with HDSC
 * components. This software is licensed by HDSC to be adapted only
 * for use in systems utilizing HDSC components. HDSC shall not be
 * responsible for misuse or illegal use of this software for devices not
 * supported herein. HDSC is providing this software "AS IS" and will
 * not be responsible for issues arising from incorrect user implementation
 * of the software.
 *
 * Disclaimer:
 * HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
 * REGARDING THE SOFTWARE (INCLUDING ANY ACCOMPANYING WRITTEN MATERIALS),
 * ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
 * WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
 * WARRANTY OF NONINFRINGEMENT.
 * HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
 * LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
 * INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
 * SAVINGS OR PROFITS,
 * EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
 * INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
 * FROM, THE SOFTWARE.
 *
 * This software may be replicated in part or whole for the licensed use,
 * with the restriction that this Disclaimer and Copyright notice must be
 * included with each copy of this software, whether used in part or whole,
 * at all times.
 */
/******************************************************************************/
/** \file main.c
 **
 ** \brief This sample demonstrates how to use Timer6.
 **
 **   - 2018-11-30  1.0  husj  first version for Device Driver Library of Timer6.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* KEY0 (SW2)*/
#define  SW2_PORT           (PortD)
#define  SW2_PIN            (Pin03)
/* KEY1 (SW4)*/
#define  SW4_PORT           (PortD)
#define  SW4_PIN            (Pin04)
/* KEY2 (SW3)*/
#define  SW3_PORT           (PortD)
#define  SW3_PIN            (Pin05)
/* KEY3 (SW5)*/
#define  SW5_PORT           (PortD)
#define  SW5_PIN            (Pin06)

/* LED0 Port/Pin definition */
#define  LED0_PORT          (PortE)
#define  LED0_PIN           (Pin06)

/* LED1 Port/Pin definition */
#define  LED1_PORT          (PortD)
#define  LED1_PIN           (Pin07)

/* LED2 Port/Pin definition */
#define  LED2_PORT          (PortB)
#define  LED2_PIN           (Pin05)

/* LED3 Port/Pin definition */
#define  LED3_PORT          (PortB)
#define  LED3_PIN           (Pin09)

/* LED0~1 toggle definition */
#define  LED0_TOGGLE()      (PORT_Toggle(LED0_PORT, LED0_PIN))
#define  LED1_TOGGLE()      (PORT_Toggle(LED1_PORT, LED1_PIN))
#define  LED2_TOGGLE()      (PORT_Toggle(LED2_PORT, LED2_PIN))
#define  LED3_TOGGLE()      (PORT_Toggle(LED3_PORT, LED3_PIN))

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
uint16_t u16CaptureA;
uint16_t u16CaptureC;
float ftCapDuty;

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 *******************************************************************************
 ** \brief Callback function of external interrupt ch.0
 **
 ******************************************************************************/

void Timer61_OverFlow_CallBack(void)
{
    static uint8_t i;

    if( 0u == i)
    {
        Timer6_SetGeneralCmpValue(M4_TMR61, Timer6GenCompareC, 0x2000u);
        i = 1u;
    }
    else
    {
        Timer6_SetGeneralCmpValue(M4_TMR61, Timer6GenCompareC, 0x6000u);
        i = 0u;
    }
}


void Timer62_CapInputCallBack(void)
{
    u16CaptureA = Timer6_GetGeneralCmpValue(M4_TMR62, Timer6GenCompareA);
    u16CaptureC = Timer6_GetGeneralCmpValue(M4_TMR62, Timer6GenCompareC);
    if(0u == M4_PORT->PCRE11_f.PIN)
    {
        ftCapDuty = (float)u16CaptureC / (float)u16CaptureA;
    }
}

/**
 ******************************************************************************
 ** \brief  Initialize the system clock for the sample
 **
 ** \param  None
 **
 ** \return None
 ******************************************************************************/
static void SysClkIni(void)
{
    en_clk_sys_source_t       enSysClkSrc;
    stc_clk_sysclk_cfg_t      stcSysClkCfg;
    stc_clk_mpll_cfg_t        stcMpllCfg;

    MEM_ZERO_STRUCT(enSysClkSrc);
    MEM_ZERO_STRUCT(stcSysClkCfg);
    MEM_ZERO_STRUCT(stcMpllCfg);

    /* Set bus clk div. */
    stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;   // Max 168MHz
    stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;  // Max 84MHz

    stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // Max 168MHz
    stcSysClkCfg.enPclk1Div = ClkSysclkDiv2;  // Max 84MHz
    stcSysClkCfg.enPclk2Div = ClkSysclkDiv4;  // Max 60MHz
    stcSysClkCfg.enPclk3Div = ClkSysclkDiv4;  // Max 42MHz
    stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  // Max 84MHz
    CLK_SysClkConfig(&stcSysClkCfg);

    CLK_HrcCmd(Enable);       //Enable HRC

    /* MPLL config. */
    stcMpllCfg.pllmDiv = 2ul;   //HRC 16M / 2
    stcMpllCfg.plln    =42ul;   //8M*42 = 336M
    stcMpllCfg.PllpDiv = 2ul;   //MLLP = 168M
    stcMpllCfg.PllqDiv = 2ul;   //MLLQ = 168M
    stcMpllCfg.PllrDiv = 2ul;   //MLLR = 168M
    CLK_SetPllSource(ClkPllSrcHRC);
    CLK_MpllConfig(&stcMpllCfg);

    /* flash read wait cycle setting */
    EFM_Unlock();
    EFM_SetLatency(EFM_LATENCY_5);
    EFM_Lock();

    /* Enable MPLL. */
    CLK_MpllCmd(Enable);

    /* Wait MPLL ready. */
    while(Set != CLK_GetFlagStatus(ClkFlagMPLLRdy))
    {
        ;
    }

    /* Switch system clock source to MPLL. */
    CLK_SetSysClkSource(CLKSysSrcMPLL);
}

/**
 *******************************************************************************
 ** \brief  Main function of project
 **
 ** \param  None
 **
 ** \retval int32_t return value, if needed
 **
 ******************************************************************************/
int32_t main(void)
{
    uint16_t                         u16Period;
    uint16_t                         u16Compare;
    stc_timer6_basecnt_cfg_t         stcTIM6BaseCntCfg;
    stc_timer6_port_output_cfg_t     stcTIM6PWMxCfg;
    stc_timer6_port_input_cfg_t      stcTIM6CapxCfg;
    stc_timer6_gcmp_buf_cfg_t        stcGCMPBufCfg;
    stc_port_init_t                  stcPortInit;
    stc_irq_regi_conf_t              stcIrqRegiConf;
    stc_timer6_sw_sync_t             stcSwSyncStart;

    MEM_ZERO_STRUCT(stcTIM6BaseCntCfg);
    MEM_ZERO_STRUCT(stcTIM6PWMxCfg);
    MEM_ZERO_STRUCT(stcTIM6CapxCfg);
    MEM_ZERO_STRUCT(stcGCMPBufCfg);
    MEM_ZERO_STRUCT(stcPortInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(stcSwSyncStart);

    SysClkIni();

    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM61, Enable);
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM62, Enable);

    PORT_SetFunc(PortE, Pin09, Func_Tim6, Disable);    //Timer61 PWMA
    //PORT_SetFunc(PortE, Pin08, Func_Tim6, Disable);  //Timer61 PWMB

    PORT_SetFunc(PortE, Pin11, Func_Tim6, Disable);    //Timer62 PWMA
    //PORT_SetFunc(PortE, Pin10, Func_Tim6, Disable);  //Timer62 PWMB

    //PORT_SetFunc(PortE, Pin13, Func_Tim6, Disable);  //Timer63 PWMA
    //PORT_SetFunc(PortE, Pin12, Func_Tim6, Disable);  //Timer63 PWMB


    /******************Config Timer61 as output signal*************************/

    stcTIM6BaseCntCfg.enCntMode   = Timer6CntSawtoothMode;              //Sawtooth wave mode
    stcTIM6BaseCntCfg.enCntDir    = Timer6CntDirUp;                     //Counter counting up
    stcTIM6BaseCntCfg.enCntClkDiv = Timer6PclkDiv1024;                  //Count clock: pclk0/1024
    Timer6_Init(M4_TMR61, &stcTIM6BaseCntCfg);                          //timer6 PWM frequency, count mode and clk config

    u16Period = 0x8000u;
    Timer6_SetPeriod(M4_TMR61, Timer6PeriodA, u16Period);                //period set

    u16Compare = 0x2000u;
    Timer6_SetGeneralCmpValue(M4_TMR61, Timer6GenCompareA, u16Compare);  //Set General Compare RegisterA Value
    u16Compare = 0x6000u;
    Timer6_SetGeneralCmpValue(M4_TMR61, Timer6GenCompareC, u16Compare);  //Set General Compare RegisterC Value as buffer register of GCMAR


    /*PWMA/PWMB output buffer config*/
    stcGCMPBufCfg.bEnGcmpTransBuf = true;
    stcGCMPBufCfg.enGcmpBufTransType = Timer6GcmpPrdSingleBuf;          //Single buffer transfer
    Timer6_SetGeneralBuf(M4_TMR61, Timer6PWMA, &stcGCMPBufCfg);         //GCMAR buffer transfer set

    stcTIM6PWMxCfg.enPortMode = Timer6ModeCompareOutput;    //Compare output function
    stcTIM6PWMxCfg.bOutEn     = true;                       //Output enable
    stcTIM6PWMxCfg.enPerc     = Timer6PWMxCompareLow;       //PWMA port output low level when CNTER value match PERAR
    stcTIM6PWMxCfg.enCmpc     = Timer6PWMxCompareHigh;      //PWMA port output high level when CNTER value match with GCMAR
    stcTIM6PWMxCfg.enStaStp   = Timer6PWMxStateSelSS;       //PWMA output status is decide by STACA STPCA when CNTER start and stop
    stcTIM6PWMxCfg.enStaOut   = Timer6PWMxPortOutLow;       //PWMA port output set low level when CNTER start
    stcTIM6PWMxCfg.enStpOut   = Timer6PWMxPortOutLow;       //PWMA port output set low level when CNTER stop
    stcTIM6PWMxCfg.enDisVal   = Timer6PWMxDisValLow;
    Timer6_PortOutputConfig(M4_TMR61, Timer6PWMA, &stcTIM6PWMxCfg); //Output config

    /*config interrupt*/
    /* Enable timer61 under flow interrupt */
    Timer6_ConfigIrq(M4_TMR61, Timer6INTENOVF, true);

    stcIrqRegiConf.enIRQn = Int002_IRQn;                    //Register INT_TMR61_GUDF Int to Vect.No.002
    stcIrqRegiConf.enIntSrc = INT_TMR61_GOVF;               //Select Event interrupt function
    stcIrqRegiConf.pfnCallback = &Timer61_OverFlow_CallBack; //Callback function
    enIrqRegistration(&stcIrqRegiConf);                     //Registration IRQ

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);            //Clear Pending
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_03);//Set priority
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);                   //Enable NVIC


    /******************Config Timer62 as output signal*************************/

    stcTIM6BaseCntCfg.enCntMode   = Timer6CntSawtoothMode;              //Sawtooth wave mode
    stcTIM6BaseCntCfg.enCntDir    = Timer6CntDirUp;                     //Counter counting up
    stcTIM6BaseCntCfg.enCntClkDiv = Timer6PclkDiv1024;                  //Count clock: pclk0/1024
    Timer6_Init(M4_TMR62, &stcTIM6BaseCntCfg);                          //timer6 PWM frequency, count mode and clk config

    u16Period = 0xFFFFu;
    Timer6_SetPeriod(M4_TMR62, Timer6PeriodA, u16Period);               //Period set

    /*PWMA input buffer config*/
    stcGCMPBufCfg.bEnGcmpTransBuf = true;
    stcGCMPBufCfg.enGcmpBufTransType = Timer6GcmpPrdSingleBuf;         //Single buffer transfer
    Timer6_SetGeneralBuf(M4_TMR62, Timer6PWMA, &stcGCMPBufCfg);        //GCMAR buffer transfer set

    stcTIM6CapxCfg.enPortSel  = Timer6xCHA;                            //Capture Input Port: PWM A port
    stcTIM6CapxCfg.enPortMode = Timer6ModeCaptureInput;                //Capture input function
    stcTIM6CapxCfg.bFltEn     = true;                                  //Input filter enable
    stcTIM6CapxCfg.enFltClk   = Timer6FltClkPclk0Div16;                //Filter clock
    Timer6_PortInputConfig(M4_TMR62, &stcTIM6CapxCfg);                 //Input config


    Timer6_ConfigHwCaptureA(M4_TMR62, Timer6HwTrigPWMARise);       //HW Capture: Timer6 PWMA port rise trig
    Timer6_ConfigHwCaptureA(M4_TMR62, Timer6HwTrigPWMAFall);       //HW Capture: Timer6 PWMA port fall trig
    Timer6_ConfigHwClear(M4_TMR62, Timer6HwTrigPWMAFall);          //HW Clear: Timer6 PWMA port fall trig
    Timer6_EnableHwClear(M4_TMR62);

    /*config interrupt*/
    /* Enable timer62 GCMAR interrupt */
    Timer6_ConfigIrq(M4_TMR62, Timer6INTENA, true);

    stcIrqRegiConf.enIRQn = Int003_IRQn;                    //Register INT_TMR61_GUDF Int to Vect.No.002
    stcIrqRegiConf.enIntSrc = INT_TMR62_GCMA;               //Select Event interrupt function
    stcIrqRegiConf.pfnCallback = &Timer62_CapInputCallBack;  //Callback function
    enIrqRegistration(&stcIrqRegiConf);                     //Registration IRQ

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);            //Clear Pending
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_04);//Set priority
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);                   //Enable NVIC


    /*start timer6*/
    stcSwSyncStart.bTimer61 = true;
    stcSwSyncStart.bTimer62 = true;
    Timer6_SwSyncStart(&stcSwSyncStart);

    while(1)
    {
        ;
    }

}




/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

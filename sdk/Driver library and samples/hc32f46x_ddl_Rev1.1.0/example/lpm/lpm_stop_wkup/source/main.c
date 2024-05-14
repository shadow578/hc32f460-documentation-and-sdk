/*******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software is owned and published by:
 * Huada Semiconductor Co., Ltd. ("HDSC").
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
 ** \brief power voltage detected interrupt sample
 **
 **   - 2018-11-06  1.0  Chengy First version for Device Driver Library of LPM.
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
#define PORT0_IRQn              (Int000_IRQn)
#define PORT1_IRQn              (Int001_IRQn)

/* LED0 Port/Pin definition */
#define LED0_PORT               (PortE)
#define LED0_PIN                (Pin06)

/* LED1 Port/Pin definition */
#define  LED1_PORT              (PortA)
#define  LED1_PIN               (Pin07)

/* LED0 off definition */
#define LED0_OFF()              (PORT_ResetBits(LED0_PORT, LED0_PIN))

/* LED0 off definition */
#define LED1_OFF()              (PORT_ResetBits(LED1_PORT, LED1_PIN))

/* LED0 toggle definition */
#define LED0_TOGGLE()           (PORT_Toggle(LED0_PORT, LED0_PIN))
#define LED1_TOGGLE()           (PORT_Toggle(LED1_PORT, LED1_PIN))

#define DLY_MS                  (1000u)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint32_t u32ExtInt04Count = 0ul;
static uint32_t u32ExtInt05Count = 0ul;

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 *******************************************************************************
 ** \brief  Led init.
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Led_Init(void)
{
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcPortInit);

    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enExInt = Enable;
    stcPortInit.enPullUp = Enable;

    LED0_OFF();
    LED1_OFF();

    /* LED0/1 Port/Pin initialization */
    PORT_Init(LED0_PORT, LED0_PIN, &stcPortInit);
    PORT_Init(LED1_PORT, LED1_PIN, &stcPortInit);
}

/**
 *******************************************************************************
 ** \brief  Port init.
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Port_Init(void)
{
    PORT_Unlock();

    /*  SW4 PD4 */
    M4_PORT->PCRD4_f.INTE = 1u;
    /* SW3 PD5 */
    M4_PORT->PCRD5_f.INTE = 1u;

    PORT_Lock();
}
/**
 *******************************************************************************
 ** \brief  Initialize the system clock as MPLL 168M.
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Clk_Init(void)
{
    en_clk_sys_source_t     enSysClkSrc;
    stc_clk_sysclk_cfg_t    stcSysClkCfg;
    stc_clk_xtal_cfg_t      stcXtalCfg;
    stc_clk_mpll_cfg_t      stcMpllCfg;
    stc_clk_output_cfg_t    stcOutputClkCfg;

    MEM_ZERO_STRUCT(enSysClkSrc);
    MEM_ZERO_STRUCT(stcSysClkCfg);
    MEM_ZERO_STRUCT(stcXtalCfg);
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

    /* Switch system clock source to MPLL. */
    /* Use Xtal as MPLL source. */
    stcXtalCfg.enMode = ClkXtalModeOsc;
    stcXtalCfg.enDrv = ClkXtalLowDrv;
    stcXtalCfg.enFastStartup = Enable;
    CLK_XtalConfig(&stcXtalCfg);
    CLK_XtalCmd(Enable);

    /* MPLL config (XTAL / pllmDiv * plln / PllpDiv = 168M). */
    stcMpllCfg.pllmDiv = 1ul;
    stcMpllCfg.plln =48ul;
    stcMpllCfg.PllpDiv = 8ul;
    stcMpllCfg.PllqDiv = 2ul;
    stcMpllCfg.PllrDiv = 2ul;
    CLK_SetPllSource(ClkPllSrcXTAL);
    CLK_MpllConfig(&stcMpllCfg);

    /* flash read wait cycle setting */
    EFM_Unlock();
    EFM_SetLatency(EFM_LATENCY_4);
    EFM_Lock();

    /* Enable MPLL. */
    CLK_MpllCmd(Enable);

    /* Switch system clock source to MPLL. */
    CLK_SetSysClkSource(CLKSysSrcMPLL);

    /* Clk output.*/
    stcOutputClkCfg.enOutputSrc = ClkOutputSrcSysclk;
    stcOutputClkCfg.enOutputDiv = ClkOutputDiv8;
    CLK_OutputClkConfig(ClkOutputCh1,&stcOutputClkCfg);
    CLK_OutputClkCmd(ClkOutputCh1,Enable);

    PORT_SetFunc(PortA, Pin08, Func_Mclkout, Disable);
}

static void ExtInt04_Callback(void)
{
    /* Recover clock. */
    PWC_IrqClkRecover();

    /* To show the times the interrupt occured. */
    u32ExtInt04Count++;
    /* Clear the interrupt flag. */
    EXINT_IrqFlgClr(ExtiCh04);

    LED0_TOGGLE();
    Ddl_Delay1ms(DLY_MS);
    LED0_TOGGLE();

    /* Switch system clock as MRC. */
    PWC_IrqClkBackup();
}

static void ExtInt05_Callback(void)
{
    /* Recover clock. */
    PWC_IrqClkRecover();

    /* To show the times the interrupt occured. */
    u32ExtInt05Count++;
    /* Clear the interrupt flag. */
    EXINT_IrqFlgClr(ExtiCh05);

    LED1_TOGGLE();
    Ddl_Delay1ms(DLY_MS);
    LED1_TOGGLE();

    /* Switch system clock as MRC. */
    PWC_IrqClkBackup();
}
/**
 *******************************************************************************
 ** \brief  Main function of switch system clock source to MPLL project
 **
 ** \param  None
 **
 ** \retval int32_t return value, if needed
 **
 ******************************************************************************/
int32_t main(void)
{
    stc_pwc_stop_mode_cfg_t stcPwcStopCfg;
    stc_exint_config_t      stcExintCfg;
    stc_irq_regi_conf_t     stcPortIrqCfg;
    uint32_t u32tmp1, u32tmp2;

    MEM_ZERO_STRUCT(stcPwcStopCfg);

    Led_Init();
    Port_Init();
    Clk_Init();

    Ddl_Delay1ms(DLY_MS);

    /* Config stop mode. */
    stcPwcStopCfg.enStpDrvAbi = StopHighspeed;
    stcPwcStopCfg.enStopClk = ClkFix;
    stcPwcStopCfg.enStopFlash = Wait;
    stcPwcStopCfg.enPll = Enable;

    while(Ok != PWC_StopModeCfg(&stcPwcStopCfg))
    {
        ;
    }

    /* SW4 config. */
    stcExintCfg.enExitCh = ExtiCh04;
    stcExintCfg.enFilterEn = Disable;
    stcExintCfg.enExtiLvl = ExIntFallingEdge;
    EXINT_Init(&stcExintCfg);

    /* Register EIRQ4.*/
    stcPortIrqCfg.enIntSrc = INT_PORT_EIRQ4;
    stcPortIrqCfg.enIRQn = PORT0_IRQn;
    stcPortIrqCfg.pfnCallback = &ExtInt04_Callback;
    enIrqRegistration(&stcPortIrqCfg);

    /* Set wake up source EIRQ4. */
    enIntWakeupEnable(Extint4WU);

    /* Enable EIRQ4. */
    enIntEnable(Int4);
    NVIC_ClearPendingIRQ(PORT0_IRQn);
    NVIC_SetPriority(PORT0_IRQn,DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(PORT0_IRQn);

    /* SW3 config. */
    stcExintCfg.enExitCh = ExtiCh05;
    stcExintCfg.enFilterEn = Disable;
    stcExintCfg.enExtiLvl = ExIntFallingEdge;
    EXINT_Init(&stcExintCfg);

    /* Register EIRQ5.*/
    stcPortIrqCfg.enIntSrc = INT_PORT_EIRQ5;
    stcPortIrqCfg.enIRQn = PORT1_IRQn;
    stcPortIrqCfg.pfnCallback = &ExtInt05_Callback;
    enIrqRegistration(&stcPortIrqCfg);

    /* Set wake up source EIRQ5. */
    enIntWakeupEnable(Extint5WU);

    /* Enable EIRQ5. */
    enIntEnable(Int5);
    NVIC_ClearPendingIRQ(PORT1_IRQn);
    NVIC_SetPriority(PORT1_IRQn,DDL_IRQ_PRIORITY_14);
    NVIC_EnableIRQ(PORT1_IRQn);


    /* Ensure DMA disable */
    u32tmp1 =  M4_DMA1->EN_f.EN;
    u32tmp2 =  M4_DMA2->EN_f.EN;
    while((0ul != u32tmp1) && ((0ul != u32tmp2)))
    {
        ;
    }
    /* Ensure FLASH is ready */
    while(1ul != M4_EFM->FSR_f.RDY)
    {
        ;
    }

    /* SW2 */
    while(0 != PORT_GetBit(PortD, Pin03))
    {
        ;
    }

    while(1)
    {
        /* Enter stop mode. */
        PWC_EnterStopMd();
    }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

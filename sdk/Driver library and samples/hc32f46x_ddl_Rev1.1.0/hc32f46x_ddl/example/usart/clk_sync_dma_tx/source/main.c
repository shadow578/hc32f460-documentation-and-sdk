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
 ** \brief This sample demonstrates clock sync data receive and transfer by DMA.
 **
 **   - 2018-11-27  1.0  Hongjh First version for Device Driver Library of USART
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/**
 *******************************************************************************
 ** \brief buffer handle
 **
 ******************************************************************************/
typedef struct stc_buf_handle
{
    uint8_t u8Size;
    uint8_t au8Buf[200];
} stc_buf_handle_t;

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* DMAC */
#define DMA_UNIT                        (M4_DMA1)
#define DMA_CH                          (DmaCh0)
#define DMA_TRG_SEL                     (EVT_USART2_TI)

/* USART channel definition */
#define USART_CH                        (M4_USART2)

/* USART baudrate definition */
#define USART_BAUDRATE                  (3000000ul)

/* USART TX Port/Pin definition */
#define USART_TX_PORT                   (PortA)
#define USART_TX_PIN                    (Pin02)
#define USART_TX_FUNC                   (Func_Usart2_Tx)

/* USART CK Port/Pin definition */
#define USART_CK_PORT                   (PortD)
#define USART_CK_PIN                    (Pin07)
#define USART_CK_FUNC                   (Func_Usart_Ck)

/* DMA block transfer complete interrupt */
#define DMA_BTC_INT_NUM                 (INT_DMA1_BTC0)
#define DMA_BTC_INT_IRQn                (Int002_IRQn)

/* LED(D26: green color) Port/Pin definition */
#define LED_PORT                        (PortA)
#define LED_PIN                         (Pin07)

/* LED operation */
#define LED_ON()                        (PORT_SetBits(LED_PORT, LED_PIN))
#define LED_OFF()                       (PORT_ResetBits(LED_PORT, LED_PIN))

/* User key:SW2 Port/Pin definition */
#define KEY_PORT                        (PortD)
#define KEY_PIN                         (Pin03)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void ClkInit(void);
static void LedInit(void);
static void DmaInit(void);
static void DmaBtcIrqCallback(void);

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static stc_buf_handle_t m_stcTxBufHanlde;

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 *******************************************************************************
 ** \brief Initialize Clock.
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void ClkInit(void)
{
    stc_clk_xtal_cfg_t   stcXtalCfg;
    stc_clk_mpll_cfg_t   stcMpllCfg;
    en_clk_sys_source_t  enSysClkSrc;
    stc_clk_sysclk_cfg_t stcSysClkCfg;

    MEM_ZERO_STRUCT(enSysClkSrc);
    MEM_ZERO_STRUCT(stcSysClkCfg);
    MEM_ZERO_STRUCT(stcXtalCfg);
    MEM_ZERO_STRUCT(stcMpllCfg);

    /* Set bus clk div. */
    stcSysClkCfg.enHclkDiv  = ClkSysclkDiv1;  /* Max 168MHz */
    stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;  /* Max 84MHz */
    stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  /* Max 168MHz */
    stcSysClkCfg.enPclk1Div = ClkSysclkDiv2;  /* Max 84MHz */
    stcSysClkCfg.enPclk2Div = ClkSysclkDiv4;  /* Max 60MHz */
    stcSysClkCfg.enPclk3Div = ClkSysclkDiv4;  /* Max 42MHz */
    stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  /* Max 84MHz */
    CLK_SysClkConfig(&stcSysClkCfg);

    /* Switch system clock source to MPLL. */
    /* Use Xtal as MPLL source. */
    stcXtalCfg.enMode = ClkXtalModeOsc;
    stcXtalCfg.enDrv = ClkXtalLowDrv;
    stcXtalCfg.enFastStartup = Enable;
    CLK_XtalConfig(&stcXtalCfg);
    CLK_XtalCmd(Enable);

    /* MPLL config. */
    stcMpllCfg.pllmDiv = 1ul;
    stcMpllCfg.plln = 50ul;
    stcMpllCfg.PllpDiv = 4ul;
    stcMpllCfg.PllqDiv = 4ul;
    stcMpllCfg.PllrDiv = 4ul;
    CLK_SetPllSource(ClkPllSrcXTAL);
    CLK_MpllConfig(&stcMpllCfg);

    /* flash read wait cycle setting */
    EFM_Unlock();
    EFM_SetLatency(EFM_LATENCY_5);
    EFM_Lock();

    /* Enable MPLL. */
    CLK_MpllCmd(Enable);

    /* Wait MPLL ready. */
    while (Set != CLK_GetFlagStatus(ClkFlagMPLLRdy))
    {
    }

    /* Switch system clock source to MPLL. */
    CLK_SetSysClkSource(CLKSysSrcMPLL);
}

/**
 *******************************************************************************
 ** \brief Initialize LED.
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void LedInit(void)
{
    stc_port_init_t stcPortInit;

    LED_OFF();

    /* LED Port/Pin initialization */
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enExInt = Enable;
    stcPortInit.enPullUp = Enable;
    PORT_Init(LED_PORT, LED_PIN, &stcPortInit);
}

/**
 *******************************************************************************
 ** \brief Initialize DMA.
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void DmaInit(void)
{
    stc_dma_config_t stcDmaInit;
    stc_irq_regi_conf_t stcIrqRegiCfg;

    /* Enable peripheral clock */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1 | PWC_FCG0_PERIPH_DMA2,Enable);

    /* Enable DMA. */
    DMA_Cmd(DMA_UNIT,Enable);

    /* Initialize DMA. */
    MEM_ZERO_STRUCT(stcDmaInit);
    stcDmaInit.u16BlockSize = 1u; /* 1 block */
    stcDmaInit.u16TransferCnt = (uint16_t)m_stcTxBufHanlde.u8Size;/* Transfer count */
    stcDmaInit.u32SrcAddr = (uint32_t)(m_stcTxBufHanlde.au8Buf);  /* Set source address. */
    stcDmaInit.u32DesAddr = (uint32_t)(&USART_CH->DR);   /* Set destination address. */
    stcDmaInit.stcDmaChCfg.enSrcInc = AddressIncrease;   /* Set source address mode. */
    stcDmaInit.stcDmaChCfg.enDesInc = AddressFix;        /* Set destination address mode. */
    stcDmaInit.stcDmaChCfg.enIntEn = Enable;             /* Enable interrupt. */
    stcDmaInit.stcDmaChCfg.enTrnWidth = Dma8Bit;         /* Set data width 8bit. */
    DMA_InitChannel(DMA_UNIT, DMA_CH, &stcDmaInit);

    /* Enable the specified DMA channel. */
    DMA_ChannelCmd(DMA_UNIT, DMA_CH, Enable);

    /* Clear DMA flag. */
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH, TrnCpltIrq);

    /* Enable peripheral circuit trigger function. */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);

    /* Set DMA trigger source. */
    DMA_SetTriggerSrc(DMA_UNIT, DMA_CH, DMA_TRG_SEL);

    /* Set DMA block transfer complete IRQ */
    stcIrqRegiCfg.enIRQn = DMA_BTC_INT_IRQn;
    stcIrqRegiCfg.pfnCallback = &DmaBtcIrqCallback;
    stcIrqRegiCfg.enIntSrc = DMA_BTC_INT_NUM;
    enIrqRegistration(&stcIrqRegiCfg);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
}

/**
 *******************************************************************************
 ** \brief DMA block transfer complete irq callback function.
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void DmaBtcIrqCallback(void)
{
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH, BlkTrnCpltIrq);
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
    uint8_t i;
    en_result_t enRet = Ok;
    uint32_t u32Fcg1Periph = PWC_FCG1_PERIPH_USART1 | PWC_FCG1_PERIPH_USART2 | \
                             PWC_FCG1_PERIPH_USART3 | PWC_FCG1_PERIPH_USART4;
    const stc_usart_clksync_init_t stcInitCfg = {
        UsartIntClkCkOutput,
        UsartClkDiv_1,
        UsartDataLsbFirst,
        UsartRtsEnable,
    };

    /* Initialize buffer */
    m_stcTxBufHanlde.u8Size = (uint8_t)sizeof(m_stcTxBufHanlde.au8Buf);
    for (i = 0u; i < m_stcTxBufHanlde.u8Size; i++)
    {
        m_stcTxBufHanlde.au8Buf[i] = i;
    }

    /* Initialize Clock */
    ClkInit();

    /* Initialize LED */
    LedInit();

    /* Initialize DMA */
    DmaInit();

    /* Enable peripheral clock */
    PWC_Fcg1PeriphClockCmd(u32Fcg1Periph, Enable);

    /* Initialize USART IO */
    PORT_SetFunc(USART_CK_PORT, USART_CK_PIN, USART_CK_FUNC, Disable);
    PORT_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_FUNC, Disable);

    /* Initialize Clock sync */
    enRet = USART_CLKSYNC_Init(USART_CH, &stcInitCfg);
    if (enRet != Ok)
    {
        while (1)
        {
        }
    }

    /* Set baudrate */
    enRet = USART_SetBaudrate(USART_CH, USART_BAUDRATE);
    if (enRet != Ok)
    {
        while (1)
        {
        }
    }

    /* User key : SW2 */
    while (Reset != PORT_GetBit(KEY_PORT, KEY_PIN))
    {
    }

    /*Enable TX interupt function*/
    USART_FuncCmd(USART_CH, UsartTxAndTxEmptyInt, Enable);

    while (1)
    {
        if (Set == DMA_GetIrqFlag(DMA_UNIT, DMA_CH, TrnCpltIrq))
        {
            LED_ON();  /* Send completely */
        }
    }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

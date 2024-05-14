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
 ** \brief Example project for i2s
 **
 **   - 2018-11-01  1.0  Wangmin First version for Device Driver Library example
 **     for I2S
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "wm8731.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Define if need play by speaker*/
#define SPEAKER_ON                  (true)
/* Define if use exclk */
#define EXCK_ON                     (true)
/* Select Record source */
#define RECORD_MIC                  (true)
/* Define I2C unit used for the example */
#define I2C_CH                      (M4_I2C2)
/* Define port and pin for SDA and SCL */
#define I2C2_SCL_PORT               (PortD)
#define I2C2_SCL_PIN                (Pin00)
#define I2C2_SDA_PORT               (PortD)
#define I2C2_SDA_PIN                (Pin01)
/* Define I2S unit used for the example */
#define I2S_CH                      (M4_I2S3)
/* Define port and pin for i2s1 function */
#if(EXCK_ON)
/* if exck enable*/
#define I2S3_PORT                   (PortB)
#define I2S3_WS_PIN                 (Pin13)
#define I2S3_SD_PIN                 (Pin14)
#define I2S3_SD_IN_PIN              (Pin15)
#define I2S3_CK_PIN                 (Pin12)
#define I2S3_EXCK_PORT              (PortB)
#define I2S3_EXCK_PIN               (Pin10)
#else
/* if exck disable */
#define I2S3_PORT                   (PortB)
#define I2S3_WS_PIN                 (Pin13)
#define I2S3_SD_PIN                 (Pin14)
#define I2S3_SD_IN_PIN              (Pin15)
#define I2S3_CK_PIN                 (Pin10)
#define I2S3_MCK_PORT               (PortB)
#define I2S3_MCK_PIN                (Pin12)
#endif

#define SPK_EN_PORT                 (PortB)
#define SPK_EN_PIN                  (Pin00)

#define SPEAKER_EN()                (PORT_SetBits(SPK_EN_PORT, SPK_EN_PIN))
#define SPEAKER_DISEN()             (PORT_ResetBits(SPK_EN_PORT, SPK_EN_PIN))

/* LED0 Port/Pin definition */
#define  LED0_PORT                  (PortE)
#define  LED0_PIN                   (Pin06)
/* LED0~1 toggle definition */
#define  LED0_TOGGLE()              (PORT_Toggle(LED0_PORT, LED0_PIN))

#define RECORDER_WAVFILELEN         (15u*1024u)

#define DMA_UNIT                    (M4_DMA2)
#define DMA_CH                      (DmaCh0)
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
uint16_t *pu16SoundData=NULL;
uint16_t au16RecorderSoundI2s[RECORDER_WAVFILELEN];
uint8_t u8RecordEndFlag = 0u;
uint32_t u32Count = 0ul;

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void delay(uint32_t u32Tmp)
{
    while(u32Tmp--)
    {
        ;
    }
}

/**
 ******************************************************************************
 ** \brief  Initialize the system clock for the sample
 **
 ** \param  None
 **
 ** \retval None
 ******************************************************************************/
static void SysClkIni(void)
{
    en_clk_sys_source_t     enSysClkSrc;
    stc_clk_sysclk_cfg_t    stcSysClkCfg;
    stc_clk_xtal_cfg_t      stcXtalCfg;
    stc_clk_mpll_cfg_t      stcMpllCfg;

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
    /* Use Xtal32 as MPLL source. */
    stcXtalCfg.enMode = ClkXtalModeOsc;
    stcXtalCfg.enDrv = ClkXtalLowDrv;
    stcXtalCfg.enFastStartup = Enable;
    CLK_XtalConfig(&stcXtalCfg);
    CLK_XtalCmd(Enable);

    /* MPLL config. */
    stcMpllCfg.pllmDiv = 1ul;
    stcMpllCfg.plln    =42ul;
    stcMpllCfg.PllpDiv = 2ul;
    stcMpllCfg.PllqDiv = 2ul;
    stcMpllCfg.PllrDiv = 2ul;
    CLK_SetPllSource(ClkPllSrcXTAL);
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
 ** \brief  Configuration Codec to record wav file
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void WM8731_CodecConfigRecord(void)
{
    stc_wm8731_reg_t stcWm8731Reg;
    MEM_ZERO_STRUCT(stcWm8731Reg);
    /* Config codec */
    /* Reset register */
    stcWm8731Reg.RESET              = 0x00u;    // Reset WM8731
    /* Left & right line input */
    stcWm8731Reg.LLIN_f.LINVOL      = 0x17u;    // Left channel line input volume: 0dB--0x17u
    stcWm8731Reg.LLIN_f.LINMUTE     = 0u;       // Enable left channel line input mute
    stcWm8731Reg.LLIN_f.LRINBOTH    = 0u;       // Disable simultaneous input volume and mute load from left to right
    stcWm8731Reg.RLIN_f.RINVOL      = 0x17u;    // Right channel line input volume 0dB
    stcWm8731Reg.RLIN_f.RINMUTE     = 0u;       // Enable right channel line input mute
    stcWm8731Reg.RLIN_f.RINBOTH     = 0u;       // Disable simultaneous input volume and mute load from right to left
    /* Left & right headphone output */
    stcWm8731Reg.LHOUT_f.LHPVOL     = 0x5Fu;    // Set volume of left headphone to 0dB. 0x30(-73dB) ~ 0x7F(+6dB), 0 ~ 0x2F: mute
    stcWm8731Reg.LHOUT_f.LZCEN      = 0u;       // Disable left channel zero cross detect
    stcWm8731Reg.LHOUT_f.LRHPBOTH   = 0u;       // Disable simultaneous output volume and mute load from left to right
    stcWm8731Reg.RHOUT_f.RHPVOL     = 0x5Fu;    // Set volume of right headphone to 0dB. 0x30(-73dB) ~ 0x7F(+6dB), 0 ~ 0x2F: mute
    stcWm8731Reg.RHOUT_f.RZCEN      = 0u;       // Enable right channel zero cross detect
    stcWm8731Reg.RHOUT_f.RLHPBOTH   = 0u;       // Disable simultaneous output volume and mute load from right to left
    /* Analog audio path control */
#if(RECORD_MIC)
    stcWm8731Reg.AAPC_f.MICBOOST    = 1u;       // Disable boost, 0: disable 1: enable
    stcWm8731Reg.AAPC_f.MUTEMIC     = 0u;       // Enable mute to ADC
    stcWm8731Reg.AAPC_f.INSEL       = 1u;       // Line input select to ADC, 0: linein  1: mic
    stcWm8731Reg.AAPC_f.BYPASS      = 1u;       // Enbale bypass 0: disable 1:enable
    stcWm8731Reg.AAPC_f.DACSEL      = 0u;       // Select DAC
    stcWm8731Reg.AAPC_f.SIDETONE    = 0u;       // Disable side tone 0: disable 1:enable
    stcWm8731Reg.AAPC_f.SIDEATT     = 0u;       // 0: -6dB, 1: -12dB, 2: -9dB, 3: -15dB.
#else
    stcWm8731Reg.AAPC_f.MICBOOST    = 0u;       // Disable boost, 0: disable 1: enable
    stcWm8731Reg.AAPC_f.MUTEMIC     = 1u;       // Enable mute to ADC
    stcWm8731Reg.AAPC_f.INSEL       = 0u;       // Line input select to ADC, 0: linein  1: mic
    stcWm8731Reg.AAPC_f.BYPASS      = 1u;       // Enbale bypass 0: disable 1:enable
    stcWm8731Reg.AAPC_f.DACSEL      = 0u;       // Select DAC
    stcWm8731Reg.AAPC_f.SIDETONE    = 0u;       // Disable side tone 0: disable 1:enable
    stcWm8731Reg.AAPC_f.SIDEATT     = 0u;       // 0: -6dB, 1: -12dB, 2: -9dB, 3: -15dB.
#endif
    /* Digital audio path control */
    stcWm8731Reg.DAPC_f.ADCHPD      = 0u;       // Enable high pass filter
    stcWm8731Reg.DAPC_f.DEEMP       = 0u;       // De-emphasis contrl. 0: disable, 1: 32kHz, 2: 44.1kHz, 3: 48kHz
    stcWm8731Reg.DAPC_f.DACMU       = 0u;       // 0:Disable soft mute   1: Enable soft mute
    stcWm8731Reg.DAPC_f.HPOR        = 0u;       // Clear offset when high pass
    /* Power down control */
    stcWm8731Reg.PDC_f.LINEINPD     = 0u;       // Disable line input power down
    stcWm8731Reg.PDC_f.MICPD        = 0u;       // Disable microphone input power down
    stcWm8731Reg.PDC_f.ADCPD        = 0u;       // Disable ADC power down
    stcWm8731Reg.PDC_f.DACPD        = 0u;       // Disable DAC power down
    stcWm8731Reg.PDC_f.OUTPD        = 0u;       // Disable output power down
    stcWm8731Reg.PDC_f.OSCPD        = 0u;       // Disable oscillator power down
    stcWm8731Reg.PDC_f.CLKOUTPD     = 0u;       // Disable CLKOUT power down
    stcWm8731Reg.PDC_f.POWEROFF     = 0u;       // Disable power off mode
    /* Digital audio interface format */
    stcWm8731Reg.DAIF_f.FORMAT      = 2u;       // 0: MSB-First, right justified, 1: MSB-first, left justified, 2: I2S-format, 3: DSP mode
    stcWm8731Reg.DAIF_f.IWL         = 0u;       // 0: 16 bits, 1: 20 bits, 2: 24 bits, 3: 32 bits
    stcWm8731Reg.DAIF_f.LRP         = 0u;       // 1: right channel DAC data when DACLRC (WS) is high,  0: right channel DAC data when DACLRC (WS) is low
    stcWm8731Reg.DAIF_f.LRSWAP      = 0u;       // 1: swap left channel and right channel, 0: don't swap
    stcWm8731Reg.DAIF_f.MS          = 0u;       // 1: Enable master mode, 0: Enable slave mode
    stcWm8731Reg.DAIF_f.BCLKINV     = 0u;       // Don't invert BCLK
    /* Sampling control */
    stcWm8731Reg.SC_f.NORMAL_USB    = 0u;       // 0: normal mode, 1: USB mode
    stcWm8731Reg.SC_f.BOSR          = 0u;       // Nomrmal mode: 0: 256fs, 1: 384fs
                                                // USB mode: 0: 250fs, 1:272fs
    stcWm8731Reg.SC_f.SR            = 2u;       // Sample rate setting
    stcWm8731Reg.SC_f.CLKDIV2       = 0u;       // 0: core clock is MCLK, 1: core clock is MCLK divided by 2
    stcWm8731Reg.SC_f.CLKODIV2      = 0u;       // 0: output clock is core clock, 1: core clock is core clock/2
    // Active control
    stcWm8731Reg.AC_f.ACTIVE        = 1u;       // 0: inactive, 1: active

    WM8731_Init(I2C_CH, &stcWm8731Reg);

    WM8731_SetHpVolume(I2C_CH, 0x6Fu,0x6Fu);    //0x2F-MUTE ~ 0x7F Maximum
}

/**
 *******************************************************************************
 ** \brief  Configuration Codec to play wav file
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void WM8731_CodecConfigPlay(void)
{
    stc_wm8731_reg_t stcWm8731Reg;
    MEM_ZERO_STRUCT(stcWm8731Reg);
    /* Config codec */
    /* Reset register */
    stcWm8731Reg.RESET              = 0x00u;    // Reset WM8731
    /* Left & right line input */
    stcWm8731Reg.LLIN_f.LINVOL      = 0x00u;    // Left channel line input volume: 0dB--0x17u
    stcWm8731Reg.LLIN_f.LINMUTE     = 1u;       // Enable left channel line input mute
    stcWm8731Reg.LLIN_f.LRINBOTH    = 0u;       // Disable simultaneous input volume and mute load from left to right
    stcWm8731Reg.RLIN_f.RINVOL      = 0x00u;    // Right channel line input volume 0dB
    stcWm8731Reg.RLIN_f.RINMUTE     = 1u;       // Enable right channel line input mute
    stcWm8731Reg.RLIN_f.RINBOTH     = 0u;       // Disable simultaneous input volume and mute load from right to left
    /* Left & right headphone output */
    stcWm8731Reg.LHOUT_f.LHPVOL     = 0x5Fu;    // Set volume of left headphone to 0dB. 0x30(-73dB) ~ 0x7F(+6dB), 0 ~ 0x2F: mute
    stcWm8731Reg.LHOUT_f.LZCEN      = 0u;       // Disable left channel zero cross detect
    stcWm8731Reg.LHOUT_f.LRHPBOTH   = 0u;       // Disable simultaneous output volume and mute load from left to right
    stcWm8731Reg.RHOUT_f.RHPVOL     = 0x5Fu;    // Set volume of right headphone to 0dB. 0x30(-73dB) ~ 0x7F(+6dB), 0 ~ 0x2F: mute
    stcWm8731Reg.RHOUT_f.RZCEN      = 0u;       // Enable right channel zero cross detect
    stcWm8731Reg.RHOUT_f.RLHPBOTH   = 0u;       // Disable simultaneous output volume and mute load from right to left
    /* Analog audio path control */
    stcWm8731Reg.AAPC_f.MICBOOST    = 0u;       // Disable boost
    stcWm8731Reg.AAPC_f.MUTEMIC     = 1u;       // Enable mute to ADC
    stcWm8731Reg.AAPC_f.INSEL       = 0u;       // Line input select to ADC
    stcWm8731Reg.AAPC_f.BYPASS      = 0u;       // Enbale bypass
    stcWm8731Reg.AAPC_f.DACSEL      = 1u;       // Select DAC
    stcWm8731Reg.AAPC_f.SIDETONE    = 0u;       // Disable side tone
    stcWm8731Reg.AAPC_f.SIDEATT     = 0u;       // 0: -6dB, 1: -12dB, 2: -9dB, 3: -15dB.
    /* Digital audio path control */
    stcWm8731Reg.DAPC_f.ADCHPD      = 0u;       // Enable high pass filter
    stcWm8731Reg.DAPC_f.DEEMP       = 3u;       // De-emphasis contrl. 0: disable, 1: 32kHz, 2: 44.1kHz, 3: 48kHz
    stcWm8731Reg.DAPC_f.DACMU       = 0u;       // 0:Disable soft mute   1: Enable soft mute
    stcWm8731Reg.DAPC_f.HPOR        = 0u;       // Clear offset when high pass
    /* Power down control */
    stcWm8731Reg.PDC_f.LINEINPD     = 0u;       // Disable line input power down
    stcWm8731Reg.PDC_f.MICPD        = 0u;       // Disable microphone input power down
    stcWm8731Reg.PDC_f.ADCPD        = 0u;       // Disable ADC power down
    stcWm8731Reg.PDC_f.DACPD        = 0u;       // Disable DAC power down
    stcWm8731Reg.PDC_f.OUTPD        = 0u;       // Disable output power down
    stcWm8731Reg.PDC_f.OSCPD        = 0u;       // Disable oscillator power down
    stcWm8731Reg.PDC_f.CLKOUTPD     = 0u;       // Disable CLKOUT power down
    stcWm8731Reg.PDC_f.POWEROFF     = 0u;       // Disable power off mode
    /* Digital audio interface format */
    stcWm8731Reg.DAIF_f.FORMAT      = 2u;       // 0: MSB-First, right justified, 1: MSB-first, left justified, 2: I2S-format, 3: DSP mode
    stcWm8731Reg.DAIF_f.IWL         = 0u;       // 0: 16 bits, 1: 20 bits, 2: 24 bits, 3: 32 bits
    stcWm8731Reg.DAIF_f.LRP         = 0u;       // 1: right channel DAC data when DACLRC (WS) is high,  0: right channel DAC data when DACLRC (WS) is low
    stcWm8731Reg.DAIF_f.LRSWAP      = 0u;       // 1: swap left channel and right channel, 0: don't swap
    stcWm8731Reg.DAIF_f.MS          = 0u;       // 1: Enable master mode, 0: Enable slave mode
    stcWm8731Reg.DAIF_f.BCLKINV     = 0u;       // Don't invert BCLK
    /* Sampling control */
    stcWm8731Reg.SC_f.NORMAL_USB    = 0u;       // 0: normal mode, 1: USB mode
    stcWm8731Reg.SC_f.BOSR          = 0u;       // Nomrmal mode: 0: 256fs, 1: 384fs
                                                // USB mode: 0: 250fs, 1:272fs
    stcWm8731Reg.SC_f.SR            = 1u;       // Sample rate setting
    stcWm8731Reg.SC_f.CLKDIV2       = 0u;       // 0: core clock is MCLK, 1: core clock is MCLK divided by 2
    stcWm8731Reg.SC_f.CLKODIV2      = 0u;       // 0: output clock is core clock, 1: core clock is core clock/2
    // Active control
    stcWm8731Reg.AC_f.ACTIVE        = 1u;       // 0: inactive, 1: active

    WM8731_Init(I2C_CH, &stcWm8731Reg);

    WM8731_SetHpVolume(I2C_CH, 0x7Fu,0x7Fu);    //0x2F-MUTE ~ 0x7F Maximum
}

/**
 *******************************************************************************
 ** \brief  Initialize LED0 for indicate recoder status
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void IniLedPort(void)
{
    stc_port_init_t stcPortInit;
    /*initiallize LED port*/
    MEM_ZERO_STRUCT(stcPortInit);

    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enExInt = Enable;
    stcPortInit.enPullUp = Enable;

    /* LED0 Port/Pin initialization */
    PORT_Init(LED0_PORT, LED0_PIN, &stcPortInit);
}

/**
 *******************************************************************************
 ** \brief  Initialize DMAC function for recorder
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Record_DmaConfig(void)
{
    stc_dma_config_t stcDmaCfg;
    MEM_ZERO_STRUCT(stcDmaCfg);

    /* Set data block size. */
    stcDmaCfg.u16BlockSize = 1u;  //uint16
    /* Set transfer count. */
    stcDmaCfg.u16TransferCnt = RECORDER_WAVFILELEN;
    /* Set source & destination address. */
    stcDmaCfg.u32SrcAddr = (uint32_t)(&I2S_CH->RXBUF);
    stcDmaCfg.u32DesAddr = (uint32_t)(&au16RecorderSoundI2s[0]);

    /* Disable linked list transfer. */
    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Disable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Disable;
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Disable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma16Bit;

    /* Enable DMA clock. */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA2,Enable);

    /* Enable DMA1. */
    DMA_Cmd(DMA_UNIT,Enable);
    /* Initialize DMA. */
    DMA_InitChannel(DMA_UNIT, DMA_CH, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(DMA_UNIT, DMA_CH,Enable);

    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    DMA_SetTriggerSrc(DMA_UNIT, DMA_CH, EVT_I2S3_RXIRQOUT);
}

/**
 *******************************************************************************
 ** \brief  Initialize DMAC function for play
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Play_DmaConfig(void)
{
    stc_dma_config_t stcDmaCfg;
    MEM_ZERO_STRUCT(stcDmaCfg);

    /* Disable DMA1. */
    DMA_Cmd(DMA_UNIT,Disable);
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH, BlkTrnCpltIrq);
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH, TrnCpltIrq);

    /* Set data block size. */
    stcDmaCfg.u16BlockSize = 1u;  //uint16
    /* Set transfer count. */
    stcDmaCfg.u16TransferCnt = RECORDER_WAVFILELEN;
    /* Set source & destination address. */
    stcDmaCfg.u32SrcAddr = (uint32_t)(&au16RecorderSoundI2s[0]);
    stcDmaCfg.u32DesAddr = (uint32_t)(&I2S_CH->TXBUF);

    /* Disable linked list transfer. */
    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Disable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Disable;
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressIncrease;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressFix;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Disable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma16Bit;

    /* Enable DMA clock. */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA2,Enable);

    /* Enable DMA1. */
    DMA_Cmd(DMA_UNIT,Enable);
    /* Initialize DMA. */
    DMA_InitChannel(DMA_UNIT, DMA_CH, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(DMA_UNIT, DMA_CH,Enable);

    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    DMA_SetTriggerSrc(DMA_UNIT, DMA_CH, EVT_I2S3_TXIRQOUT);
}

/**
 *******************************************************************************
 ** \brief  Get I2S clock frequency.
 **
 ** \param  [in] pstcI2sReg             Pointer to I2S register
 ** \arg    M4_I2S1                     I2s channel 1
 ** \arg    M4_I2S2                     I2s channel 2
 ** \arg    M4_I2S3                     I2s channel 3
 ** \arg    M4_I2S4                     I2s channel 4
 **
 ** \retval uint32_t                    The  I2S clock frequency.
 **
 ** \note   None
 **
 ******************************************************************************/
uint32_t GetI2SClkFreq(M4_I2S_TypeDef* pstcI2sReg)
{
    en_clk_peri_source_t enSrc = ClkPeriSrcPclk;
    uint32_t u32Freq = 0ul;
    stc_clk_freq_t stcClkFreq;
    stc_pll_clk_freq_t stcPllClkFreq;

    /* Check parameters */
    if(NULL != pstcI2sReg)
    {
        enSrc = CLK_GetI2sClkSource(pstcI2sReg);
        CLK_GetClockFreq(&stcClkFreq);
        CLK_GetPllClockFreq(&stcPllClkFreq);
        switch(enSrc)
        {
            case ClkPeriSrcPclk:
                u32Freq = stcClkFreq.pclk3Freq;
                break;
            case ClkPeriSrcMpllp:
                u32Freq = stcPllClkFreq.mpllp;
                break;
            case ClkPeriSrcMpllq:
                u32Freq = stcPllClkFreq.mpllq;
                break;
            case ClkPeriSrcMpllr:
                u32Freq = stcPllClkFreq.mpllr;
                break;
            case ClkPeriSrcUpllp:
                u32Freq = stcPllClkFreq.upllp;
                break;
            case ClkPeriSrcUpllq:
                u32Freq = stcPllClkFreq.upllq;
                break;
            case ClkPeriSrcUpllr:
                u32Freq = stcPllClkFreq.upllr;
                break;
            default:
                break;
        }
    }
    return u32Freq;
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
    stc_i2s_config_t stcI2sCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t stcPortIni;

    MEM_ZERO_STRUCT(stcI2sCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);

    /* Initialize system clock, PLL 168M */
    SysClkIni();
    /* Initialize LED0 */
    IniLedPort();

    /* Initialize i2c port for codec wm8731 */
    PORT_SetFunc(I2C2_SCL_PORT, I2C2_SCL_PIN, Func_I2c2_Scl, Disable);
    PORT_SetFunc(I2C2_SDA_PORT, I2C2_SDA_PIN, Func_I2c2_Sda, Disable);
    /* Enable I2C Peripheral*/
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_I2C2, Enable);

    /* Initialize i2s port for codec wm8731 recorder function */
    MEM_ZERO_STRUCT(stcPortIni);
    stcPortIni.enPullUp = Enable;
    stcPortIni.enPinDrv = Pin_Drv_H;
    PORT_Init(I2S3_PORT, I2S3_CK_PIN, &stcPortIni);
    PORT_Init(I2S3_PORT, I2S3_WS_PIN, &stcPortIni);
    PORT_Init(I2S3_PORT, I2S3_SD_PIN, &stcPortIni);
    PORT_SetFunc(I2S3_PORT, I2S3_CK_PIN, Func_I2s3_Ck, Disable);
    PORT_SetFunc(I2S3_PORT, I2S3_WS_PIN, Func_I2s3_Ws, Disable);
    PORT_SetFunc(I2S3_PORT, I2S3_SD_IN_PIN, Func_I2s3_Sd, Disable);
#if(EXCK_ON)
    PORT_Init(I2S3_EXCK_PORT, I2S3_EXCK_PIN, &stcPortIni);
    PORT_SetFunc(I2S3_EXCK_PORT, I2S3_EXCK_PIN, Func_I2s, Disable);
#else
    PORT_Init(I2S3_MCK_PORT, I2S3_MCK_PIN, &stcPortIni);
    PORT_SetFunc(I2S3_MCK_PORT, I2S3_MCK_PIN, Func_I2s, Disable);
#endif
    /* Enable I2S Peripheral*/
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_I2S3, Enable);

    /* WM8731 configuration for recorder */
    WM8731_CodecConfigRecord();

    /* Config clock source for i2s */
    CLK_SetI2sClkSource(I2S_CH, ClkPeriSrcMpllp);
#if 1
    /* Config i2s peripheral */
    I2s_DeInit(I2S_CH);
    stcI2sCfg.u32I2sInterClkFreq = GetI2SClkFreq(I2S_CH);
    stcI2sCfg.enStandrad = Std_Philips;
    stcI2sCfg.enMode = I2sMaster;
    stcI2sCfg.enChanelLen = I2s_ChLen_32Bit;
    stcI2sCfg.enDataBits = I2s_DataLen_16Bit;
    stcI2sCfg.u32AudioFreq = I2S_AudioFreq_8k;
#if(EXCK_ON)
    stcI2sCfg.enMcoOutEn = Disable;
    stcI2sCfg.enExckEn = Enable;
#else
    stcI2sCfg.enMcoOutEn = Enable;
    stcI2sCfg.enExckEn = Disable;
#endif
    I2s_Init(I2S_CH, &stcI2sCfg);

    /* Config DMAC for recorder*/
    Record_DmaConfig();

#if(SPEAKER_ON)
    /* Initialize SPK_EN port for speaker */
    MEM_ZERO_STRUCT(stcPortIni);
    stcPortIni.enPinMode = Pin_Mode_Out;
    PORT_Init(SPK_EN_PORT, SPK_EN_PIN, &stcPortIni);
    SPEAKER_EN();
#endif
    /* Prepare buffer for store recoder data */
    pu16SoundData = &au16RecorderSoundI2s[0];

    /* Wait for press key SW2 to kick start record */
    while(Reset != PORT_GetBit(PortD, Pin03))
    {
        ;   //SW2
    }
    /* Indicate recording */
    LED0_TOGGLE();

    /* Enable I2S RXI function to kick start */
    I2S_FuncCmd(I2S_CH, RxIntEn, Enable);
    /* Enable i2s RX function */
    I2S_FuncCmd(I2S_CH, RxEn, Enable);

    /* Wait for recorder buffer full */
    while(Set != DMA_GetIrqFlag(DMA_UNIT,DMA_CH, TrnCpltIrq))
    {
        ;
    }
    I2s_DeInit(I2S_CH);
    /* Indicate record finished */
    LED0_TOGGLE();

    /* Wait for press key SW2 to play recoder wav */
    while(Reset != PORT_GetBit(PortD, Pin03))
    {
        ;   //SW2
    }

#endif
    /* Buffer prepare for play */
    pu16SoundData = (uint16_t *)&au16RecorderSoundI2s[0];
    u32Count = 0ul;
    /* Config I2S_SD IO for play */
    PORT_SetFunc(I2S3_PORT, I2S3_SD_IN_PIN, Func_Gpio, Disable);
    PORT_SetFunc(I2S3_PORT, I2S3_SD_PIN, Func_I2s3_Sd, Disable);
    /* Configuration Codec WM8731 to play wav file */
    WM8731_CodecConfigPlay();

    /* Config DMAC for play */
    Play_DmaConfig();

    /* Config i2s peripheral */
    I2s_DeInit(I2S_CH);
    stcI2sCfg.u32I2sInterClkFreq = GetI2SClkFreq(I2S_CH);
    stcI2sCfg.enStandrad = Std_Philips;
    stcI2sCfg.enMode = I2sMaster;
    stcI2sCfg.enChanelLen = I2s_ChLen_32Bit;
    stcI2sCfg.enDataBits = I2s_DataLen_16Bit;
    stcI2sCfg.u32AudioFreq = I2S_AudioFreq_8k;
#if(EXCK_ON)
    stcI2sCfg.enMcoOutEn = Disable;
    stcI2sCfg.enExckEn = Enable;
#else
    stcI2sCfg.enMcoOutEn = Enable;
    stcI2sCfg.enExckEn = Disable;
#endif
    I2s_Init(I2S_CH, &stcI2sCfg);

    /* Enable i2s TX function */
    I2S_FuncCmd(I2S_CH, TxEn, Enable);
    /* Enable I2S TXI function to kick start conmmunication */
    I2S_FuncCmd(I2S_CH, TxIntEn, Enable);
    /* Wait for DMA finished */
    while(Set != DMA_GetIrqFlag(DMA_UNIT,DMA_CH, TrnCpltIrq))
    {
        ;
    }

    while(1)
    {
        ;
    }

}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

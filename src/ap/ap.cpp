#include "ap.h"
#include "bsp.h"
#include "cli.h"
#include "reset.h"
#include "modules.h"

bool is_boot_mode = true;

cmd_t cmd_boot;

void cliInfo(cli_args_t *args);
void cliBoot(cli_args_t *args);

static bool is_run_fw     = true;
static bool is_update_fw  = false;

void apInit(void)
{
  uint32_t boot_param;
  uint16_t err_code;

  boot_param = resetGetBootMode();

  if ( boot_param == MODE_BIT_BOOT )
  {
    boot_param &= ~MODE_BIT_BOOT;
    for ( int i=0; i <5; i++)
    {
      ledOn(HW_LED_CH_DOWN);
      delay(100);
    }
    is_run_fw = false;
  }

  if ( boot_param == MODE_BIT_UPDATE )
  {
    boot_param &= ~MODE_BIT_UPDATE;
    is_run_fw = true;
    is_update_fw =true;    
  }

  logPrintf("\n");

  if ( is_update_fw )
  {
    // =============== LCD 
    // if (lcdIsInit())
    // {
    //   lcdClearBuffer(black);
    //   lcdPrintf(0, 8, white, " Update Firm..");
    //   lcdUpdateDraw();
    // }

    logPrintf("[  ] bootUpdateFirm");
    err_code = bootUpdateFirm();
    logPrintf("[ EC ] err_code =%04x %d\r\n", err_code, err_code );
    if ( err_code == CMD_OK )
      logPrintf("[OK]\r\n");
    else 
      logPrintf("[E_] err : 0x%04X\r\n", err_code);
  }
  if ( is_run_fw )
  {
    // =============== LCD 
    // if (lcdIsInit())
    // {
    //   lcdClearBuffer(black);
    //   lcdPrintf(0, 8, white, " Jump Firm...");
    //   lcdUpdateDraw();
    // }    

    switch( resetGetCount() )
    {
      case 2:
      {
        is_boot_mode = true;
        // cmdInit(&cmd_boot);
        // cmdOpen(&cmd_boot, _DEF_UART1, 115200);

        cmdTaskInit();
        logPrintf("BOOT Mode ..\n\n");
      }
      break;

      case 3:
      {
        is_boot_mode = false;
        cliOpen(_DEF_UART1, 115200);

        cliAdd("info", cliInfo);
        cliAdd("boot", cliBoot);

        logPrintf("CLI Mode ..\n\n");
      }
      break;

      default:
      {
        err_code = bootJumpFirm();
        logPrintf("[2 EC ] err_code =%04x %d\r\n",err_code,err_code );
        if ( err_code != CMD_OK )
        {
          logPrintf("[  ] bootJumpFirm()\r\n");
          logPrintf("[E ] err : 0x%04x\r\n", err_code);
          // if ( bootUpdateTagVerify() == OK)
          if ( bootVerifyUpdate() == CMD_OK )
          {
            logPrintf("[  ] retry update\n");
            if (bootUpdateFirm() == CMD_OK )
            {
              err_code = bootJumpFirm();
              if (err_code != CMD_OK )
                logPrintf("[E_] err : 0x%04X\r\n", err_code);
            }
          }
        }
      }
      break;
      
    }
  }

}

extern bool cmdTaskIsRunning ;
void apMain(void)
{
  uint32_t pre_time;
  uint32_t led_time = 100;
  bool is_retry_update_fw = false;
  uint32_t retry_pre_time;

  // cmdTaskInit();

  if (is_boot_mode != true)
  {
    led_time = 50;
  }

  retry_pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= led_time)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }

    if ( is_boot_mode == true)
    {
      cmdTaskUpdate();
    }
    else 
    {
      if ( cliMain() == true )
        retry_pre_time = millis();
    }

    // Firmware Recovery
    if (millis()-retry_pre_time >= (15*1000))
    {
      if (is_retry_update_fw == false)
      {
        uint16_t err_code;
        // is_retry_update_fw = true;
        // bootJumpToFw();
        
        // if (bootUpdateTagVerify() == OK && bootUpdateFw() == OK)
        // {
        //   bootJumpToFw();
        // }
        err_code = bootJumpFirm();
        logPrintf("[2 EC ] err_code =%04x %d\r\n",err_code,err_code );
        if ( err_code != CMD_OK ) {
          logPrintf("[  ] bootJumpFirm()\r\n");
          logPrintf("[E ] err : 0x%04x\r\n", err_code);
          // if ( bootUpdateTagVerify() == OK)
          if ( bootVerifyUpdate() == CMD_OK )
          {
            logPrintf("[  ] retry update\n");
            if (bootUpdateFirm() == CMD_OK )
            {
              err_code = bootJumpFirm();
              if (err_code != CMD_OK )
                logPrintf("[E_] err : 0x%04X\r\n", err_code);
            }
          }
        }
      }

    }

  }

}

void cliInfo(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "flash"))
  {
    extern const uint32_t __flash_binary_start;
    extern const uint32_t __flash_binary_end;

    cliPrintf("Flash FW Start : 0x%X\n", &__flash_binary_start);
    cliPrintf("Flash FW End   : 0x%X\n", &__flash_binary_end);
    cliPrintf("Flash FW Size  : %d KB\n", ((uint32_t)&__flash_binary_end - (uint32_t)&__flash_binary_start)/1024);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "jump"))
  {
    cliPrintf("JumpToFw()\n");
    delay(50);

    jumpToFw();
    ret = true;
  }


  if (ret != true)
  {
    cliPrintf("info flash\n");
  }
}

void cliBoot(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "jump"))
  {
    cliPrintf("jump to fw\n");
    delay(50);

    jumpToFw();
    ret = true;
  }


  if (ret != true)
  {
    cliPrintf("boot jump\n");
  }
}
#include "readUartCommand.h"

#define BAUDRATE 57600
#define UART USART2
#define USART_TX_Pin GPIO_PIN_2
#define USART_RX_Pin GPIO_PIN_3

osjob_t initjob;
osjob_t readAt;

RTC_HandleTypeDef hrtc; 

ReadUartCommand reader; 

char * command = nullptr;

extern "C" void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Enable Power Control clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

extern "C" void RTC_Init(void)
{
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  __HAL_RCC_RTC_ENABLE();

  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* RTC interrupt Init */
  HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_IRQn);
}

//Interrupt handlers
extern "C" void RTC_IRQHandler(void){HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);}

extern "C" void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc){
  HAL_PWR_DisableSleepOnExit();
}

static void initfunc(osjob_t* j){
  //Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();
  //Configure the system clock
  SystemClock_Config();
  //init RTC
  RTC_Init();
  //initializes serial communication and mac parameters
  reader.begin(BAUDRATE, &hrtc);
}

static void readAtfunc(osjob_t* j){
  //read command from serial port with default baudrate = 57600
  command = reader.getCommand();
  //parse command
  reader.parseCommand(command);
}

void setup(void){
  //initialize run-time environment
  os_init();
  //run initfunc
  os_setCallback(&initjob, initfunc);
}

void loop(void){
  //schedule a read command if there is data available
  if(Serial.available()>0){
    //give time to fill buffer with full command and then read it
    //since we dont know the length of the command
    delay(100);
    os_setCallback(&readAt, readAtfunc);
  }
  //execute scheduled jobs and events
  os_runloop_once();
}
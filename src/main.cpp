#include "readUartCommand.h"

#define BAUDRATE 57600
#define UART USART2
#define USART_TX_Pin GPIO_PIN_2
#define USART_RX_Pin GPIO_PIN_3
#define RTCint RTC_IRQn
#define UARTint USART2_IRQn

#define MAX_LENGTH_MESSAGE 127

osjob_t initjob;
osjob_t readAt;

RTC_HandleTypeDef hrtc; 
UART_HandleTypeDef huart;

ReadUartCommand reader; 

uint8_t uart_buffer[MAX_LENGTH_MESSAGE] = {0};
uint8_t buffer_index = 0;

bool commandAvailable = false;

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV4;
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
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

extern "C" void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

   /**USART2 GPIO Configuration
  PA2     ------> USART2_TX
  PA3     ------> USART2_RX
  */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
  HAL_NVIC_SetPriority(RTCint, 0, 0);
  HAL_NVIC_EnableIRQ(RTCint);
}

extern "C" void UART_Init(void)
{
  huart.Instance = UART;
  huart.Init.BaudRate = 57600;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits = UART_STOPBITS_1;
  huart.Init.Parity = UART_PARITY_NONE;
  huart.Init.Mode = UART_MODE_TX_RX;
  huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart.Init.OverSampling = UART_OVERSAMPLING_16;
  huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  //bugs when using auto baud rate
  //huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_AUTOBAUDRATE_INIT;
  //huart.AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_AUTOBAUDRATE_ENABLE;
  //huart.AdvancedInit.AutoBaudRateMode = UART_ADVFEATURE_AUTOBAUDRATE_ON0X55FRAME;

  __HAL_RCC_USART2_CLK_ENABLE();

  if (HAL_UART_Init(&huart) != HAL_OK)
  {
    Error_Handler();
  }

  /* USART2 interrupt Init */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

//Interrupt handlers
extern "C" void RTC_IRQHandler(void){HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);}
extern "C" void USART2_IRQHandler(void){HAL_UART_IRQHandler(&huart);}

//Interupt implementations
extern "C" void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc){
  if(counter > 0){
    counter--;
    sleepTime -= 32000;
    if(sleepTime < 32000){
      HAL_RTCEx_SetWakeUpTimer_IT(hrtc, sleepTime/WAKE_UP_BASE_TIME, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    }else{
      HAL_RTCEx_SetWakeUpTimer_IT(hrtc, 0xFFFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    }
  }else{
    HAL_RTCEx_DeactivateWakeUpTimer(hrtc);
    HAL_PWR_DisableSleepOnExit();
  }
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *_huart)
{ 
  //added wake up by uart interrupt
  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
  HAL_PWR_DisableSleepOnExit();
  
  if(uart_buffer[buffer_index] == '\n' || buffer_index == MAX_LENGTH_MESSAGE){
    uart_buffer[buffer_index] = '\0';
    commandAvailable = true;
    buffer_index = 0;
  }else{
    buffer_index++;
  }
  HAL_UART_Receive_IT(_huart, (uint8_t *)&uart_buffer[buffer_index], 1);
}

static void initfunc(osjob_t* j){
  //Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();
  //Configure the system clock
  SystemClock_Config();
  //init GPIO
  GPIO_Init();
  //init UART
  UART_Init();
  //init RTC
  RTC_Init();
  //initializes serial communication and mac parameters
  reader.begin(BAUDRATE, &huart, &hrtc);
  //Enable UART interrupt when data is received
  HAL_UART_Receive_IT(&huart, (uint8_t *)&uart_buffer[buffer_index], 1);
}

static void parseCommand(osjob_t* j){
  reader.parseCommand((char*)uart_buffer);
}

void setup(void){
  //initialize LMIC run-time environment
  os_init();
  //run init functions
  os_setCallback(&initjob, initfunc);
}

void loop(void){
  if(commandAvailable){
    os_setCallback(&readAt, parseCommand);
    commandAvailable = false;
  }
  //execute scheduled jobs and events
  os_runloop_once();
}
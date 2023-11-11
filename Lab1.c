// Программа для ознакомления с модулями RCC, GPIO, TIM, MISC

// Реализуется мигание светодиода, подключенного к выводу PC15,
// с задержкой 500 мс на таймере TIM14;

#include "stm32_lib/inc/stm32f0xx_rcc.h"
#include "stm32_lib/inc/stm32f0xx_gpio.h"
#include "stm32_lib/inc/stm32f0xx_tim.h"
#include "stm32_lib/inc/stm32f0xx_misc.h"

void TimConfg(void) // Настройка таймера тактирования
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); 
  // Включаем тактирование TIM14
  
    TIM_TimeBaseInitTypeDef Tim;
    NVIC_InitTypeDef NVIC_I;

    Tim.TIM_ClockDivision = TIM_CKD_DIV1; 
// Выключаем предварительный делитель частоты таймера.
    Tim.TIM_CounterMode   = TIM_CounterMode_Up; 
// Значение в счетном регистре увеличивается.
    Tim.TIM_Prescaler     = (SystemCoreClock / 1000) - 1; 
// Делим так чтобы на таймер шел 1 КГц.
    Tim.TIM_Period        = 499; 
// Таймер будет генерирвать событие 2 раза в секунду.
    Tim.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM14, &Tim); // Инициализация таймера.

    // Настройка прерывания от таймера.
    NVIC_I.NVIC_IRQChannel = TIM14_IRQn;
    NVIC_I.NVIC_IRQChannelPriority = 3; 
// Приоритет прерывания (0 - 3. 0 - наивысший, 3 - наименьший).
    NVIC_I.NVIC_IRQChannelCmd = ENABLE; 
// Разрешаем прерывание.
    NVIC_Init(&NVIC_I);

    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE); 
// Разрешаем прерывание таймера по переполнению.

    TIM_Cmd(TIM14, ENABLE); // Запуск таймера.
}

// Функция инвертирует состояние вывода.
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	   /* Check the parameters */
	   assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

	   GPIOx->ODR ^= GPIO_Pin;
}

void TIM14_IRQHandler(void) // Обработчик прерывания от таймера
{
    GPIO_ToggleBits(GPIOB, GPIO_Pin_15);   
// Мигаем светодиодом
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update); 
// Сброс флага прерывания по переполнению таймера
}


int main(void)
{
	SystemInit();

	//Назначение порта 15 как выхода
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);

	GPIO_InitTypeDef ledinit;
	GPIO_StructInit(&ledinit);
	ledinit.GPIO_Pin = GPIO_Pin_15;
	ledinit.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &ledinit);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);

	TimConfg(); // Вызов функции настройки таймера

    while(1)
    {
    }
}

// Программа считывает сигнал задающего генератора
// с вывода PA1 и формирует сигнал, пропорциональный заданному,
// используя ШИМ с частотой (9±0.45) кГц на выводе PB0 аппаратно, а на выводе PB1 программно.
// Для преобразования аналогового задающего сигнала в цифровой используется модуль ADC.

#include "stm32_lib/inc/stm32f0xx_rcc.h"
#include "stm32_lib/inc/stm32f0xx_gpio.h"
#include "stm32_lib/inc/stm32f0xx_tim.h"
#include "stm32_lib/inc/stm32f0xx_misc.h"
#include "stm32_lib/inc/stm32f0xx_adc.h"

// Параметр для определения скважности ШИМ
volatile uint16_t  PWM_val = 10;

// Аппаратный выход PB0
void GPIO_Ap_SHIM(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);

	GPIO_InitTypeDef ledinit_AF;
	GPIO_StructInit(&ledinit_AF);

	ledinit_AF.GPIO_Pin = GPIO_Pin_0;
	ledinit_AF.GPIO_Mode = GPIO_Mode_AF;
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_1);
	GPIO_Init(GPIOB, &ledinit_AF);
}

// Программный выход PB1
void GPIO_Progr_SHIM(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);

	GPIO_InitTypeDef ledinit;
	GPIO_StructInit(&ledinit);

	ledinit.GPIO_Pin = GPIO_Pin_1;
	ledinit.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &ledinit);
}

// Аналоговый вход PA1
void GPIO_Out(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// Конфигурация ШИМ аппаратная
void SHIM_Conf_AP(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// Инициализация переменной для настройки прерывания
	NVIC_InitTypeDef NVIC_I;

	// Настройка переполнения таймера
	TIM_TimeBaseInitTypeDef timer_init;
	TIM_TimeBaseStructInit(&timer_init);

	timer_init.TIM_Period = 111 - 1;
	timer_init.TIM_Prescaler = (SystemCoreClock / 1000) - 1;
	TIM_TimeBaseInit(TIM3, &timer_init);

	// Задание скважности ШИМ
	TIM_OCInitTypeDef tim_OC_init;
	TIM_OCStructInit(&tim_OC_init);

	tim_OC_init.TIM_Pulse = PWM_val;
	tim_OC_init.TIM_OCMode = TIM_OCMode_PWM1;
	tim_OC_init.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM3, &tim_OC_init);

	// Настройка прерывания от таймера
	NVIC_I.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_I.NVIC_IRQChannelPriority = 3; // Приоритет прерывания (0 - 3. 0 - наивысший, 3 - наименьший).
    NVIC_I.NVIC_IRQChannelCmd = ENABLE; // Разрешаем прерывание
	NVIC_Init(&NVIC_I);

	// Включаем генерацию прерывания по переполнению
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	// Включаем таймер
	TIM_Cmd(TIM3, ENABLE);
}

// Конфигурация ШИМ программная
void SHIM_Conf_PROG(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// Инициализация переменной для настройки прерывания
	NVIC_InitTypeDef NVIC_I;

	// Настройка переполнения таймера
	TIM_TimeBaseInitTypeDef timer_init;
	TIM_TimeBaseStructInit(&timer_init);

	timer_init.TIM_Period = 111 - 1;
	timer_init.TIM_Prescaler = (SystemCoreClock / 1000) - 1;
	TIM_TimeBaseInit(TIM3, &timer_init);

	// Задание скважности ШИМ
	TIM_OCInitTypeDef tim_OC_init;
	TIM_OCStructInit(&tim_OC_init);

	tim_OC_init.TIM_Pulse = PWM_val;
	tim_OC_init.TIM_OCMode = TIM_OCMode_PWM1;
	tim_OC_init.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM3, &tim_OC_init);

	// Настройка прерывания от таймера
	NVIC_I.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_I.NVIC_IRQChannelPriority = 3; // Приоритет прерывания (0 - 3. 0 - наивысший, 3 - наименьший).
	NVIC_I.NVIC_IRQChannelCmd = ENABLE; // Разрешаем прерывание
	NVIC_Init(&NVIC_I);

	// Включаем генерацию прерывания по переполнению
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	// Генератор прерывания по событию сравнения
	TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);

	// Включаем таймер
	TIM_Cmd(TIM3, ENABLE);
}

// Переопределение обработчика прерывания ШИМ
// для программной реализации
void TIM3_IRQHandler()
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM_SetCompare1(TIM3,PWM_val);
		if (PWM_val > 0) GPIO_SetBits(GPIOB, GPIO_Pin_1);
	}
	if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
		if (PWM_val < 109) GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		}
}

// Переопределение обработчика прерывания ШИМ
// для аппаратной реализации
//void TIM3_IRQHandler()
//{
//	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//	TIM_SetCompare1(TIM3,PWM_val);
//}

// Настройка модуля АЦП
void ADC_Conf(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// Создание структуры АЦП
	ADC_InitTypeDef ADC_InitStructure;
	// Инициализация структуры АЦП
	ADC_StructInit(&ADC_InitStructure);
	// Загрузка настроек по умолчанию
	ADC_Init(ADC1, &ADC_InitStructure);
	// Включаем прерывание по окончанию преобразования
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	// Включаем модуль
	ADC_Cmd(ADC1, ENABLE);
	// Разрешаем прерывание
	NVIC_EnableIRQ(ADC1_IRQn);
	// Загрузка конфигурации
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_7_5Cycles);
	//Запуск преобразования
	ADC_StartOfConversion(ADC1);
}

// Переопределение обработчика прерывания АЦП
void ADC_IRQHandler()
{
  if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
  {
     ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
     PWM_val = (110 * (ADC_GetConversionValue(ADC1)))/ 4095;
     ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_239_5Cycles);
     ADC_StartOfConversion(ADC1);
   }
}

int main(void)
{
	SystemInit();

	GPIO_Progr_SHIM();
	GPIO_Out();
	SHIM_Conf_PROG();
	ADC_Conf();

    while(1)
    {
    }
}

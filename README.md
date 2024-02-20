# Программы для работы со стендом микроконтроллера STM32F0
Лабораторная 1(Lab1.c):
Реализация мигания светодиода, подключенного к выводу PC15, с задержкой 500 миллисекунд на таймере TIM14.

Лабораторная 2(Lab2.c):
Программа считывает задающий сигнал(sin) с генератора на выводе PA1 и формирует по данному сигналу ШИМ с частотой 9кГц для управления ДПТ.

Модель МК: STM32F030K6T6

Datasheed STM32F0:
https://www.farnell.com/datasheets/2554494.pdf

Reference Manual:
https://www.st.com/resource/en/reference_manual/dm00091010-stm32f030x4-x6-x8-xc-and-stm32f070x6-xb-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf

В качестве среды разработки используется CoIDE.

`
const func = (num) => {  
  if (num > 0) {  
    return num - 1;  
  }
  return num + 1;  
}; 
`

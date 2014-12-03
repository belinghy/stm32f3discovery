// -------------------------------------------------
// Content: This file contains EXTI0_IRQHandler, which contain the user button.
//
// -------------------------------------------------

#include "cmsis_device.h"
#include "stm32f3_discovery.h"

extern __IO uint32_t UserButtonPressed;
__IO uint32_t i = 0;

void EXTI0_IRQHandler(void)
{
  if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
  {
    /* Delay */
    for(i=0; i<0x7FFFF; i++);

    /* Wait for SEL button to be pressed  */
    while(STM_EVAL_PBGetState(BUTTON_USER) != RESET);
    /* Delay */
    for(i=0; i<0x7FFFF; i++);
    UserButtonPressed++;

    if (UserButtonPressed > 0x2)
    {
      UserButtonPressed = 0x0;
    }

    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }
}

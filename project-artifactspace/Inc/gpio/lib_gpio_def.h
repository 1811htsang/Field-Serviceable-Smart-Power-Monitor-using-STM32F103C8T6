/*
 * lib_gpio_def.h
 *
 *  Created on: Feb 10, 2026
 *      Author: shanghuang
 */

#ifndef LIB_GPIO_DEF_H_
  #define LIB_GPIO_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
    #else
      #include "lib_keyword_def.h"
    #endif

  // Khai báo địa chỉ ngoại vi GPIO

    #define GPIOA_REGS_BASEADDR 0x40010800ul
    #define GPIOB_REGS_BASEADDR 0x40010C00ul
    #define GPIOC_REGS_BASEADDR 0x40011000ul
    #define GPIOD_REGS_BASEADDR 0x40011400ul
    #define GPIOE_REGS_BASEADDR 0x40011800ul
    #define GPIOF_REGS_BASEADDR 0x40011C00ul
    #define GPIOG_REGS_BASEADDR 0x40012000ul    

  // Khai báo bộ thanh ghi của ngoại vi GPIO

    tdf_strc GPIO_REGS_Typedef {
      __vo BLANK_REG_32B GPIO_CRL;        // Offset 0x00, reset 0x4444 4444
      __vo BLANK_REG_32B GPIO_CRH;        // Offset 0x04, reset 0x4444 4444
      __vo BLANK_REG_32B GPIO_IDR;        // Offset 0x08, reset 0x0000 XXXX
      __vo BLANK_REG_32B GPIO_ODR;        // Offset 0x0C, reset 0x0000 0000
      __vo BLANK_REG_32B GPIO_BSRR;       // Offset 0x10, reset 0x0000 0000
      __vo BLANK_REG_32B GPIO_BRR;        // Offset 0x14, reset 0x0000 0000
      __vo BLANK_REG_32B GPIO_LCKR;       // Offset 0x18, reset 0x0000 0000
    } GPIO_REGS_Typedef;

	// >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define GPIOA_REGS_PTR ((GPIO_REGS_Typedef *) GPIOA_REGS_BASEADDR)
      #define GPIOB_REGS_PTR ((GPIO_REGS_Typedef *) GPIOB_REGS_BASEADDR)
      #define GPIOC_REGS_PTR ((GPIO_REGS_Typedef *) GPIOC_REGS_BASEADDR)
      #define GPIOD_REGS_PTR ((GPIO_REGS_Typedef *) GPIOD_REGS_BASEADDR)
      #define GPIOE_REGS_PTR ((GPIO_REGS_Typedef *) GPIOE_REGS_BASEADDR)
      #define GPIOF_REGS_PTR ((GPIO_REGS_Typedef *) GPIOF_REGS_BASEADDR)
      #define GPIOG_REGS_PTR ((GPIO_REGS_Typedef *) GPIOG_REGS_BASEADDR)
    #else
      extern GPIO_REGS_Typedef MOCK_GPIOA_REGS;
      extern GPIO_REGS_Typedef MOCK_GPIOB_REGS;
      extern GPIO_REGS_Typedef MOCK_GPIOC_REGS;
      extern GPIO_REGS_Typedef MOCK_GPIOD_REGS;
      extern GPIO_REGS_Typedef MOCK_GPIOE_REGS;
      extern GPIO_REGS_Typedef MOCK_GPIOF_REGS;
      extern GPIO_REGS_Typedef MOCK_GPIOG_REGS;

      #define GPIOA_REGS_PTR (&MOCK_GPIOA_REGS)
      #define GPIOB_REGS_PTR (&MOCK_GPIOB_REGS)
      #define GPIOC_REGS_PTR (&MOCK_GPIOC_REGS)
      #define GPIOD_REGS_PTR (&MOCK_GPIOD_REGS)
      #define GPIOE_REGS_PTR (&MOCK_GPIOE_REGS)
      #define GPIOF_REGS_PTR (&MOCK_GPIOF_REGS)
      #define GPIOG_REGS_PTR (&MOCK_GPIOG_REGS)
    #endif

	// Khai báo các định nghĩa bit cần sử dụng trên thanh ghi GPIOx_CRL và GPIOx_CRH

    #define GPIO_CNF_INPUT_ANALOG        0x00ul
    #define GPIO_CNF_INPUT_FLOATING      0x01ul
    #define GPIO_CNF_INPUT_PU_PD         0x02ul

    #define GPIO_CNF_OUTPUT_PP           0x00ul
    #define GPIO_CNF_OUTPUT_OD           0x01ul

    #define AFIO_OUTPUT_PP               0x10ul
    #define AFIO_OUTPUT_OD               0x11ul

    #define GPIO_MODE_INPUT              0x00ul

    #define GPIO_MODE_OUTPUT_10MHz       0x01ul
    #define GPIO_MODE_OUTPUT_2MHz        0x02ul
    #define GPIO_MODE_OUTPUT_50MHz       0x03ul

    #define GPIO_CNF_MODE_MASK           0x0Ful // 11 for CNF + 11 for MODE = 1111 = 15
    #define GPIO_CNF_MODE_RESET          0x04ul // 01 for CNF + 00 for MODE = 0100 = 4

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi GPIOx_ODR

    #define GPIO_DATA_RESET              RESET

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi GPIOx_BSRR

    #define GPIO_BSRR_SET                SET
    #define GPIO_BSRR_RESET              RESET

    /**
     * Ghi chú:
     * Trong thanh ghi gồm 2 nhóm BRy và BSy,
     * Nếu cả 2 nhóm đều được ghi cùng 1 lúc 
     * thì BSy có ưu tiên cao hơn BRy.
     */

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi GPIOx_BRR

    #define GPIO_BRR_RESET_ENABLE        SET
    #define GPIO_BRR_RESET_DISABLE       RESET

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi GPIOx_LCKR

    #define GPIO_LCKR_LCKK_ENABLE        SET
    #define GPIO_LCKR_LCKK_DISABLE       RESET

    #define GPIO_LCKR_LCKK_POSITION      16u
    #define GPIO_LCKR_LCKK_MASK          (1u << GPIO_LCKR_LCKK_POSITION)

  // Khi báo các chân GPIO

    #define GPIO_PIN_0                   ((ui16)0x0001u)  /* Pin 0 selected    */
    #define GPIO_PIN_1                   ((ui16)0x0002u)  /* Pin 1 selected    */
    #define GPIO_PIN_2                   ((ui16)0x0004u)  /* Pin 2 selected    */
    #define GPIO_PIN_3                   ((ui16)0x0008u)  /* Pin 3 selected    */
    #define GPIO_PIN_4                   ((ui16)0x0010u)  /* Pin 4 selected    */
    #define GPIO_PIN_5                   ((ui16)0x0020u)  /* Pin 5 selected    */
    #define GPIO_PIN_6                   ((ui16)0x0040u)  /* Pin 6 selected    */
    #define GPIO_PIN_7                   ((ui16)0x0080u)  /* Pin 7 selected    */
    #define GPIO_PIN_8                   ((ui16)0x0100u)  /* Pin 8 selected    */
    #define GPIO_PIN_9                   ((ui16)0x0200u)  /* Pin 9 selected    */
    #define GPIO_PIN_10                  ((ui16)0x0400u)  /* Pin 10 selected   */
    #define GPIO_PIN_11                  ((ui16)0x0800u)  /* Pin 11 selected   */
    #define GPIO_PIN_12                  ((ui16)0x1000u)  /* Pin 12 selected   */
    #define GPIO_PIN_13                  ((ui16)0x2000u)  /* Pin 13 selected   */
    #define GPIO_PIN_14                  ((ui16)0x4000u)  /* Pin 14 selected   */
    #define GPIO_PIN_15                  ((ui16)0x8000u)  /* Pin 15 selected   */
    
    #define GPIO_PIN_ALL                 ((ui16)0xFFFFu)  /* All pins selected */
    #define GPIO_PIN_MASK                0xFFFFu  /* PIN mask for assert test */

  // Khai báo các chế độ GPIO 

    #define GPIO_MODE_INPUT_ANALOG       0x0000u // equivalent to 00 CNF + 00 MODE = 0000 = 0
    #define GPIO_MODE_INPUT_FLOATING     0x0004u // equivalent to 01 CNF + 00 MODE = 0100 = 4
    #define GPIO_MODE_INPUT_PU_PD        0x0008u // equivalent to 10 CNF + 00 MODE = 1000 = 8
    #define GPIO_MODE_OUTPUT_10MHz_PP    0x0001u // equivalent to 00 CNF + 01 MODE = 0001 = 1
    #define GPIO_MODE_OUTPUT_10MHz_OD    0x0005u // equivalent to 01 CNF + 01 MODE = 0101 = 5
    #define GPIO_MODE_OUTPUT_2MHz_PP     0x0002u // equivalent to 00 CNF + 10 MODE = 0010 = 2
    #define GPIO_MODE_OUTPUT_2MHz_OD     0x0006u // equivalent to 01 CNF + 10 MODE = 0110 = 6
    #define GPIO_MODE_OUTPUT_50MHz_PP    0x0003u // equivalent to 00 CNF + 11 MODE = 0011 = 3
    #define GPIO_MODE_OUTPUT_50MHz_OD    0x0007u // equivalent to 01 CNF + 11 MODE = 0111 = 7

  // Khai báo các chế độ AFIO

    #define AFIO_MODE_OUTPUT_10MHz_PP    0x0009u // equivalent to 10 CNF + 01 MODE = 1001 = 9
    #define AFIO_MODE_OUTPUT_10MHz_OD    0x000Du // equivalent to 11 CNF + 01 MODE = 1101 = 13
    #define AFIO_MODE_OUTPUT_2MHz_PP     0x000Au // equivalent to 10 CNF + 10 MODE = 1010 = 10
    #define AFIO_MODE_OUTPUT_2MHz_OD     0x000Eu // equivalent to 11 CNF + 10 MODE = 1110 = 14
    #define AFIO_MODE_OUTPUT_50MHz_PP    0x000Bu // equivalent to 10 CNF + 11 MODE = 1011 = 11
    #define AFIO_MODE_OUTPUT_50MHz_OD    0x000Fu // equivalent to 11 CNF + 11 MODE = 1111 = 15

  // Khai báo cấu hình chế độ PULL 

    #define GPIO_NOPULL                  0xF0u
    #define GPIO_PULLUP                  0xF1u
    #define GPIO_PULLDOWN                0xF2u

  // Khai báo cấu hình trigger ngắt ngoài

    #define GPIO_TRIGGER_NONE            0xA0u
    #define GPIO_TRIGGER_RISING          0xA1u
    #define GPIO_TRIGGER_FALLING         0xA2u
    #define GPIO_TRIGGER_BOTH            0xA3u

#endif /* LIB_GPIO_DEF_H_ */

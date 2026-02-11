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

  // Khai báo địa chỉ ngoại vi AFIO

    #define AFIO_REGS_BASEADDR  0x40010000ul

  // Khai báo địa chỉ ngoại vi GPIO

    #define GPIOA_REGS_BASEADDR 0x40010800ul
    #define GPIOB_REGS_BASEADDR 0x40010C00ul
    #define GPIOC_REGS_BASEADDR 0x40011000ul
    #define GPIOD_REGS_BASEADDR 0x40011400ul
    #define GPIOE_REGS_BASEADDR 0x40011800ul
    #define GPIOF_REGS_BASEADDR 0x40011C00ul
    #define GPIOG_REGS_BASEADDR 0x40012000ul    

	// Khai báo bộ thanh ghi của ngoại vi AFIO

    tdf_strc AFIO_REGS_Typedef {
      __vo BLANK_REG AFIO_EVCR;       // Offset 0x00
      __vo BLANK_REG AFIO_MAPR;       // Offset 0x04
      __vo BLANK_REG AFIO_EXTICR1;    // Offset 0x08
      __vo BLANK_REG AFIO_EXTICR2;    // Offset 0x0C
      __vo BLANK_REG AFIO_EXTICR3;    // Offset 0x10
      __vo BLANK_REG AFIO_EXTICR4;    // Offset 0x14
      __vo BLANK_REG RESERVED0;       // Offset 0x18
      __vo BLANK_REG AFIO_MAPR2;      // Offset 0x1c
    } AFIO_REGS_Typedef;

  // Khai báo bộ thanh ghi của ngoại vi GPIO

    tdf_strc GPIO_REGS_Typedef {
      __vo BLANK_REG GPIO_CRL;        // Offset 0x00
      __vo BLANK_REG GPIO_CRH;        // Offset 0x04
      __vo BLANK_REG GPIO_IDR;        // Offset 0x08
      __vo BLANK_REG GPIO_ODR;        // Offset 0x0C
      __vo BLANK_REG GPIO_BSRR;       // Offset 0x10
      __vo BLANK_REG GPIO_BRR;        // Offset 0x14
      __vo BLANK_REG GPIO_LCKR;       // Offset 0x18
    } GPIO_REGS_Typedef;

	// >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define AFIO_REGS_PTR ((AFIO_REGS_Typedef *) AFIO_REGS_BASEADDR)

      #define GPIOA_REGS_PTR ((GPIO_REGS_Typedef *) GPIOA_REGS_BASEADDR)
      #define GPIOB_REGS_PTR ((GPIO_REGS_Typedef *) GPIOB_REGS_BASEADDR)
      #define GPIOC_REGS_PTR ((GPIO_REGS_Typedef *) GPIOC_REGS_BASEADDR)
      #define GPIOD_REGS_PTR ((GPIO_REGS_Typedef *) GPIOD_REGS_BASEADDR)
      #define GPIOE_REGS_PTR ((GPIO_REGS_Typedef *) GPIOE_REGS_BASEADDR)
      #define GPIOF_REGS_PTR ((GPIO_REGS_Typedef *) GPIOF_REGS_BASEADDR)
      #define GPIOG_REGS_PTR ((GPIO_REGS_Typedef *) GPIOG_REGS_BASEADDR)
    #else
      extern AFIO_REGS_Typedef MOCK_AFIO_REGS;
      #define AFIO_REGS_PTR (&MOCK_AFIO_REGS)

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

  // Khi báo các chân GPIO

    #define GPIO_PIN_0                   0x0001u  /* Pin 0 selected    */
    #define GPIO_PIN_1                   0x0002u  /* Pin 1 selected    */
    #define GPIO_PIN_2                   0x0004u  /* Pin 2 selected    */
    #define GPIO_PIN_3                   0x0008u  /* Pin 3 selected    */
    #define GPIO_PIN_4                   0x0010u  /* Pin 4 selected    */
    #define GPIO_PIN_5                   0x0020u  /* Pin 5 selected    */
    #define GPIO_PIN_6                   0x0040u  /* Pin 6 selected    */
    #define GPIO_PIN_7                   0x0080u  /* Pin 7 selected    */
    #define GPIO_PIN_8                   0x0100u  /* Pin 8 selected    */
    #define GPIO_PIN_9                   0x0200u  /* Pin 9 selected    */
    #define GPIO_PIN_10                  0x0400u  /* Pin 10 selected   */
    #define GPIO_PIN_11                  0x0800u  /* Pin 11 selected   */
    #define GPIO_PIN_12                  0x1000u  /* Pin 12 selected   */
    #define GPIO_PIN_13                  0x2000u  /* Pin 13 selected   */
    #define GPIO_PIN_14                  0x4000u  /* Pin 14 selected   */
    #define GPIO_PIN_15                  0x8000u  /* Pin 15 selected   */
    #define GPIO_PIN_ALL                 0xFFFFu  /* All pins selected */

    #define GPIO_PIN_MASK                0xFFFFu  /* PIN mask for assert test */

#endif /* LIB_GPIO_DEF_H_ */

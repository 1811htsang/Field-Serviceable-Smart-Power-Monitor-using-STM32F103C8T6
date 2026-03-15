/*
 * lib_spi_def.h
 *
 *  Created on: Mar 15, 2026
 *      Author: shanghuang
 */

#ifndef LIB_SPI_DEF_H_
  #define LIB_SPI_DEF_H_

  // Khai báo thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
    #else
      #include "lib_keyword_def.h"
    #endif

  // Khai báo địa chỉ ngoại vi

    #define SPI_REGS_BASEADDR 0x40013000ul 

    /**
     * Ghi chú:
     * SPI trên dòng F1 có 3 cái là SPI1, SPI2, SPI3.
     * Tuy nhiên SPI2 và SPI3 dành cho sử dụng với I2S 
     * nên sẽ không hỗ trợ sử dụng trong thư viện SPI này.
     */

  // Khai báo bộ thanh ghi của ngoại vi

    tdf_strc SPI_REGS_Typedef {
      __vo BLANK_REG_16B SPI_CR1;         // Offset 0x00, reset 0x0000 0000
      __vo BLANK_REG_16B SPI_CR2;         // Offset 0x04, reset 0x0000 0000
      __vo BLANK_REG_16B SPI_SR;          // Offset 0x08, reset 0x0000 0000
      __vo BLANK_REG_16B SPI_DR;          // Offset 0x0C, reset 0x0000 0000
      __vo BLANK_REG_16B SPI_CRCPR;       // Offset 0x10, reset 0x0000 0007
      __vo BLANK_REG_16B SPI_RXCRCR;      // Offset 0x14, reset 0x0000 0000
      __vo BLANK_REG_16B SPI_TXCRCR;      // Offset 0x18, reset 0x0000 0000
    } SPI_REGS_Typedef;

  // >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define SPI_REGS_PTR ((SPI_REGS_Typedef *) SPI_REGS_BASEADDR)
    #else
      extern SPI_REGS_Typedef MOCK_SPI_REGS;
      #define SPI_REGS_PTR (&MOCK_SPI_REGS)
    #endif

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi SPI_CR1

    #define SPI_CR1_CPHA_POS 0u
    #define SPI_CR1_CPHA_MASK (1u << SPI_CR1_CPHA_POS)

    #define SPI_CR1_CPOL_POS 1u
    #define SPI_CR1_CPOL_MASK (1u << SPI_CR1_CPOL_POS)

    #define SPI_CR1_MSTR_POS 2u
    #define SPI_CR1_MSTR_MASK (1u << SPI_CR1_MSTR_POS)

    #define SPI_CR1_BR_POS 3u
    #define SPI_CR1_BR_MASK (7u << SPI_CR1_BR_POS)

    #define SPI_CR1_SPE_POS 6u
    #define SPI_CR1_SPE_MASK (1u << SPI_CR1_SPE_POS)

    #define SPI_CR1_LSBFIRST_POS 7u
    #define SPI_CR1_LSBFIRST_MASK (1u << SPI_CR1_LSBFIRST_POS)

    #define SPI_CR1_SSI_POS 8u
    #define SPI_CR1_SSI_MASK (1u << SPI_CR1_SSI_POS)

    #define SPI_CR1_SSM_POS 9u
    #define SPI_CR1_SSM_MASK (1u << SPI_CR1_SSM_POS)

    #define SPI_CR1_RXONLY_POS 10u
    #define SPI_CR1_RXONLY_MASK (1u << SPI_CR1_RXONLY_POS)

    #define SPI_CR1_DFF_POS 11u
    #define SPI_CR1_DFF_MASK (1u << SPI_CR1_DFF_POS)

    #define SPI_CR1_CRCEN_POS 13u
    #define SPI_CR1_CRCEN_MASK (1u << SPI_CR1_CRCEN_POS)

    #define SPI_CR1_CRCNEXT_POS 14u
    #define SPI_CR1_CRCNEXT_MASK (1u << SPI_CR1_CRCNEXT_POS)

    #define SPI_CR1_BIDIOE_POS 14u
    #define SPI_CR1_BIDIOE_MASK (1u << SPI_CR1_BIDIOE_POS)

    #define SPI_CR1_BIDIMODE_POS 15u
    #define SPI_CR1_BIDIMODE_MASK (1u << SPI_CR1_BIDIMODE_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi SPI_CR2

    #define SPI_CR2_RXDMAEN_POS 0u
    #define SPI_CR2_RXDMAEN_MASK (1u << SPI_CR2_RXDMAEN_POS)

    #define SPI_CR2_TXDMAEN_POS 1u
    #define SPI_CR2_TXDMAEN_MASK (1u << SPI_CR2_TXDMAEN_POS)

    #define SPI_CR2_SSOE_POS 2u
    #define SPI_CR2_SSOE_MASK (1u << SPI_CR2_SSOE_POS)

    #define SPI_CR2_ERRIE_POS 5u
    #define SPI_CR2_ERRIE_MASK (1u << SPI_CR2_ERRIE_POS)

    #define SPI_CR2_RXNEIE_POS 6u
    #define SPI_CR2_RXNEIE_MASK (1u << SPI_CR2_RXNEIE_POS)

    #define SPI_CR2_TXEIE_POS 7u
    #define SPI_CR2_TXEIE_MASK (1u << SPI_CR2_TXEIE_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi SPI_SR

    #define SPI_SR_RXNE_POS 0u
    #define SPI_SR_RXNE_MASK (1u << SPI_SR_RXNE_POS)

    #define SPI_SR_TXE_POS 1u
    #define SPI_SR_TXE_MASK (1u << SPI_SR_TXE_POS)

    #define SPI_SR_CHSIDE_POS 2u
    #define SPI_SR_CHSIDE_MASK (1u << SPI_SR_CHSIDE_POS)

    #define SPI_SR_UDR_POS 3u
    #define SPI_SR_UDR_MASK (1u << SPI_SR_UDR_POS)

    #define SPI_SR_CRCERR_POS 4u
    #define SPI_SR_CRCERR_MASK (1u << SPI_SR_CRCERR_POS)

    #define SPI_SR_MODF_POS 5u
    #define SPI_SR_MODF_MASK (1u << SPI_SR_MODF_POS)

    #define SPI_SR_OVR_POS 6u
    #define SPI_SR_OVR_MASK (1u << SPI_SR_OVR_POS)

    #define SPI_SR_BSY_POS 7u
    #define SPI_SR_BSY_MASK (1u << SPI_SR_BSY_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi SPI_CRCPR

    #define SPI_CRCPR_CRCPOLY_POS 0u
    #define SPI_CRCPR_CRCPOLY_MASK (0xFFFFu << SPI_CRCPR_CRCPOLY_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi SPI_RXCRCR

    #define SPI_RXCRCR_RXCRC_POS 0u
    #define SPI_RXCRCR_RXCRC_MASK (0xFFFFu << SPI_RXCRCR_RXCRC_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi SPI_TXCRCR

    #define SPI_TXCRCR_TXCRC_POS 0u
    #define SPI_TXCRCR_TXCRC_MASK (0xFFFFu << SPI_TXCRCR_TXCRC_POS)

#endif /* LIB_SPI_DEF_H_ */

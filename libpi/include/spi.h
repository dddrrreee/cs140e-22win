#ifndef __SPI_H__
#define __SPI_H__

// XXX: need to redo so you can optionally have multiple spi devices active.
// the downside is that i think you will have to reset the cs on each 
// transaction?

// right now we just specialize to the main SPI0 rpi interface; trivial to
// add spi1, spi2.

#define SPI_CE0 1111
#define SPI_CE1 2222

void spi_init(unsigned chip_select, unsigned clock_divider);

// tx and rx must be the same size; easy to have dedicated rx or tx.
int spi_transfer(uint8_t rx[], const uint8_t tx[], unsigned nbytes);

// clk_div must be a power of 2.
// void spi0_init(unsigned clk_div);
// void spi1_init(unsigned clk_div);

#endif 

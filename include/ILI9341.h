#ifndef ILI9341_H
#define ILI9341_H

#include "ILI9341_hw.h"

enum ROTATION{
    HORIZONTAL  = 0x28, //0x08 0x48 
    //VERTICAL  = 0x68  //0x68 0x28 NOT DEFINED RIGHT NOW, TODO
};

//public

void ILI9341_init(const uint8_t rotation);//spi_inst_t *spi_port, uint dc, uint rst, uint miso, uint cs, uint sck, uint mosi);

uint16_t ILI9341_width();

uint16_t ILI9341_height();

void ILI9341_startWrite();

void ILI9341_endWrite();

void ILI9341_writePixels(uint16_t *buffer, uint32_t idx);

void ILI9341_setAddrWindow(uint16_t x, uint16_t y, 
    uint16_t loadWidth, uint16_t loadHeight);

void ILI9341_set_rotation(const uint8_t rotation);

void ILI9341_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

void ILI9341_demo();

//private

void ILI9341_set_command(uint8_t cmd);

void ILI9341_command_param(uint8_t param);

void ILI9341_write_data(const void *data, size_t len); // len - lenght of data in bytes 

void ILI9341_set_out_writing(const uint16_t start_x, const uint16_t end_x, const uint16_t start_y, const uint16_t end_y);

void ILI9341_clear();

void ILI9341_hw_reset();

void ILI9341_sw_reset();

uint16_t RGB_to_16bit(uint8_t r, uint8_t g, uint8_t b);

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(ILI9341_CS_PIN, 0); // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(ILI9341_CS_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

#endif //ILI9341
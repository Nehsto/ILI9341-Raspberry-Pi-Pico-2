#include "ILI9341.h"

//public

void ILI9341_init(const uint8_t rotation){//spi_inst_t *spi_port, uint dc, uint rst, uint miso, uint cs, uint sck, uint mosi){
    spi_init(ILI9341_SPI_PORT, 90 * MHz);
    gpio_set_function(ILI9341_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_SCK_PIN,  GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_MOSI_PIN, GPIO_FUNC_SPI);
    spi_set_format(ILI9341_SPI_PORT, 8, SPI_CPOL_1, SPI_CPOL_1, SPI_MSB_FIRST);
    // Make the SPI pins available to picotool
    // bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN,
    //     PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Data = 1; Command = 0
    gpio_init(ILI9341_DC_PIN);
    gpio_set_dir(ILI9341_DC_PIN, GPIO_OUT);

    // Active Low
    gpio_init(ILI9341_RST_PIN);
    gpio_set_dir(ILI9341_RST_PIN, GPIO_OUT);
    gpio_put(ILI9341_RST_PIN, 1);

    // Active Low
    gpio_init(ILI9341_CS_PIN);
    gpio_set_dir(ILI9341_CS_PIN, GPIO_OUT);
    gpio_put(ILI9341_CS_PIN, 1);
    // Make the CS pin available to picotool
    // bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));

    uint8_t data[15] = { 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1,
		0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00};
    uint8_t data2[15]= { 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1,
        0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f};

    ILI9341_hw_reset();
    ILI9341_set_command(ILI9341_SWRESET);

    ILI9341_set_command(ILI9341_GAMMASET);
    ILI9341_command_param(0x04);

    ILI9341_set_command(ILI9341_GMCTRP1);
	ILI9341_write_data(data, 15);
    
    ILI9341_set_command(ILI9341_GMCTRN1);
    ILI9341_write_data(data2, 15);

    ILI9341_set_command(ILI9341_MADCTL);
    ILI9341_command_param(rotation);

    ILI9341_set_command(ILI9341_PIXFMT);
    ILI9341_command_param(0x55);

    ILI9341_set_command(ILI9341_FRMCTL1);
    ILI9341_command_param(0x00);
    ILI9341_command_param(0x1B);
    
    ILI9341_set_command(ILI9341_SLEEPOUT);
    ILI9341_set_command(ILI9341_IDLEOFF);
    ILI9341_set_command(ILI9341_DISPLAYON);
}

uint16_t ILI9341_width(){
    return ILI9341_WIDTH;
}

uint16_t ILI9341_height(){
    return ILI9341_HEIGHT;
}

void ILI9341_startWrite(){
    cs_select();
}

void ILI9341_endWrite(){
    cs_deselect();
}

void ILI9341_writePixels(){

}

void ILI9341_setAddrWindow(){

}

void ILI9341_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color){
	uint16_t buffer[ILI9341_HEIGHT];

	for(size_t i = 0; i < height; i++){
		buffer[i] = color;
	}

    ILI9341_set_out_writing(x, x + width - 1, y, y + height - 1);
    for(size_t i = 0; i < width; i++){
        ILI9341_write_data(buffer, height * sizeof(uint16_t));
    }
}

void ILI9341_demo(){
    ILI9341_clear();
    int w = ILI9341_WIDTH;
    int h = ILI9341_HEIGHT;

    //Top Left, Red
    ILI9341_draw_rect(0, 0, 50, 50, RGB_to_16bit(0xff, 0, 0));

    //Bottom Left, Blue
    ILI9341_draw_rect(0, h-50, 50, 50, RGB_to_16bit(0, 0, 0xff));

    //Top right, Green
    ILI9341_draw_rect(w-50, 0, 50, 50, RGB_to_16bit(0, 0xff, 0));

    //Bottom right, Magenta
    ILI9341_draw_rect(w-50, h-50, 50, 50, RGB_to_16bit(0xff, 0x00, 0xff));

    //Middle, Cyan
    ILI9341_draw_rect((w/2)-50, (h/2)-25, 100, 50, RGB_to_16bit(0x00, 0xff, 0xff));

}

// private

void ILI9341_set_command(uint8_t cmd){
    gpio_put(ILI9341_DC_PIN, COMMAND_BIT);
    asm volatile("nop \n nop \n nop");

    cs_select();
    spi_write_blocking(ILI9341_SPI_PORT, &cmd, 1);
    cs_deselect();

    gpio_put(ILI9341_DC_PIN, DATA_BIT);
    asm volatile("nop \n nop \n nop");

    sleep_ms(CMD_SLEEP_MS);
}

void ILI9341_command_param(uint8_t param){
    ILI9341_write_data(&param, 1);
}

void ILI9341_write_data(const void *data, size_t len){
    cs_select();
    spi_write_blocking(ILI9341_SPI_PORT, (const uint8_t *)data, len);
    cs_deselect();
    //sleep_ms(CMD_SLEEP_MS);
}

void ILI9341_set_out_writing(const uint16_t start_x, const uint16_t end_x, const uint16_t start_y, const uint16_t end_y){
    // Column address set.
    // When horizontal, pages are along the x axis (rotated 90 deg)
    ILI9341_set_command(ILI9341_PASET);

    ILI9341_command_param((start_x >> 8) & 0xFF);
    ILI9341_command_param(start_x & 0xFF);

    ILI9341_command_param((end_x >> 8) & 0xFF);
    ILI9341_command_param(end_x & 0xFF);

    // Page address set.
    // When horizontal, columns are on the y axis (rotated 90 deg)
    ILI9341_set_command(ILI9341_CASET);

    ILI9341_command_param((start_y >> 8) & 0xFF);
    ILI9341_command_param(start_y & 0xFF);

    ILI9341_command_param((end_y >> 8) & 0xFF);
    ILI9341_command_param(end_y & 0xFF);

    // Start writing.
    ILI9341_set_command(ILI9341_RAMWR);
}

void ILI9341_clear(){
	ILI9341_draw_rect(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, 0);
}

void ILI9341_hw_reset(){
    gpio_put(ILI9341_RST_PIN, false);
    sleep_ms(HW_RESET_SLEEP_MS);
    gpio_put(ILI9341_RST_PIN, true);
    sleep_ms(BOOTUP_SLEEP_MS);
}

void ILI9341_sw_reset(){
    ILI9341_set_command(ILI9341_SWRESET);
    sleep_ms(BOOTUP_SLEEP_MS);
}

uint16_t RGB_to_16bit(uint8_t r, uint8_t g, uint8_t b){
    r >>= 3;
    g >>= 2;
    b >>= 3;
    uint16_t res = 0;
    res = (((uint16_t) r & 0x1F) << 11) | (((uint16_t) g & 0x3F) << 5) | ((uint16_t) b & 0x1F);
    return (res >> 8) | (res << 8);
}
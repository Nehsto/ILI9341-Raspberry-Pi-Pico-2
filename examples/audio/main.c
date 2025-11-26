#include <math.h>
#include <stdio.h>
#include <string.h>
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "i2s.h"
#include "pico/stdlib.h"


#include "i2sPlayer.h"

static __attribute__((aligned(8))) pio_i2s i2s;

static void dma_i2s_out_handler(void) {
    /* We're double buffering using chained TCBs. By checking which buffer the
     * DMA is currently reading from, we can identify which buffer it has just
     * finished reading (the completion of which has triggered this interrupt).
     * This handler is triggered when the output DMA finishes sending a buffer.
     */
    // if (*(int32_t**)dma_hw->ch[i2s.dma_ch_out_ctrl].read_addr == i2s.output_buffer) {
        // DMA is reading from first buffer, so it just finished the second buffer
        // We can now write new data to the second buffer
    //     first_buff = false;
    // } else {
        // DMA is reading from second buffer, so it just finished the first buffer
        // We can now write new data to the first buffer
    //     first_buff = true;
    // }
    write_to = true;
    dma_hw->ints0 = 1u << i2s.dma_ch_out_data;  // clear the IRQ
}

int main(){
    set_sys_clock_khz(132000, true);
    stdio_init_all();

    sleep_ms(2000);

    printf("System Clock: %lu\n", clock_get_hz(clk_sys));
    
    FATFS fs;
    FRESULT fr;

    i2s_program_start_out(pio0, &i2s_config_default, dma_i2s_out_handler, &i2s);

    printf("DMA out ctrl channel: %u\n", i2s.dma_ch_out_ctrl);
    printf("DMA out data channel: %u\n", i2s.dma_ch_out_data);
    printf("DMA ctrl read addr: %p\n", (void*)dma_hw->ch[i2s.dma_ch_out_ctrl].read_addr);
    printf("DMA data read addr: %p\n", (void*)dma_hw->ch[i2s.dma_ch_out_data].read_addr);
    printf("DMA data write addr: %p\n", (void*)dma_hw->ch[i2s.dma_ch_out_data].write_addr);
    printf("PIO TXF level: %u\n", pio_sm_get_tx_fifo_level(i2s.pio, i2s.sm_dout));
    printf("Output buffer 0: %p\n", (void*)i2s.output_buffer);
    printf("Output buffer 1: %p\n", (void*)&i2s.output_buffer[STEREO_BUFFER_SIZE]);

    // //Better test pattern - write as bytes to match WAV reading
    // uint8_t* buf_ptr = (uint8_t*)i2s.output_buffer;
    // for(int i = 0; i < STEREO_BUFFER_SIZE * 2; i += 2) {
    //     buf_ptr[i] = 0x34;      // Low byte
    //     buf_ptr[i+1] = 0x12;    // High byte (little-endian)
    // }
    // buf_ptr = (uint8_t*)&i2s.output_buffer[STEREO_BUFFER_SIZE];
    // for(int i = 0; i < STEREO_BUFFER_SIZE * 2; i += 2) {
    //     buf_ptr[i] = 0x78;      // Low byte
    //     buf_ptr[i+1] = 0x56;    // High byte (little-endian)
    // }
    // printf("Buffers filled, starting playback\n");

    puts("i2s_example started.");

    printf("Mounting SD card...\n");
    fr = f_mount(&fs, "", 1);
    printf("f_mount returned: %d\n", fr);

    printf("Checking if f_mount value = %d\n", FR_OK);
    if (FR_OK != fr) {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    printf("Returned value is valid\n");

    //i2sPlayer_propertiesWAV("music_box.wav");
    i2sPlayer_playWAV("music_box.wav", &i2s);

    return 0;
}

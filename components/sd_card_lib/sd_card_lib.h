#ifndef SD_CARD_LIB_H_
#define SD_CARD_LIB_H_

#define MOUNT_POINT     "/sdcard"
#define PIN_NUM_MISO    19
#define PIN_NUM_MOSI    23
#define PIN_NUM_CLK     18
#define PIN_NUM_CS      5
#define SPI_DMA_CHAN    1

typedef struct {
    int id;
    int temperature;
    int humidity;
} messages_data_t;

void sd_card_init(void);
void sd_card_write(int temperature, int humidity);
char* json_creat(int id, int temperature, int humidity);
messages_data_t sd_card_read(uint32_t line_number);

#endif
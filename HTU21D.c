#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>

#define I2C_HTU21D_ADDR     0x40
#define HTU21D_RESET        0xFE
#define HTU21D_TEMPERATURE  0xE3
#define HTU21D_HUMIDITY     0xE5

void htu21_read_temperature (uint16_t adc, float *temperature);
void htu21_read_humidity (uint16_t adc, float *humidity);

int main(){
    uint8_t ret, fd, value;
    uint8_t dataRead_temp[4];
    uint8_t dataRead_hmdt[4];
    uint8_t dataWrite_temp = HTU21D_TEMPERATURE;
    uint8_t dataWrite_hmdt = HTU21D_HUMIDITY;
    uint16_t adc_temperature, adc_humidity;
    float temperature, humidity;
    
    fd = open("/dev/i2c-11", O_RDWR);
    
    if (fd < 0){
        perror("Failed open device");
        close(fd);
        return errno;
    }
    
    ret = ioctl(fd, I2C_SLAVE, I2C_HTU21D_ADDR);
    if (ret < 0){
        printf("ioctl error: %s\n", strerror(ret));
        close(fd);
        return 1;
    }
    
    for (value=0; value<=2; value++) {
        ret = write(fd, &dataWrite_temp, 1);
        if (ret != 1){
            printf("Dev write rc: %s\n", strerror(ret));
            close(fd);
            return 1;
        }
        
        ret = read(fd, dataRead_temp, 3);
        if (ret < 0){
            printf("Dev read rc: %s\n", strerror(ret));
            close(fd);
            return 1;
        }
        
        ret = write(fd, &dataWrite_hmdt, 1);
        if (ret != 1){
            printf("Dev write rc: %s\n", strerror(ret));
            close(fd);
            return 1;
        }
        
        ret = read(fd, dataRead_hmdt, 3);
        if (ret < 0){
            printf("Dev read rc: %s\n", strerror(ret));
            close(fd);
            return 1;
        }
        
        adc_temperature = (dataRead_temp[0]<<8 | dataRead_temp[1]) & 0xFFFC;
        adc_humidity = (dataRead_hmdt[0] << 8) | dataRead_hmdt[1];
        
        htu21_read_temperature (adc_temperature, &temperature);
        htu21_read_humidity (adc_humidity, &humidity);

        printf("TEMP: %3.2f'C HUMIDITY: %3.2f %cRH\n", temperature, humidity, '%');
        sleep(1);
    }
    
    close(fd);
    return 0;
}

void htu21_read_temperature (uint16_t adc, float *temperature)
{
    *temperature = (float)adc * 175.72 / (1UL<<16) + (-46.85);
}

void htu21_read_humidity (uint16_t adc, float *humidity)
{
    // Perform conversion function
	*humidity = (float)adc * 125 / (1UL<<16) + (-6);
}

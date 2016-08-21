#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>

struct flash_write_test_ctx {
    bool terminate;
    int foo;
    bool fail_pro;
    bool fail_app;
};

static void flash_write_task(void* arg)
{
    struct flash_write_test_ctx* ctx = (struct flash_write_test_ctx*) arg;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    const uint32_t sector = 6;
    if (spi_flash_erase_sector(sector) != ESP_OK) {
        ctx->fail_pro = true;
        ets_printf("Erase failed\r\n");
    } else {
        uint32_t val = 0xabcd1234;
        for (uint32_t offset = 0; offset < SPI_FLASH_SEC_SIZE; offset += 4) {
            if (spi_flash_write(sector * SPI_FLASH_SEC_SIZE + offset, &val, 4) != ESP_OK) {
                ets_printf("Write failed at offset=%d\r\n", offset);
                ctx->fail_pro = true;
                break;
            }
        }
        uint32_t val_read;
        for (uint32_t offset = 0; offset < SPI_FLASH_SEC_SIZE; offset += 4) {
            if (spi_flash_read(sector * SPI_FLASH_SEC_SIZE + offset, &val_read, 4) != ESP_OK) {
                ets_printf("Read failed at offset=%d\r\n", offset);
                ctx->fail_pro = true;
                break;
            }
            if (val_read != val) {
                ets_printf("Read invalid value=%08x at offset=%d\r\n", val_read, offset);
                ctx->fail_pro = true;
                break;
            }
        }
    }
    ctx->terminate = true;
    vTaskDelete(NULL);
}

static void appcpu_dummy_task(void* arg)
{
    volatile struct flash_write_test_ctx* ctx = (struct flash_write_test_ctx*) arg;
    const uint32_t sector = 6;
    if (spi_flash_erase_sector(sector) != ESP_OK) {
        ctx->fail_app = true;
    }
    while (!ctx->terminate) {
        ctx->foo *= (ctx->foo + 1);
    }
    vTaskDelete(NULL);
}

TEST_CASE("flash write and erase work on PRO CPU and fail on APP CPU", "[spi_flash]")
{
    TaskHandle_t flash_write_task_handle;
    TaskHandle_t appcpu_dummy_task_handle;
    struct flash_write_test_ctx ctx;
    ctx.terminate = false;
    ctx.foo = 1;
    ctx.fail_pro = false;
    ctx.fail_app = false;
    xTaskCreatePinnedToCore(flash_write_task, "1", 2048, &ctx, 3, &flash_write_task_handle, 0);
    xTaskCreatePinnedToCore(appcpu_dummy_task, "2", 2048, &ctx, 3, &appcpu_dummy_task_handle, 1);
    while (!ctx.terminate) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    TEST_ASSERT_EQUAL(false, ctx.fail_pro);
    TEST_ASSERT_EQUAL(true, ctx.fail_app);
}


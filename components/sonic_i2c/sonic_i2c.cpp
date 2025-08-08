#include "sonic_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sonic_i2c {

static const char *const TAG = "sonic_i2c";

void SonicI2CComponent::set_update_interval(uint32_t update_interval) {
  PollingComponent::set_update_interval(update_interval);
}

void SonicI2CComponent::update() {
  uint32_t now = millis();

  switch (state_) {
    // STATE 0: Idle — trigger measurement
    case 0: {
      uint8_t cmd = 0x00;
      auto err = this->write(&cmd, 1);
      if (err == i2c::ERROR_OK) {
        state_ = 1;
        last_request_ms_ = now;
        ESP_LOGV(TAG, "Triggered measurement");
      } else {
        ESP_LOGW(TAG, "Failed to trigger measurement: %d", (int)err);
        this->status_set_warning();
      }
      break;
    }

    // STATE 1: Move to waiting
    case 1:
      state_ = 2;
      break;

    // STATE 2: Wait and read result
    case 2:
      if (now - last_request_ms_ > READ_TIMEOUT_MS) {
        uint8_t reg = 0x03;
        auto err = this->write(&reg, 1);
        if (err != i2c::ERROR_OK) {
          ESP_LOGW(TAG, "Failed to select register 0x03: %d", (int)err);
          state_ = 0;
          return;
        }

        delay(1);  // Small delay if sensor needs it

        uint8_t data[2];
        err = this->read(data, 2);
        if (err == i2c::ERROR_OK) {
          uint16_t dist = (data[0] << 8) | data[1];
          if (dist > 0 && dist < 5000) {
            this->publish_state(dist);
            ESP_LOGV(TAG, "Distance: %u mm", dist);
          } else {
            ESP_LOGD(TAG, "Invalid reading: %u mm", dist);
          }
        } else {
          ESP_LOGW(TAG, "Read failed at 0x03: %d", (int)err);
        }

        state_ = 0;  // Reset for next cycle
      }
      break;
  }
}

void SonicI2CComponent::dump_config() {
  LOG_SENSOR("", "Sonic I2C Sensor", this);
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with I2C device failed!");
  }
}

}  // namespace sonic_i2c
}  // namespace esphome

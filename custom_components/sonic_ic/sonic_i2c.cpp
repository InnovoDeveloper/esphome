#include "sonic_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sonic_i2c {

static const char *const TAG = "sonic_i2c";

void SonicI2CComponent::update() {
  uint32_t now = millis();

  switch (state_) {
    // STATE 0: Idle — trigger measurement
    case 0: {
      if (this->write(0x00, 0x00)) {
        state_ = 1;
        last_request_ms_ = now;
        ESP_LOGV(TAG, "Triggered measurement");
      } else {
        ESP_LOGW(TAG, "Failed to trigger measurement");
        this->status_set_warning();
      }
      break;
    }

    // STATE 1: Just triggered — move to waiting
    case 1:
      state_ = 2;
      break;

    // STATE 2: Waiting for result — read distance from 0x03
    case 2:
      if (now - last_request_ms_ > 100) {  // Wait max 100ms
        uint8_t data[2];
        if (this->read(0x03, data, 2) == i2c::ERROR_OK) {
          uint16_t dist = (data[0] << 8) | data[1];
          if (dist > 0 && dist < 5000) {
            this->publish_state(dist);
            ESP_LOGV(TAG, "Distance: %u mm", dist);
          } else {
            ESP_LOGD(TAG, "Invalid reading: %u mm", dist);
          }
        } else {
          ESP_LOGW(TAG, "Read failed at 0x03");
        }
        state_ = 0;  // Reset
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

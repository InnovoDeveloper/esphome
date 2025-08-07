#include "sonic_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
static const char *TAG = "sonic_i2c";
namespace sonic_i2c_sensor {

void SonicI2C::dump_config() {
  LOG_SENSOR(TAG, "Ultrasonic Sensor", this);
  ESP_LOGD(TAG, "Ultrasonic Sensor I2C Address :  %x", this->address_);
  LOG_UPDATE_INTERVAL(this);
}

void SonicI2C::setup() {
  // Init the sensor
  ESP_LOGI(TAG, "Ultrasonic Sensor Setup begin");
}

void SonicI2C::update() {
  // Start measurement and schedule reading after 120ms
  if (!this->measurement_in_progress_) {
    this->start_measurement();
    this->measurement_in_progress_ = true;
    this->measurement_start_time_ = millis();
    // Schedule read_distance after 120ms
    this->set_timeout("sonic_i2c_read", 120, [this]() { this->read_distance(); });
  }
}

void SonicI2C::start_measurement() {
  uint8_t val = 0x01;
  this->write(&val, 1);
  ESP_LOGD(TAG, "Measurement started");
}

void SonicI2C::read_distance() {
  uint32_t data;
  uint8_t data_buffer[] = {0, 0, 0, 0, 0};

  if (this->read(data_buffer, 3)) {
    data = (data_buffer[0] << 16) | (data_buffer[1] << 8) | data_buffer[2];
    float Distance = float(data) / 1000;
    if (Distance > 4500.00) {
      Distance = 4500.00;
    }
    if (Distance >= 4500 || Distance <= 20) {
      ESP_LOGI(TAG, "Incorrect Distance Reading");
    } else {
      ESP_LOGD(TAG, "%s - Got distance: %.2f mm", this->name_.c_str(), Distance);
    }
    publish_state(Distance);
  } else {
    ESP_LOGW(TAG, "I2C Read failed");
  }

  this->measurement_in_progress_ = false;
}

} // namespace sonic_i2c_sensor
} // namespace esphome

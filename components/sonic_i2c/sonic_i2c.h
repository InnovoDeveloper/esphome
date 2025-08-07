#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace sonic_i2c {

class SonicI2CComponent : public PollingComponent, public i2c::I2CDevice, public sensor::Sensor {
 public:
  void update() override;
  void dump_config() override;
  virtual ~SonicI2CComponent() {}

  void set_address(uint8_t address) { this->address_ = address; }

 protected:
  uint8_t state_ = 0;  // 0=idle, 1=requested, 2=waiting
  uint32_t last_request_ms_ = 0;
};

}  // namespace sonic_i2c
}  // namespace esphome

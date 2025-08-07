#pragma once 

#include <esphome.h>
#include "esphome/core/gpio.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome{
namespace sonic_i2c_sensor{
 
class SonicI2C: public i2c::I2CDevice, public sensor::Sensor, public PollingComponent {
    public:
     void setup() override ;
     void dump_config() override;
     void update() override ;
     void start_measurement();
     void read_distance();

    private:
     bool measurement_in_progress_{false};
     uint32_t measurement_start_time_{0};
};

}

}

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    UNIT_MILLIMETER,
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
)

sonic_i2c_ns = cg.esphome_ns.namespace("sonic_i2c")
SonicI2CComponent = sonic_i2c_ns.class_(
    "SonicI2CComponent", cg.PollingComponent, i2c.I2CDevice, sensor.Sensor
)

CONFIG_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_MILLIMETER,
    device_class=DEVICE_CLASS_DISTANCE,
    state_class=STATE_CLASS_MEASUREMENT,
    accuracy_decimals=0,
).extend(
    {
        cv.GenerateID(): cv.declare_id(SonicI2CComponent),
    }
).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x57))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)
    yield sensor.register_sensor(var, config)

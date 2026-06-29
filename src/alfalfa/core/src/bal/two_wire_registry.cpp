#include <alfalfa/bal/two_wire_registry.hpp>

#include <map>
#include <set>


auto known_devices = std::map<std::string, std::set<uint8_t>>{
    {"ADS105", {0x48, 0x49, 0x4A, 0x4B}},               // 12-bit ADC with PGA and comparator
    {"ADS1115", {0x48, 0x49, 0x4A, 0x4B}},              // 16-bit ADC with PGA and comparator
    {"AHT10", {0x38, 0x39}},                            // Temperature and Humidity Sensor
    {"BMP280", {0x76, 0x77}},                           // Temperature and Pressure Sensor
    {"INA3221", {0x40, 0x41, 0x42, 0x43}},              // Triple-channel current and voltage monitor
    {"MCP4725A0", {0x60, 0x61}},                        // 12-bit DAC with EEPROM
    {"MCP4725A1", {0x62, 0x63}},                        // 12-bit DAC with EEPROM
    {"MCP4725A2", {0x64, 0x65}},                        // 12-bit DAC with EEPROM
    {"MCP4725A3", {0x66, 0x67}},                        // 12-bit DAC with EEPROM
    {"MCP23017", {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27}}, // 16-bit I/O expander with interrupt
    {"SI5351", {0x60, 0x61}},                           // Clock generator with three outputs
    {"TCS34725", {0x29}},                               // Color sensor with IR filter
    {"BH1750", {0x23, 0x5C}},                           // Digital light sensor
    {"VEML7700", {0x10}},                               // Ambient light sensor with high dynamic range
    {"SI7021-A20", {0x40}},                             // Temperature and Humidity Sensor
    {"SHT21", {0x40}},                                  // Temperature and Humidity Sensor
    {"SHT20", {0x40}},                                  // Temperature and Humidity Sensor
    {"BMI160", {0x68, 0x69}}                            // 3-axis accelerometer and gyroscope
};



namespace alfalfa
{
namespace bal
{

    std::vector<std::string> MatchDeviceAddress(uint8_t device_address)
    {
        std::vector<std::string> matched_devices;
        for (const auto& [device_name, addresses] : known_devices)
        {
            if (addresses.find(device_address) != addresses.end())
            {
                matched_devices.push_back(device_name);
            }
        }
        return matched_devices;
    }

}
}
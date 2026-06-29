#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include <alfalfa/lib/types/units/all.hpp>
#include <alfalfa/hal/two_wire.hpp>


namespace alfalfa
{
namespace bal
{
namespace ADS1x1x
{

    enum class Register : uint8_t
    {
        Conversion = 0x00,
        Config = 0x01,
        LowThreshold = 0x02,
        HighThreshold = 0x03,
    };


    struct ConfigRegister
    {
        enum class ComparatorQueue : uint8_t
        {
            ASSERT_AFTER_ONE = 0b00,
            ASSERT_AFTER_TWO = 0b01,
            ASSERT_AFTER_FOUR = 0b10,
            DISABLE_COMPARATOR = 0b11,
        };

        enum class InputMuxConfig : uint8_t
        {
            An0ToAn1 = 0b000,
            An0ToAn3 = 0b001,
            An1ToAn3 = 0b010,
            An2ToAn3 = 0b011,
            An0 = 0b100,
            An1 = 0b101,
            An2 = 0b110,
            An3 = 0b111,
        };

        enum class GainConfig : uint8_t
        {
            FSR_6144 = 0b000,
            FSR_4096 = 0b001,
            FSR_2048 = 0b010,
            FSR_1024 = 0b011,
            FSR_0512 = 0b100,
            FSR_0256 = 0b101,
        };

        enum class DataRate1015 : uint8_t
        {
            SPS_128 = 0b000,
            SPS_250 = 0b001,
            SPS_490 = 0b010,
            SPS_920 = 0b011,
            SPS_1600 = 0b100,
            SPS_2400 = 0b101,
            SPS_3300 = 0b110,
        };

        enum class DataRate1115 : uint8_t
        {
            SPS_8 = 0b000,
            SPS_16 = 0b001,
            SPS_32 = 0b010,
            SPS_64 = 0b011,
            SPS_128 = 0b100,
            SPS_250 = 0b101,
            SPS_475 = 0b110,
            SPS_860 = 0b111,
        };

        ComparatorQueue comp_queue:2;
        bool comparator_latch:1;
        bool comparator_polarity:1;
        bool comparator_mode:1;
        uint8_t data_rate:3;
        bool mode:1;
        GainConfig pga_gain:3;
        InputMuxConfig input_mux:3;
        bool os_single_shot:1;
    };

    static_assert(std::endian::native == std::endian::little);
    static_assert(sizeof(ConfigRegister) == 2);

    struct BaseModule
    {
        BaseModule(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> two_wire_module,
            alfalfa::hal::two_wire::AddressVariant address);

        /**
         * @brief Read a single 16-bit register from the device.
         *
         * @param reg Register to read.
         * @return Unsigned 16-bit value of the register.
         */
        uint16_t ReadRegister(alfalfa::bal::ADS1x1x::Register reg);

        /**
         * @brief Write a 16-bit value to a single register.
         *
         * @param reg Register to write to.
         * @param value Value to write.
         */
        void WriteRegister(alfalfa::bal::ADS1x1x::Register reg, uint16_t value);

        /**
         * @brief Attempts to execute a transaction with the device.
         *
         * @return If an ack was received for a test transaction.
         */
        bool IsConnected();

    private:
        const std::byte _address;
        std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> _tw;
    };

    struct ADS1015 : public alfalfa::bal::ADS1x1x::BaseModule
    {
        ADS1015(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> two_wire_module,
            alfalfa::hal::two_wire::AddressVariant address);

        int16_t GetConversionResult();

        Units::Volt GetVoltage();

        void ConfigureSingleShot(
            const ConfigRegister::InputMuxConfig input,
            const ConfigRegister::GainConfig gain,
            const ConfigRegister::DataRate1015 rate,
            const bool trigger);

        void ConfigureContinuosSampling(
            const ConfigRegister::InputMuxConfig input,
            const ConfigRegister::GainConfig gain,
            const ConfigRegister::DataRate1015 rate);

        void _SetGain(const ConfigRegister::GainConfig gain);

    private:
        double _value_per_count;
    };

    struct ADS1115 : public alfalfa::bal::ADS1x1x::BaseModule
    {
        ADS1115(
            std::shared_ptr<alfalfa::hal::two_wire::iBusMaster> two_wire_module,
            alfalfa::hal::two_wire::AddressVariant address);

        int16_t GetConversionResult();

        Units::Volt GetVoltage();

        void ConfigureSingleShot(
            const ConfigRegister::InputMuxConfig input,
            const ConfigRegister::GainConfig gain,
            const ConfigRegister::DataRate1115 rate,
            const bool trigger);

        void ConfigureContinuosSampling(
            const ConfigRegister::InputMuxConfig input,
            const ConfigRegister::GainConfig gain,
            const ConfigRegister::DataRate1115 rate);

        void _SetGain(const ConfigRegister::GainConfig gain);

    private:
        double _value_per_count;
    };

}
}
}

#include <alfalfa/impl/esp-idf/hal/two_wire.hpp>
#include <alfalfa/boards/esp32_c3_super_mini.hpp>

#include <alfalfa/oal/timekeeper.hpp>

#include <alfalfa/abm/console.hpp>
#include <alfalfa/lib/log.hpp>
#include <alfalfa/lib/custom.hpp>
#include <alfalfa/bal/aht10.hpp>
#include <alfalfa/bal/bmp280.hpp>
#include <alfalfa/bal/mcp4725.hpp>
#include <alfalfa/bal/ads1x1x.hpp>
#include <alfalfa/bal/two_wire_registry.hpp>


extern "C" void app_main();


void app_main()
{
    auto board = std::make_shared<alfalfa::boards::Esp32C3SuperMini>();
    alfalfa::logging::Init();
    alfalfa::logging::SetLevel(alfalfa::logging::LogLevel::Debug);

    auto console = alfalfa::abm::console::Console(board->uart->Rx(), board->uart->Tx());
    alfalfa::logging::SetStdout(std::get<1>(console.GetStdStreams()));
    auto loop = Timekeeper::MonotoneInterval(50_ms);
    auto blinky = Timekeeper::MonotoneInterval(175_ms);

    console.AddFunction("blinky", [&blinky](auto& in, auto& out, auto& args){
        if (args.size() != 1)
        {
            out << "Usage: blinky <period>" << std::endl;
            return;
        }
        auto period = FromString<Units::Second>(args.at(0));
        blinky.SetBackoff(period);
        out << "Updated blinky successfully.\n";
    });
    console.AddFunction("reset", [board](auto& in, auto& out, auto& args){
        out << "Resetting board...\n";
        board->SoftReset();
    });

    auto i2c = std::make_shared<alfalfa::hal::two_wire::BusMaster>(
        I2C_NUM_0,
        alfalfa::hal::Pin(5), // SCL
        alfalfa::hal::Pin(6),  // SDA
        100_kHz,
        50_ms
    );

    // Scan the I2C address range, and show who's online
    auto alive_addresses = alfalfa::hal::two_wire::ScanBus(*i2c, 50_ms, 0x01, 0x7F);
    for (auto address : alive_addresses)
    {
        auto matches = alfalfa::bal::MatchDeviceAddress(address);
        Logln(std::format("Address {:x} is alive", address));
        for (const auto& match : matches)
        {
            Logln(std::format("  - {}", match));
        }
    }
    Logln(std::format("Total alive: {}", alive_addresses.size()));

    auto sensor = std::make_shared<alfalfa::bal::Bmp280::Bmp280>(i2c, true);
    auto chip_id = sensor->ReadChipId();
    Logln("BMP280 initialized. Chip ID: ", std::format("{:#04x}", chip_id));
    if (chip_id != 0x58)
    {
        throw std::runtime_error("BMP280: Invalid chip ID");
    }
    sensor->SetControlMeas(
        alfalfa::bal::Bmp280::PowerMode::Normal,
        alfalfa::bal::Bmp280::TemperatureSampling::x16_20bit,
        alfalfa::bal::Bmp280::PressureSampling::x16_UltraHighResolution);

    Logln("Reading MCP4725 DAC");
    auto mcp_dac = std::make_shared<alfalfa::bal::MCP4725>(i2c, true);

    Logln("Reading AHT10");
    auto aht10 = std::make_shared<alfalfa::bal::Aht10>(i2c, false);

    auto ads1015 = std::make_shared<alfalfa::bal::ADS1x1x::ADS1015>(i2c, alfalfa::hal::two_wire::AddressSelectorExtended::GND);
    ads1015->ConfigureContinuosSampling(
        alfalfa::bal::ADS1x1x::ConfigRegister::InputMuxConfig::An0,
        alfalfa::bal::ADS1x1x::ConfigRegister::GainConfig::FSR_6144,
        alfalfa::bal::ADS1x1x::ConfigRegister::DataRate1015::SPS_920);
    auto ads1115 = std::make_shared<alfalfa::bal::ADS1x1x::ADS1115>(i2c, alfalfa::hal::two_wire::AddressSelectorExtended::GND);
    ads1115->ConfigureContinuosSampling(
        alfalfa::bal::ADS1x1x::ConfigRegister::InputMuxConfig::An0,
        alfalfa::bal::ADS1x1x::ConfigRegister::GainConfig::FSR_6144,
        alfalfa::bal::ADS1x1x::ConfigRegister::DataRate1115::SPS_860);
    auto an_value = Units::percent_0;
    uint16_t cnt = 0;

    auto trxs = Timekeeper::MonotoneInterval(1_s);
    while(true)
    {
        loop.DelayUntilMark();
        if (blinky.Mark())
        {
            board->blue_led.Toggle();
            {
                an_value += mcp_dac->GetStep();
                if (an_value > Units::percent_100)
                {
                    an_value = Units::percent_0;
                }
                mcp_dac->WriteOutput(an_value);
                cnt += 1;
                if (cnt >= mcp_dac->GetMaxCount())
                {
                    cnt = 0;
                }
                mcp_dac->WriteOutput(cnt);
                Logln(std::format("Setting DAC to {:.2f}, count: {}", an_value, cnt));
            }
        }
        console.ConsoleTask();

        if (trxs.Mark())
        {
            try
            {
                const auto [temp, pres] = sensor->ReadCombo();
                Logln(std::format("BMP280: {:.2f} {:.0f|h}", temp, pres));
                const auto [humidity, temperature] = aht10->ReadHumidityAndTemperature();
                Logln(std::format("AHT10:  {:.2f} {:.2f}", humidity, temperature));
                const auto an_value_read_10 = ads1015->GetVoltage();
                Logln(std::format("ADS1015 voltage: {:.3f}", an_value_read_10));
                const auto an_value_read_11 = ads1115->GetVoltage();
                Logln(std::format("ADS1115 voltage: {:.3f}", an_value_read_11));
            }
            catch (const std::exception& e)
            {
                Logln("Error initializing AHT10: ", e.what());
            }
        }
    }
}

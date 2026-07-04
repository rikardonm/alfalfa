#include <alfalfa/impl/esp-idf/hal/two_wire.hpp>
#include <alfalfa/impl/esp-idf/hal/spi.hpp>
#include <alfalfa/boards/esp32_c3_super_mini.hpp>

#include <alfalfa/oal/timekeeper.hpp>

#include <alfalfa/abm/console.hpp>
#include <alfalfa/lib/types/color.hpp>
#include <alfalfa/lib/log.hpp>
#include <alfalfa/lib/custom.hpp>
#include <alfalfa/bal/aht10.hpp>
#include <alfalfa/bal/bmp280.hpp>
#include <alfalfa/bal/mcp4725.hpp>
#include <alfalfa/bal/ads1x1x.hpp>
#include <alfalfa/bal/ws2812b.hpp>
#include <alfalfa/bal/two_wire_registry.hpp>


extern "C" void app_main();


void app_main()
{
    auto board = std::make_shared<alfalfa::boards::Esp32C3SuperMini>();
    alfalfa::logging::Init();
    alfalfa::logging::SetLevel(alfalfa::logging::LogLevel::Debug);

    auto console = alfalfa::abm::console::Console(board->uart->Rx(), board->uart->Tx());
    alfalfa::logging::SetStdout(std::get<1>(console.GetStdStreams()));
    auto loop = Timekeeper::MonotoneInterval(10_ms);
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
        200_kHz,
        5_ms
    );

    auto spi = std::make_shared<alfalfa::hal::impl::spi::Module>(
        SPI2_HOST,
        alfalfa::hal::Pin(10), // MOSI
        alfalfa::hal::Pin(20), // MISO
        alfalfa::hal::Pin(21), // SCK
        512
    );

    // WS2812B chip select on IO7
    auto ws2812b = alfalfa::bal::WS2812B::StaticSpiDriver<8>(
        std::make_unique<alfalfa::hal::impl::spi::Slave>(
            spi,
            alfalfa::hal::Pin(7),
            4_MHz
        ));

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
    uint8_t led_cnt = 0;
    uint8_t led_sel = 0;

    auto trxs = Timekeeper::MonotoneInterval(1_s);
    auto ws_led = Timekeeper::MonotoneInterval(5_ms);
    bool led_up = true;
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

        if (ws_led.Mark())
        {
            alfalfa::lib::Color color;
            switch (led_sel)
            {
                case 0:
                    color.red = led_cnt;
                    color.green = 0;
                    color.blue = 0;
                break;
                case 1:
                    color.red = 0;
                    color.green = led_cnt;
                    color.blue = 0;
                break;
                case 2:
                    color.red = 0;
                    color.green = 0;
                    color.blue = led_cnt;
                break;
                case 3:
                    color.red = led_cnt;
                    color.green = led_cnt;
                    color.blue = led_cnt;
                break;
                default:
                    led_sel = 0;
                break;
            }
            std::array<alfalfa::lib::Color, 8> colors;
            colors.fill(color);
            ws2812b.SetColor(colors);
            led_cnt += led_up ? +1 : -1;
            switch (led_cnt)
            {
                case 0x00:
                    led_up = true;
                    led_sel += 1;
                    led_sel %= 4;
                break;
                case 0xFF:
                    led_up = false;
                break;
                default:
                break;
            }
        }

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

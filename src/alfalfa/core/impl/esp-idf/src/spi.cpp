#include <alfalfa/impl/esp-idf/hal/spi.hpp>


namespace alfalfa
{
namespace hal
{
namespace impl
{
namespace spi
{
    Slave::Slave(
        std::shared_ptr<Module> module,
        std::optional<alfalfa::hal::Pin>&& chip_select,
        int clock_speed_hz)
    : _module(std::move(module))
    {
        spi_device_interface_config_t config = {
            .mode = 0,
            .clock_speed_hz = clock_speed_hz,
            .spics_io_num = chip_select ? static_cast<int>(chip_select->IoIndex()) : -1,
            .queue_size = 1
        };
        auto err = spi_bus_add_device(_module->_host, &config, &_dev_handle);
        if (err != ESP_OK)
        {
            std::string detail("Failed to add SPI device: ");
            detail += esp_err_to_name(err);
            throw std::runtime_error(detail);
        }
    }

    void Slave::Transceive(const uint8_t data)
    {
        spi_device_acquire_bus(_dev_handle, portMAX_DELAY);
        spi_transaction_t t = {};
        t.length = 8;
        t.tx_buffer = &data;
        auto err = spi_device_transmit(_dev_handle, &t);
        spi_device_release_bus(_dev_handle);
        if (err != ESP_OK)
        {
            std::string detail("Failed to transmit SPI data: ");
            detail += esp_err_to_name(err);
            throw std::runtime_error(detail);
        }
    }

    void Slave::Transceive(const uint8_t data, uint8_t& target)
    {
        spi_device_acquire_bus(_dev_handle, portMAX_DELAY);
        spi_transaction_t t = {};
        t.length = 8;
        t.tx_buffer = &data;
        t.rx_buffer = &target;
        auto err = spi_device_transmit(_dev_handle, &t);
        spi_device_release_bus(_dev_handle);
        if (err != ESP_OK)
        {
            std::string detail("Failed to transmit SPI data: ");
            detail += esp_err_to_name(err);
            throw std::runtime_error(detail);
        }
    }

    void Slave::Transceive(const std::span<const uint8_t> data)
    {
        spi_device_acquire_bus(_dev_handle, portMAX_DELAY);
        spi_transaction_t t = {};
        t.length = data.size() * 8;
        t.tx_buffer = data.data();
        auto err = spi_device_transmit(_dev_handle, &t);
        spi_device_release_bus(_dev_handle);
        if (err != ESP_OK)
        {
            std::string detail("Failed to transmit SPI data: ");
            detail += esp_err_to_name(err);
            throw std::runtime_error(detail);
        }
    }

    void Slave::Transceive(const std::span<const uint8_t> data, std::span<uint8_t> target)
    {
        if (data.size() != target.size())
        {
            throw std::invalid_argument("Data and target spans must have the same size");
        }
        spi_device_acquire_bus(_dev_handle, portMAX_DELAY);
        spi_transaction_t t = {};
        t.length = data.size() * 8;
        t.tx_buffer = data.data();
        t.rx_buffer = target.data();
        auto err = spi_device_transmit(_dev_handle, &t);
        spi_device_release_bus(_dev_handle);
        if (err != ESP_OK)
        {
            std::string detail("Failed to transmit SPI data: ");
            detail += esp_err_to_name(err);
            throw std::runtime_error(detail);
        }
    }

    Module::Module(
        spi_host_device_t host,
        std::optional<::alfalfa::hal::Pin>&& miso,
        std::optional<::alfalfa::hal::Pin>&& mosi,
        std::optional<::alfalfa::hal::Pin>&& clk,
        int max_tx_size)
    : _host(host)
        , _miso(std::move(miso))
        , _mosi(std::move(mosi))
        , _clk(std::move(clk))
    {
        spi_bus_config_t bus_config = {
            .mosi_io_num = static_cast<int>(_mosi ? _mosi->IoIndex() : -1),
            .miso_io_num = static_cast<int>(_miso ? _miso->IoIndex() : -1),
            .sclk_io_num = static_cast<int>(_clk ? _clk->IoIndex() : -1),
            .max_transfer_sz = max_tx_size
        };
        auto err = spi_bus_initialize(_host, &bus_config, SPI_DMA_CH_AUTO);
        if (err != ESP_OK)
        {
            throw std::runtime_error("Failed to initialize SPI bus");
        }
    }

}
}
}
}

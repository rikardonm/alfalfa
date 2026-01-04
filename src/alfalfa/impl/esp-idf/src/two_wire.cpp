#include <alfalfa/impl/hal/two_wire.hpp>

#include <alfalfa/lib/log.hpp>
#include <alfalfa/oal/timekeeper.hpp>


auto logger = ::alfalfa::logging::Logger("TwoWire");


using namespace Units::Literals;


namespace alfalfa
{
namespace hal
{
namespace two_wire
{

    void CheckAndThrow(const esp_err_t err, const char* message)
    {
        if (err != ESP_OK)
        {
            const std::string msg2 = std::format("{} ({:#04x}: {})", message, err, esp_err_to_name(err));
            throw TwoWireException(msg2, alfalfa::ErrorCode::LowerLevelError, err);
        }
    }

    BusMaster::BusMaster(
        const i2c_port_t module_num,
        const ::alfalfa::hal::Pin scl,
        const ::alfalfa::hal::Pin sda,
        const Units::Hertz frequency,
        const Units::Second timeout)
        : bus_frequency(frequency)
        , transaction_timeout(timeout)
    {
        if (module_num >= I2C_NUM_MAX)
        {
            throw TwoWireException("Invalid I2C port number", alfalfa::ErrorCode::InvalidValue);
        }

        i2c_master_bus_config_t i2c_bus_config = {
            .i2c_port = module_num,
            .sda_io_num = static_cast<gpio_num_t>(sda.IoIndex()),
            .scl_io_num = static_cast<gpio_num_t>(scl.IoIndex()),
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = false,
                .allow_pd = false
            }
        };

        CheckAndThrow(i2c_new_master_bus(&i2c_bus_config, &bus_handle), "Failed to create I2C master bus");
        i2c_device_config_t i2c_dev_config = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = I2C_DEVICE_ADDRESS_NOT_USED, // Address will be set per-transaction
            .scl_speed_hz = static_cast<uint32_t>(bus_frequency.UnderlyingValue()),
            .scl_wait_us = 0,
            .flags = {
                .disable_ack_check = false,
            }
        };
        CheckAndThrow(i2c_master_bus_add_device(bus_handle, &i2c_dev_config, &dev_handle), "Failed to add I2C device");
    }

    BusMaster::~BusMaster()
    {
        i2c_master_bus_rm_device(dev_handle);
        i2c_del_master_bus(bus_handle);
    }

    void BusMaster::Execute(trx::Collection collection)
    {
        const auto terminate_read = [](std::vector<i2c_operation_job_t>& jobs){
            if (jobs.size() == 0)
            {
                return;
            }
            if (jobs.back().command != I2C_MASTER_CMD_READ)
            {
                return;
            }
            auto& og_read = jobs.back().read;
            if (og_read.ack_value == I2C_NACK_VAL)
            {
                return;
            }
            if (og_read.total_bytes == 1)
            {
                og_read.ack_value = I2C_NACK_VAL;
                return;
            }
            og_read.total_bytes -= 1;
            jobs.emplace_back(i2c_operation_job_t{
                .command = I2C_MASTER_CMD_READ,
                .read = {
                    .ack_value = I2C_NACK_VAL,
                    .data = og_read.data + og_read.total_bytes,
                    .total_bytes = 1
                }
            });
        };

        std::vector<i2c_operation_job_t> jobs;
        jobs.reserve(collection.commands.size());
        for (auto& item : collection.commands)
        {
            if (not std::holds_alternative<trx::_ReadCommand>(item))
            {
                terminate_read(jobs);
            }

            if (std::holds_alternative<trx::_WriteCommand>(item))
            {
                auto& data = std::get<trx::_WriteCommand>(item).data;
                jobs.emplace_back(i2c_operation_job_t{
                    .command = I2C_MASTER_CMD_WRITE,
                    .write = {
                        .ack_check = false,
                        .data = reinterpret_cast<uint8_t*>(const_cast<std::byte*>(data.data())),
                        .total_bytes = data.size()
                    }
                });
            }
            else if (std::holds_alternative<trx::_ReadCommand>(item))
            {
                auto& data = std::get<trx::_ReadCommand>(item).data;
                auto data_ptr = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data.data()));
                jobs.emplace_back(i2c_operation_job_t{
                    .command = I2C_MASTER_CMD_READ,
                    .read = {
                        .ack_value = I2C_ACK_VAL,
                        .data = data_ptr,
                        .total_bytes = data.size()
                    }
                });
            }
            else if (std::holds_alternative<trx::Start>(item))
            {
                jobs.emplace_back(i2c_operation_job_t{.command = I2C_MASTER_CMD_START, .write = {}});
                auto& start = std::get<trx::Start>(item);
                jobs.emplace_back(i2c_operation_job_t{
                    .command = I2C_MASTER_CMD_WRITE,
                    .write = {
                        .ack_check = false,
                        .data = reinterpret_cast<uint8_t*>(const_cast<std::byte*>(&start.address_command)),
                        .total_bytes = 1
                    }
                });
            }
            else if (std::holds_alternative<trx::Stop>(item))
            {
                jobs.emplace_back(i2c_operation_job_t{.command = I2C_MASTER_CMD_STOP, .write = {}});
            }
            else
            {
                throw std::invalid_argument("Invalid TwoWire transaction command variant");
            }
        }

        auto err = i2c_master_execute_defined_operations(
            dev_handle,
            jobs.data(),
            static_cast<size_t>(jobs.size()),
            static_cast<int>(transaction_timeout.DecayTo(std::milli())));
        CheckAndThrow(err, "Failed to execute I2C transaction");
    }

}
}
}

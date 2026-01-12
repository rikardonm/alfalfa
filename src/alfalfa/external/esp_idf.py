import typing as t

import lobs
from lobs.exporters.esp_idf import exporter as esp_idf, nvs
import alfalfa


class Framework(lobs.Package, tag="esp-idf-framework"):
    nvs_table = lobs.Parameter[nvs.Table | None | t.Literal['single-app', 'single-app-large']](default=None)
    """NVS partition table configuration.
    If set to 'single-app' or 'single-app-large', uses the built-in partition tables."""
    memory_size = lobs.Parameter[lobs.Storage | t.Literal['1MB', '2MB', '4MB', '8MB', '16MB', '32MB', '64MB', '128MB']](default=None)
    """Physical flash memory size configuration."""
    cpp_exceptions_enabled = lobs.Parameter[bool](default=False)
    """Whether C++ exceptions are enabled."""
    cpp_exceptions_pool_size = lobs.Parameter[int | None](default=None)
    """Size of the emergency pool for C++ exceptions, in bytes. If None, uses the default value."""
    compiler_optimization_level = lobs.Parameter[t.Literal['none', 'debug', 'size', 'perf'] | None](default=None)
    """Compiler optimization level for the bootloader."""
    cpu_default_frequency_mhz = lobs.Parameter[t.Literal['80MHz', '160MHz', '240MHz'] | None](default=None)
    """Default CPU core frequency."""
    cpu_target = lobs.Parameter[t.Literal['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c2', 'esp32c6', 'esp32h2']](default='esp32')
    """Target CPU architecture."""
    bootloader_rollback_enabled = lobs.Parameter[bool](default=False)
    """Whether bootloader rollback protection is enabled."""
    bootloader_log_level = lobs.Parameter[t.Literal['none', 'error', 'warn', 'info', 'debug', 'verbose'] | None](default='warn')
    """Bootloader log verbosity level."""
    bootloader_compiler_optimization_level = lobs.Parameter[t.Literal['none', 'debug', 'size', 'perf'] | None](default='size')
    """Compiler optimization level for the bootloader."""
    # ESP_CONSOLE_USB_SERIAL_JTAG && ESP_CONSOLE_UART_DEFAULT
    driver_default_log_level = lobs.Parameter[t.Literal['none', 'error', 'warn', 'info', 'debug', 'verbose'] | None](default='warn')
    """Driver log verbosity level."""

    app = lobs.cpp.project.SimpleLibrary()

    def get_exporter_configuration(self, config_type: type[lobs.exporter.TCFG], gen_path: lobs.Path) -> lobs.exporter.TCFG | None:
        assert config_type is esp_idf.EspIdfConfig
        cfg = config_type()

        match self.nvs_table.value:
            case None:
                raise ValueError("NVS table configuration must be specified.")
            case 'single-app':
                cfg.config_flags.update({
                    "CONFIG_PARTITION_TABLE_SINGLE_APP": True,
                    "CONFIG_PARTITION_TABLE_SINGLE_APP_LARGE": False,
                    "CONFIG_PARTITION_TABLE_TWO_OTA": False,
                })
            case nvs.Table():
                nvs_table_path = gen_path / "partitions.csv"
                self.nvs_table.value.export(nvs_table_path)
                offset = self.nvs_table.value.offset or 0x8000
                cfg.config_flags.update({
                    # Explicitly disable "pre-baked" partitions
                    "CONFIG_PARTITION_TABLE_SINGLE_APP": False,
                    "CONFIG_PARTITION_TABLE_SINGLE_APP_LARGE": False,
                    "CONFIG_PARTITION_TABLE_TWO_OTA": False,
                    # Enable and configure custom
                    "CONFIG_PARTITION_TABLE_CUSTOM": True,
                    "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME": nvs_table_path,
                    "CONFIG_PARTITION_TABLE_FILENAME": nvs_table_path,
                    "CONFIG_PARTITION_TABLE_OFFSET": lambda offset=offset: hex(offset),  # 32kiB seems to be default
                    "CONFIG_PARTITION_TABLE_MD5": True,
                })
            case _:
                raise NotImplementedError(f"Unsupported NVS table configuration: {self.nvs_table.value}")

        if mem_size := self.memory_size.value:
            if isinstance(mem_size, str):
                size_mib = int(mem_size[:-2])
                size_dec = 1
            else:
                size_mib, size_dec = self.memory_size.value.MIB.decimal_value.as_integer_ratio()
                if size_dec != 1:
                    raise ValueError("Memory size must be specified in whole MiB units.")
            mem_values = (1, 2, 4, 8, 16, 32, 64, 128)
            if size_mib not in mem_values:
                raise ValueError(f"Unsupported memory size specified. Supported sizes are: {mem_values}.")
            cfg.config_flags.update({
                # Flash size configuration, goes along with the partition table configuration
                "CONFIG_ESPTOOLPY_FLASHSIZE": f"{size_mib}MB",
                f"CONFIG_ESPTOOLPY_FLASHSIZE_{size_mib}MB": True,
            })

        # Enable C++ exceptions
        if self.cpp_exceptions_enabled.value:
            cfg.config_flags.update({"CONFIG_COMPILER_CXX_EXCEPTIONS": True})
            if pool_size := self.cpp_exceptions_pool_size.value:
                cfg.config_flags.update({"CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE": pool_size})

        # Compiler optimization level
        if v := self.compiler_optimization_level.value:
            cfg.config_flags.update({
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG": v == 'debug',
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_SIZE": v == 'size',
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_PERF": v == 'perf',
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE": v == 'none',
            })

        # CPU core frequency
        if True:
            v = self.cpu_default_frequency_mhz.value
            if isinstance(v, float):
                v = int(v)
            cfg.config_flags.update({
                "CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_80": v == 80e6 or v == '80MHz',
                "CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_160": v == 160e6 or v == '160MHz',
                "CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240": v == 240e6 or v == '240MHz',
                "CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ": v if isinstance(v, int) else 0,
            })

        # CPU target
        if True:
            v = self.cpu_target.value
            cfg.config_flags.update({
                "CONFIG_IDF_TARGET_ESP32": v == 'esp32',
                "CONFIG_IDF_TARGET_ESP32S2": v == 'esp32s2',
                "CONFIG_IDF_TARGET_ESP32S3": v == 'esp32s3',
                "CONFIG_IDF_TARGET_ESP32C3": v == 'esp32c3',
                "CONFIG_IDF_TARGET_ESP32C2": v == 'esp32c2',
                "CONFIG_IDF_TARGET_ESP32C6": v == 'esp32c6',
                "CONFIG_IDF_TARGET_ESP32H2": v == 'esp32h2',
                "CONFIG_IDF_TARGET": v,
            })

        # Bootloader rollback protection
        if self.bootloader_rollback_enabled.value:
            cfg.config_flags.update({
                "CONFIG_BOOTLOADER_ROLLBACK_PROT": True,
            })

        if v := self.bootloader_log_level.value:
            idx = ['none', 'error', 'warn', 'info', 'debug', 'verbose'].index(v)
            cfg.config_flags.update({
                "CONFIG_BOOTLOADER_LOG_LEVEL_NONE": v == 'none',
                "CONFIG_BOOTLOADER_LOG_LEVEL_ERROR": v == 'error',
                "CONFIG_BOOTLOADER_LOG_LEVEL_WARN": v == 'warn',
                "CONFIG_BOOTLOADER_LOG_LEVEL_INFO": v == 'info',
                "CONFIG_BOOTLOADER_LOG_LEVEL_DEBUG": v == 'debug',
                "CONFIG_BOOTLOADER_LOG_LEVEL_VERBOSE": v == 'verbose',
                "CONFIG_BOOTLOADER_LOG_LEVEL": idx,
            })

        if v := self.bootloader_compiler_optimization_level.value:
            cfg.config_flags.update({
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE": v == 'none',
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG": v == 'debug',
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_SIZE": v == 'size',
                "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_PERF": v == 'perf',
            })

        if v := self.driver_default_log_level.value:
            idx = ['none', 'error', 'warn', 'info', 'debug', 'verbose'].index(v)
            cfg.config_flags.update({
                "CONFIG_LOG_DEFAULT_LEVEL_NONE": v == 'none',
                "CONFIG_LOG_DEFAULT_LEVEL_ERROR": v == 'error',
                "CONFIG_LOG_DEFAULT_LEVEL_WARN": v == 'warn',
                "CONFIG_LOG_DEFAULT_LEVEL_INFO": v == 'info',
                "CONFIG_LOG_DEFAULT_LEVEL_DEBUG": v == 'debug',
                "CONFIG_LOG_DEFAULT_LEVEL_VERBOSE": v == 'verbose',
                "CONFIG_LOG_DEFAULT_LEVEL": idx,
                "CONFIG_LOG_MAXIMUM_EQUALS_DEFAULT": True,
                "CONFIG_LOG_MAXIMUM_LEVEL": idx,
            })

        return cfg


Framework.require(alfalfa.Package.implementation, "esp-idf")

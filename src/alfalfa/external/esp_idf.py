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

    app = lobs.cpp.project.SimpleLibrary()

    def get_exporter_configuration(self, config_type: type[lobs.exporter.TCFG], gen_path: lobs.Path) -> lobs.exporter.TCFG | None:
        assert config_type is esp_idf.EspIdfConfig
        cfg = config_type()
        if not isinstance(self.nvs_table.value, nvs.Table):
            raise NotImplementedError("Only custom NVS table configurations are supported at the moment.")
        else:
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

        return cfg


Framework.require(alfalfa.Package.implementation, "esp-idf")

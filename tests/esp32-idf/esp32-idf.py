from pathlib import Path

import lobs
from lobs.exporter.esp_idf import EspIdfConfig
import alfalfa


_cd = Path(__file__).parent


project = lobs.Package(
    lobs.ProjectMeta(
        name="esp32-idf",
        version=lobs.Version(0, 0, 1),
        exporter_configuration=[
            EspIdfConfig(sdk_config_default=_cd / "sdkconfig.default"),
        ],
    ),
    lobs.cpp.ManagedApplication(
        (_cd / "main").glob("*.cpp"),
        include_dirs=[_cd / "inc"],
    ),
    dependencies=[
        alfalfa.package,
    ],
)

# SPDX-FileCopyrightText: 2025-present Ricardo Marchesan <ricardo@azevem.com>
#
# SPDX-License-Identifier: MIT
# flake8: noqa: F401
# pyright: reportUnusedImport = false
import lobs

import typing as t
import annotated_types as ant
import lobs.exporters

import alfalfa.external.nlohmann_json

from . import version

_cd = lobs.Path(__file__).parent
src = _cd / "src"


class Package(
    lobs.Package,
    version=lobs.Version.parse(version.__version__),
    description="Alfalfa C++ Framework",
    tag="alfalfa",
):
    implementation = lobs.Parameter[t.Literal["esp-idf", "linux"]](default=None)
    """The desired platform implementation to use."""

    @t.override
    def _make_project(self, basepath: lobs.Path):
        proj = lobs.cpp.project.SimpleLibrary(
            public_includes=[_cd / "inc"],
            source_files=[
                src / "abm" / "console.cpp",
                src / "bal" / "ina3221.cpp",
                src / "bal" / "led.cpp",
                src / "bal" / "mcp23017.cpp",
                src / "lib" / "types" / "triplet-version.cpp",
                src / "lib" / "log.cpp",
                src / "oal" / "timekeeper.cpp",
            ],
            linked_libraries=[
                alfalfa.external.nlohmann_json.NlohmannJsonPackage,
            ]
        )
        assert isinstance(proj.source_files, list)
        assert isinstance(proj.public_includes, list)
        match self.implementation.value:
            case "esp-idf":
                impl_dir = "esp-idf"
            case "linux":
                impl_dir = "linux"
            case _:
                raise ValueError(f"Unsupported Alfalfa implementation: {self.implementation.value}")
        proj.source_files.extend(list((_cd / "impl" / impl_dir / "src").glob("*.cpp")))
        proj.public_includes.append(_cd / "impl" / impl_dir / "inc")
        return proj

    def get_exporter_configuration(self, config_type: type[lobs.exporter.TCFG], gen_path: lobs.Path) -> lobs.exporter.TCFG | None:
        if issubclass(config_type, lobs.exporters.esp_idf.EspIdfConfig):
                return config_type(
                    esp_components=['esp_timer', 'driver'],
                )
        return None

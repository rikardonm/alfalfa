import lobs
import alfalfa.core
from alfalfa.core.external import esp_idf

_cwd = lobs.Path(__file__).parent


class Board(lobs.Package):
    framework = esp_idf.Framework

    app = lobs.cpp.project.SimpleLibrary(
        public_includes=[_cwd / "inc"],
        source_files=[_cwd / "esp32_c3_super_mini.cpp"],
        linked_libraries=[
            alfalfa.core.Package,
            esp_idf.Framework,
        ]
    )


Board.require(esp_idf.Framework.cpu_target, 'esp32c3')
Board.require(esp_idf.Framework.memory_size, '4MB')

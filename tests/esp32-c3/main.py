import lobs
import alfalfa.core
from alfalfa.core.boards.esp32_c3_super_mini import Board as _Board

_cwd = lobs.Path(__file__).parent


class Main(lobs.Package):
    app = lobs.cpp.project.SimpleLibrary(
        source_files=[ _cwd / "main.cpp"],
        linked_libraries=[
            alfalfa.core.Package,
            _Board,
        ],
    )


Main.require(_Board.framework.nvs_table, 'single-app')
Main.require(_Board.framework.cpp_exceptions_enabled, True)
Main.require(_Board.framework.cpp_exceptions_pool_size, 4096)

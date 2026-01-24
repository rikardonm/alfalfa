import lobs
import alfalfa
from alfalfa.boards.esp32_c3_super_mini import Board as _Board

_cwd = lobs.Path(__file__).parent


class Main(lobs.Package):
    app = lobs.cpp.project.SimpleLibrary(
        source_files=[ _cwd / "main.cpp"],
        linked_libraries=[
            alfalfa.Package,
            _Board,
        ],
    )


Main.require(_Board.framework.nvs_table, 'single-app')
Main.require(_Board.framework.cpp_exceptions_enabled, True)
Main.require(_Board.framework.cpp_exceptions_pool_size, 4096)

import lobs

import main


class App(lobs.Package):
    app = lobs.cpp.project.EmbeddedApplication(
        linked_libraries=[main.Main]
    )

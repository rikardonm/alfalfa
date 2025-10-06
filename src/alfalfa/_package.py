from pathlib import Path

import lobs

from . import version

_cd = Path(__file__).parent


pkg = lobs.Package(
    lobs.ProjectMeta(
        "alfalfa",
        lobs.Version.parse(version.__version__),
    ),
    lobs.cpp.project.Library(
        include_dirs=[_cd / "inc"],
    ),
)

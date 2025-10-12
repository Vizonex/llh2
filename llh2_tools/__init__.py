"""
llh2_tools
----------

Used for generating C code for making the llh2 library exist.

"""

import sys
from pathlib import Path
from typing import NamedTuple

from llparse import LLParse

from .llh2 import H2Parser, buildCHeaders


class Version(NamedTuple):
    major: int  # Rewrites
    minor: int  # new additions
    patch: int  # Bug fixes

    def as_str(self) -> str:
        return f"{self.major}.{self.minor}.{self.patch}"


__llh2_version__ = Version(0, 0, 1)

VERSION_HEADER = f"""
#ifndef INCLUDE_LLH2_VERSION_H_
#define INCLUDE_LLH2_VERSION_H_

#define LLH2_VERSION_MAJOR {__llh2_version__.major}
#define LLH2_VERSION_MINOR {__llh2_version__.minor}
#define LLH2_VERSION_PATCH {__llh2_version__.patch}

#endif /* INCLUDE_LLH2_VERSION_H_ */
"""


def build_code():
    """Generates internal code for llh2"""

    llh2 = LLParse("llh2__internal")

    p = H2Parser(llh2)
    src = llh2.build(
        p.build(),
        header_name="llh2",
        headerGuard="INCLUDE_LLH2_ITSELF_H_",
        debug="llh2__internal_debug" if "--debug" in sys.argv else None,
    )
    build_folder = Path("src")
    
    # create directories for ungenerated scripts...
    if not build_folder.exists():
        build_folder.mkdir()

    include = Path("include")
    if not include.exists():
        include.mkdir()


    (build_folder / "llh2.c").write_text(src.c)
    (include / "llh2.h").write_text(
        VERSION_HEADER + buildCHeaders() + src.header + "\n" + p.do_build()
    )
    (build_folder / "api.c").write_text(p.build_api())

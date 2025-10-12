from setuptools import Extension, setup
from Cython.Build import cythonize
from pathlib import Path

# made for setting up a dummy cython tests for testing if llh2 works correctly.
import os

PATH = Path(os.getcwd())

setup(
    ext_modules=cythonize(
        [
            Extension(
                "tests.h2parser",
                sources=[
                    "tests/h2parser.pyx",
                    str(PATH / "src" / "llh2.c"),
                    str(PATH / "src" / "api.c"),
                    str(PATH / "native" / "http2.c"),
                ],
                include_dirs=[str(PATH / "include")],
            )
        ]
    ),
)

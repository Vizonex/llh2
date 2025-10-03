# Pythonic workflow for building C Releases
# This is an attempted mirror of llhttp's typescript version
# as a parody of llhttp/bin/generate.ts
from __future__ import annotations # incase of 3.9...
from typing import NamedTuple
from pathlib import Path
from llparse import LLParse
from llh2 import H2Parser, buildCHeaders
import sys
import subprocess
import shutil
import gzip
import tarfile
import zipfile
import os
from hashlib import sha256, sha1
from _hashlib import HASH # typehinting

# This is going to be controversal but I think mesonbuild is better at this job than
# CMakelists CMakelists is good but it lacks simplicity.


# For verification of the releases I will include hashing to verify that these are in fact the releases.

def hash_digest_file(_hash:HASH, file:Path | str, bits:int = 1024):
    """digests a file for hash verification"""
    with open(file, "rb") as r:
        while buf :=  r.read(bits):
            _hash.update(buf)
    




class Version(NamedTuple):
    major:int # Rewrites
    minor:int # new additions
    patch:int # Bug fixes

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
    llh2 = LLParse("llh2__internal")

    p =  H2Parser(llh2)
    src = llh2.build(
        p.build(),    
        header_name="llh2",
        headerGuard="INCLUDE_LLH2_ITSELF_H_",
        debug="llh2__internal_debug" if "--debug" in sys.argv else None,
    )
    build_folder = Path("src")
    include = Path("include")

    (build_folder / "llh2.c").write_text(src.c)
    (include / "llh2.h").write_text(VERSION_HEADER + buildCHeaders() + src.header + "\n" + p.do_build())
    (build_folder / "api.c").write_text(p.build_api())

def build_meson():
    """Compiles and then obtains binary releases from a 
    temporary folder and then compiles to static and dynamic files 
    """
    if not os.path.exists("build-tmp"):
        subprocess.Popen(f"meson setup build-tmp --buildtype {'release' if '--debug' not in sys.argv else 'debug'}", cwd=Path(__file__).parent).wait()

    # we will save an execution step by using cwd argument aka current working directory to help
    subprocess.Popen("meson compile --verbose", cwd=os.path.join(os.getcwd(), "build-tmp")).wait()

    # We now move on to creating and reorganizing the release.
    btemp =  Path("build-tmp")
    static_name = btemp / "llh2.a" # will rename for windows later...
    dynamic_name = btemp / ("llh2.dll" if sys.platform == "win32" else "llh2.so")
    is_windows = sys.platform == "win32"

    # Binary files
    bin_path = Path("bin")

    STATIC_DIR = (bin_path / "static")
    DYNAMIC_DIR = (bin_path / "dynamic")
    if not bin_path.exists():
        bin_path.mkdir()

    if not STATIC_DIR.exists():
        STATIC_DIR.mkdir()

    if not DYNAMIC_DIR.exists():
        DYNAMIC_DIR.mkdir()
    
    # Move files, we need will cleanup the rest when this section finishes...
    shutil.move(static_name,  STATIC_DIR / ("llh2.a" if not is_windows else "llh2.lib"))

    if is_windows:
        # add dll and other stuff
        shutil.move(dynamic_name, DYNAMIC_DIR / "llh2.dll")
        shutil.move(btemp / "llh2.exp", bin_path / "llh2.exp")
        # lib file for dll...
        shutil.move(btemp / "llh2.lib", DYNAMIC_DIR / "llh2.lib")

    else:
        # I'll reorganize more bin files once I have a good idea of what
        # other os types look like exactly...
        shutil.move(dynamic_name, DYNAMIC_DIR / "llh2.so")
       
    # is there anything else that we want from build-tmp? if it's a file that starts with llh2\.*
    # better take it now before we initiate cleanup later
    for f in btemp.iterdir():
        if f.is_file() and f.name.startswith('llh2.'):
            # Move to bin path
            shutil.move(f, bin_path / f.name)
        
    # CLEANUP build-tmp if we fail that's ok too I guess :/

    shutil.rmtree(btemp, ignore_errors=True)
    return bin_path




def hash_all_files(_hash:HASH, binpath: Path):
    # get module location it's the safest way to ensure we get to the correct path
    cwd = Path(__file__).parent
    
    # binaries
    for i in binpath.iterdir():
        if i.is_file():
            hash_digest_file(_hash, i)
        else:
            for f in i.iterdir():
                # incase somehow untruthy.
                if f.is_file():
                    hash_digest_file(_hash, f)

    # code
    hash_digest_file(_hash, cwd / "include" / "llh2.h")
    hash_digest_file(_hash, cwd / "native" / "http2.c")
    hash_digest_file(_hash, cwd / "src" / "api.c")
    hash_digest_file(_hash, cwd / "src" / "llh2.c")
    
    # Licensing and builds
    hash_digest_file(_hash, cwd / "LICENSE")
    hash_digest_file(_hash, cwd / "meson.build")
    hash_digest_file(_hash, cwd / "llh2.pxd") # Cython bindings if anyone wants them...
    hash_digest_file(_hash, cwd / "CMakeLists.txt")

    return _hash.hexdigest()




# See: https://stackoverflow.com/a/1855118 I added typehinting as a special edit
def zipdir(path: Path, ziph: zipfile.ZipFile):
    # ziph is zipfile handle
    for root, _, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file), 
                       os.path.relpath(os.path.join(root, file), 
                                       os.path.join(path, '..')))


def build_archive():
    """Obtains compiled binaries (static & DLL) from meson build and then reconverts them to
    proper names if on windows then releases are rearranged in this order
    There's several archive formats for it's releases. We get zip format 
    and tar.gz and tar.

    include:
        - llh2.h
    src:
        - api.c
        - llh2.c

    native:
        - http2.c
    
    bin:
        - static:
            - llh2.lib or llh2.a
        - dynamic:
            - llh2.lib or llh2.a
            - llh2.dll or llh2.so
        - llh2.pdb (Windows)
    - LICENSE (MIT)
    - README.md
    - meson.build

    - CMakeLists.txt 
        - WARNING: (This Does not compile dynamic and assumes 
            you want static builds)

    - verify.py (Hash verification if needed) (Todo: Maybe in another update...)
    - build.sha1 (Sha1 hash)
    - build.sha256 (sha256 hash)
    """
    # Step 1: build the code...
    build_code()

    bin_dir = build_meson()

    cwd = Path(__file__).parent

    RELEASE_NAME = f'llh2-{sys.platform}-{__llh2_version__.as_str()}'
    with zipfile.ZipFile(RELEASE_NAME + ".zip", 'w') as z:
        zipdir(bin_dir, z)
        # code
        zipdir(cwd / "include", z)
        zipdir(cwd / "native", z)
        zipdir(cwd / "src", z)
        # Licensing and builds
        z.write(cwd / "LICENSE", "LICENSE")
        z.write(cwd / "meson.build", "meson.build")
        z.write(cwd / "llh2.pxd", "llh2.pxd") # Cython bindings if anyone wants them...
        z.write(cwd / "CMakeLists.txt", "CMakeLists.txt")
        z.writestr('build.sha1', hash_all_files(sha1(), bin_dir))
        z.writestr('build.sha256', hash_all_files(sha256(), bin_dir))




if __name__ == "__main__":
    build_archive()
    

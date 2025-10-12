# llh2

Frame Parser for the http/2 protocol with the objective of parsing frames and unpacking incoming frames
based off llhttp's structural design and approch. You may notice that were using python and this is 
becuase of some unimplemented features in the typescript version that have yet to be introduced. 
Work is based off RFC 9113 with the goal of being RFC Compliant.



# How to compile

Unlike llhttp this requires python 3.9 or higher.

## Step 1 install python version of llparse 
```
pip install -r requirements.txt
```

## Step 2 run script

```
python llh2.py
```
A New version of llh2.c and llh2.h should appear in the build directory if all was done correctly.

## TODOS
- A workflow simillar to llhttp for releases.


# Batteries included (Pun intended)

## Binary Data Included Stress free
Releases will always include binary data for both static and dynamic artifacts as compiling these is an annoying step when performing any installation of any C Library. If this is a security concern for you, hashes are included to verify these releases as well as tools for building releases as well as Meson and CMakeLists.txt file for building your own. Meson is used by default for all binary compilations incase this concerns you.

## Cython bindings
Copying off the cython file should be enough to help you bind the library straight on over to python however know that the 
binary data should not be relied on and you should look at setup_tests.py for the sources you need to gather when compiling which are as follows 

### Sources
- native/http2.c
- src/api.c
- src/llh2.c

### Includes
- include/llh2.h

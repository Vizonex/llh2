# llh2

Frame Parser for the http/2 protocol with the objective of parsing frames and unpacking incoming frames
based off llhttp's structural design and approch. You may notice that were using python and this is 
becuase of some unimplemented features in the typescript version that have yet to be introduced.

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

CMakeLists.txt file coming soon.

# diggable
diggable provides a graphical interface to the books provided by the matchmaker library<br/>
Note that matchmaker's book feature is new and that currently only one book is offered<br/>
For information about matchmaker see https://github.com/shtroizel/matchmaker

## building from source
diggable can be built with modest hardware. However, matchmaker, the library that powers diggable, will
require roughly 4GB of RAM per CPU core to compile - but even when using only 1 core, the total RAM + swap
should be at least 24GB. If you do not have enough RAM, consider getting the AppImage instead of trying
to build from source.

### dependencies
diggable is developed on Artix Linux
<br/>
```
pacman -Sy base-devel cmake python fltk ttf-dejavu
```

### clone and initialize matchmaker
for this example we will assume a username of "shtroizel" and will clone both matchmaker and diggable
into /home/shtroizel/repos - adjust the clone and/or build and/or install paths as needed.
```
cd /home/shtroizel
mkdir repos
cd repos
git clone https://github.com/shtroizel/matchmaker.git
cd matchmaker
git submodule update --init --recursive
```

### building matchmaker
I recommend using dates for build and install directories, but of course you can name these
however you like. I also recommend using the "-q" option to not only significantly reduce the compile
time, but also to reduce diggable's startup time. For a relocatable installation, install both matchmaker
and diggable into the same location. From within /home/shtroizel/repos/matchmaker (or wherever you cloned
yours) and adjusting for today's date or whatever convention you prefer, do something like:
```
./scripts/build_and_install.py -b build_2023-05-29 -i /home/shtroizel/diggable_2023-05-29 -q
```
The build will progress through a few layers of code generation taking several hours (or even days),
depending on your hardware. I recommend 4G RAM / cpu core. Use the "-j" option to specify # of cpu cores.
run "./scripts/build_and_install.py -h" for more help building matchmaker.

### clone diggable
```
cd /home/shtroizel/repos
git clone https://github.com/shtroizel/diggable.git
```

### build and install diggable
For this example we will build in /home/shtroizel/repos/diggable/build_2023-05-29 and install to
/home/shtroizel/diggable_2023-05-29. Of course change this to whatever you like, but note that diggable
must be installed to work properly - running diggable from the build directory is not supported. Also,
if you used the -q option when building matchmaker (recommended), then make sure you also specify -Dq=1
on your cmake line here (this is not deduced from the matchmaker_DIR).
```
cd /home/shtroizel/repos/diggable
mkdir build_2023-05-29
cd build_2023-05-29
cmake -DCMAKE_INSTALL_PREFIX=/home/shtroizel/diggable_2023-05-29 -DCMAKE_BUILD_TYPE=Release -Dmatchmaker_DIR=/home/shtroizel/diggable_2023-05-29/lib/matchmaker_q/cmake -Dq=1 ..
make install
```

### running diggable
Always run the installed diggable...
```
/home/shtroizel/diggable_2023-05-29/bin/diggable
```

### updating diggable
Remember when pulling changes to matchmaker to also update submodules - for example:
```
cd /home/shtroizel/repos/diggable
git pull --rebase
cd /home/shtroizel/repos/matchmaker
git pull --rebase
git submodule update --init --recursive
```

## troubleshooting
If fonts are broken, try installing a font usable by fltk - ex: pacman -S ttf-dejavu. The AppImage builds
do not currently include fonts!

## donating to this project
donations are greatly appreciated, thank you!<br>
* monero: 4BETuKtvLjkT7VBW85HLJ1XPqhr1TqQj11UmEMnphgG4DzNtSgJmK8b3ZmTudnbbjf7oHfNpCPxDG3BWnmVSCrtrNXMnhyv<br>

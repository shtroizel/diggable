# diggable
diggable provides a graphical interface to the books provided by the matchmaker library<br/>
note that matchmaker's book feature is new and that currently only one book is offered<br/>
for information about matchmaker see https://github.com/shtroizel/matchmaker

## dependencies
although diggable is developed to run on Artix Linux, diggable might also run on other Artix-like
operating systems
<br/>
matchmaker requires a compiler, CMake, and Python; diggable requires FLTK
```
pacman -Sy base-devel cmake python fltk
```

## clone and initialize matchmaker
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

## building matchmaker
I recommend using dates for build and install directories, but of course you can name these
however you like. I also recommend using the "-q" option to not only significantly reduce the compile
time, but also to reduce diggable's startup time. From within /home/shtroizel/repos/matchmaker (or
wherever you cloned yours) and adjusting for today's date or whatever convention you prefer,
do something like:
```
./scripts/build_and_install.py -b build_2023-01-08 -i /home/shtroizel/matchmaker_2023-01-08 -q
```
The build will progress through a few layers of code generation taking several hours (or even days),
depending on your hardware. I recommend 4G RAM / cpu core. Use the "-j" option to specify # of cpu cores.
run "./scripts/build_and_install.py -h" for more help building matchmaker

## clone diggable
```
cd /home/shtroizel/repos
git clone https://github.com/shtroizel/diggable.git
```

## build and install diggable
For this example we will build in /home/shtroizel/repos/diggable/build_2023-01-08 and install to
/home/shtroizel/repos/diggable/install_2023-01-08. Of course change this to whatever you like, but note
that diggable must be installed to work properly - running diggable from the build directory is not
supported. Also, if you used the -q option when building matchmaker (recommended), then make sure you
also specify -Dq=1 on your cmake line here (this is not deduced from the matchmaker_DIR).
```
cd /home/shtroizel/repos/diggable
mkdir build_2023-01-08
cd build_2023-01-08
cmake -DCMAKE_INSTALL_PREFIX=../install_2023-01-08 -DCMAKE_BUILD_TYPE=Release -Dmatchmaker_DIR=/home/shtroizel/matchmaker_2023-01-08/lib/matchmaker_q/cmake -Dq=1 ..
make install
```

## running diggable
Always run the installed diggable...
```
/home/shtroizel/repos/diggable/install_2023-01-08/bin/diggable
```

## updating diggable
remember when pulling changes to matchmaker to also update submodules - for example:
```
cd /home/shtroizel/repos/diggable
git pull --rebase
cd /home/shtroizel/repos/matchmaker
git pull --rebase
git submodule update --init --recursive
```

## donating to this project
donations are greatly appreciated, thank you!<br>
* monero: 4BETuKtvLjkT7VBW85HLJ1XPqhr1TqQj11UmEMnphgG4DzNtSgJmK8b3ZmTudnbbjf7oHfNpCPxDG3BWnmVSCrtrNXMnhyv<br>

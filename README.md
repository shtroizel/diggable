# diggable
diggable provides a graphical interface to the books provided by the matchmaker library<br/>
note that matchmaker's book feature is new and that currently only one book is offered<br/>
for information about matchmaker see https://github.com/shtroizel/matchmaker

## clone and initialize matchmaker
instructions here will assume a username of "shtroizel" and will clone both matchmaker and diggable
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
however you like. Use the "-q" option to not only significantly reduce the compile time, but also
to reduce diggable's startup time. Also, using the "-q" option will improve the usability of
"term viewer" (middle part of diggable interface), by only completing terms that actually exist
within the book. From within /home/shtroizel/repos/matchmaker (or wherever you cloned yours) and
adjusting for today's date or whatever convention you like, do something like:
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
supported
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

## updates
remember when pulling changes to matchmaker to afterward run:
```
git submodule update --init --recursive
```
this step can be skipped for the diggable repository.

## donating to this project
donations are greatly appreciated, thank you!<br>
* monero: 4BETuKtvLjkT7VBW85HLJ1XPqhr1TqQj11UmEMnphgG4DzNtSgJmK8b3ZmTudnbbjf7oHfNpCPxDG3BWnmVSCrtrNXMnhyv<br>

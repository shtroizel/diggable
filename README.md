# diggable
diggable provides a graphical interface to the books provided by the matchmaker library<br/>
Note that matchmaker's book feature is new and that currently only one book is offered<br/>
For information about matchmaker see https://github.com/shtroizel/matchmaker

## AppImage releases
The easiest way to get diggable is to download an AppImage release, set the x bit and run it. However,
AppImage builds will take longer to start than a build done on your own. Startup times for AppImages
can exceed 1 min (or even a few min) depending on hardware. A self-built diggable will start much faster,
but will still require more time to start than most programs do - so be patient! To run the AppImage on
your favorite Linux distro, do:
```
wget https://github.com/shtroizel/diggable/releases/download/4966-1/diggable-4966-1-x86_64.AppImage
chmod u+x diggable-4966-1-x86_64.AppImage
./diggable-4966-1-x86_64.AppImage
```

## Usage
Despite single-threadedness, diggable is mostly very fast. The exceptions to this are window resizing and
font size changes, which require a second or two of patience. Also, the initial startup will require at
least a few seconds and possibly much longer depending on hardware.<br>

The interface is divided into 3 main columns. From left to right these are the "Book Viewer", "Term Viewer"
and "Location Viewer". Diggable is optimized for mouse input but can also be used on a touch screen. Scroll
any of these areas by using the mouse wheel, left mouse dragging within the text area or by using the
scrollbars. Terms can be selected using the left mouse button. Selected terms are automatically copied to
the selection and copy buffers. The text of multiple terms can be copied (mainly for use in other programs)
using right mouse drag. Right-drag text selections are auto-copied to both the selection and copy buffers.
This is particularly useful for copy/pasting hyperlinks into your browser. Clicking the white input area in
the top of the Term Viewer will paste selection buffer content into the input area (broken though in
4966-1, will be fixed for 4966-2 or build from source to get this feature now).<br>

The white term input area always has keyboard focus. If input has at least 1 char, then the "Completion"
for the input is shown below the white input area. Use 'Enter' to select the first completion entry or
click any result you like to select a term, just as you would in the Book or Location Viewers. Also,
up/down arrow keys can be used to select the prev/next completion result - this is especially useful for
browsing images. To browse images, use 'Esc' to clear any input (if any) and then type three tilds
('~' chars). All images are prefixed with '~~~', so just use up/down arrow to go through them. Images are
shown in the "Term Info" area located in the middle of the Term Viewer between the "Completion" and the
"Term Stack". Clicking an image will enlarge it (or restore it if already enlarged).<br>

The "Term Stack" is a small buffer of terms that is appended to whenever a term is selected. When the
buffer is full, the oldest term is deleted to make room for the new one. The oldest term is the bottom of
the stack. The currently selected term is at the top of the stack. The 'Del' button (or keyboard key) can
be used to revert the last selection and restore the previously selected term. Some non-image terms have
synonyms. If the selected term has any synonyms, then they are shown in the same "Term Info" area where
images are shown. The Location Viewer (right column), shows all of the locations where the currently
selected term appears within the book.<br>

At the bottom of the "Term Viewer", below the "Term Stack", there are 2 rows of 7 buttons. The top of these
rows has buttons for various sorting orders. The bottom row has the following buttons:
<pre>
 !  --> for jump navigation
Del --> delete most recent term added to the term stack and re-select the prev term
Esc --> clear input area at the top of the term viewer
F11 --> toggle fullscreen mode --> recommended to use maximize instead!
 +  --> increase font size (be patient, this is slow!)
 -  --> decrease font size (be patient, this is slow!)
L R --> toggle mouse button inversion (useful for selecting text on touch devices)
</pre>

Although there is no real search support, completion lists come close. The difference is that with
completions, only terms that begin with the given criteria are shown - with real searching, you could also
find results where the middle of a word matches. Also, currently all text is case-sensitive, so typing in
useful completion prefixes like, "How", "Who" and "Why" will return different results than "how", "who" and
"why". This unfortunate sensitivity is being worked on, and will be addressed in future diggable versions.

## building from source
diggable can be built with modest hardware. However, matchmaker, the library that powers diggable, will
require roughly 4GB of RAM per CPU core to compile - but even when using only 1 core, the total RAM + swap
should be at least 24GB. If you do not have enough RAM, consider getting the AppImage instead of trying
to build from source.

### dependencies
diggable is developed on Artix Linux
<br/>
```
pacman -Sy base-devel cmake python fltk
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

## donating to this project
donations are greatly appreciated, thank you!<br>
* monero: 4BETuKtvLjkT7VBW85HLJ1XPqhr1TqQj11UmEMnphgG4DzNtSgJmK8b3ZmTudnbbjf7oHfNpCPxDG3BWnmVSCrtrNXMnhyv<br>

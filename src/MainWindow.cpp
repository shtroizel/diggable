/*
Copyright (c) 2021, Shtroizel
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "MainWindow.h"

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Widget.H>



MainWindow::MainWindow(int w, int h, char const * t) : Fl_Double_Window(w, h, t)
{
    begin();
    copy = new Fl_Button( 10, 150, 70, 30, "C&opy");
    copy->callback( cb_copy, this );

    quit = new Fl_Button(100, 150, 70, 30, "&Quit");
    quit->callback(cb_quit, this);

    inp = new Fl_Input(50, 50, 140, 30, "Input:");
    out = new Fl_Output(50, 100, 140, 30, "Output:");
    end();
    resizable(this);
}


MainWindow::~MainWindow()
{
}

void MainWindow::cb_copy(Fl_Widget * o, void * v)
{
    (void) o;
   //MainWindow* T=(MainWindow*)v;
   //T->cb_copy_i();

   // or just the one line below
    ( (MainWindow*)v )->cb_copy_i();
}


void MainWindow::cb_copy_i() {

   out->value(inp->value());
}

//----------------------------------------------------

void MainWindow::cb_quit(Fl_Widget *, void * v) {

   ( (MainWindow*)v )->cb_quit_i();
}


void MainWindow::cb_quit_i() {

    hide();
}

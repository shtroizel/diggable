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


#include "TextWidget.h"


TextWidget::TextWidget(int x, int y, int w, int h, const char* t) : Fl_Widget(x, y, w, h, t)
{
}


TextWidget::~TextWidget() noexcept
{

}
int highlight{0};

// int TextWidget::handle(int event)
// {
//     std::cout << "event: " << std::to_string(event) << std::endl;
//     return 1;
//     switch(event) {
//         case FL_PUSH:
//             highlight = 1;
//             std::cout << "FL_PUSH!" << std::endl;
//             redraw();
//             return 1;
//         case FL_DRAG: {
//                 std::cout << "FL_DRAG!" << std::endl;
//                 int t = Fl::event_inside(this);
//                 if (t != highlight) {
//                     highlight = t;
//                     redraw();
//                 }
//             }
//             return 1;
//         case FL_RELEASE:
//             std::cout << "FL_RELEASE!" << std::endl;
//             if (highlight) {
//                 highlight = 0;
//                 redraw();
//                 do_callback();
//                 // never do anything after a callback, as the callback
//                 // may delete the widget!
//             }
//             return 1;
//         case FL_SHORTCUT:
//             std::cout << "FL_SHORTCUT!" << std::endl;
//             if (Fl::event_key() == 'x') {
//                 do_callback();
//                 return 1;
//             }
//             return 0;
//         default:
//             return Fl_Widget::handle(event);
//     }
//
//     return 1;
// }

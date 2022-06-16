#include "progress_printer.hpp"

#include <iostream>
using std::cout;
using std::endl;

void showCursor(bool show) {
#define CSI "\e["
    if (show) {
        fputs(CSI "?25h", stdout);
    }
    else {
        fputs(CSI "?25l", stdout);
    }
#undef CSI
}


ProgressPrinter::ProgressPrinter(std::uintmax_t complete_size) : complete_size_(complete_size) {
    showCursor(false);
}

ProgressPrinter::~ProgressPrinter() {
    showCursor(true);
}


void ProgressPrinter::progress_update(int bytes_processed) {
    percent_ = bytes_processed * 100 / complete_size_;
    update();
}

void ProgressPrinter::finish() {
  
    if (percent_ != 100) {
        percent_ = 100;
        update();
    }
  
    cout << endl;
    showCursor(true);
}


void ProgressPrinter::update() {
    cout << "\r[\u001b[32m";
    for (int i=0; i<100; i++) {
        if (i < percent_) cout << "#";
        else cout << " ";
    }
    cout << "\u001b[0m] " << percent_ << "\% done";
}

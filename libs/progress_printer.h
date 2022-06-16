#pragma once

#include <cstdint>

class ProgressPrinter {

    std::uintmax_t complete_size_;
    int percent_;

    void update();

public:
    ProgressPrinter(std::uintmax_t complete_size);
    ~ProgressPrinter();

    void progress_update(int bytes_processed);
    void finish();
};

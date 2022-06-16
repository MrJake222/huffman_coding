#pragma once

#include <sstream>
#include <map>
#include <set>

#include <chrono>

#include "linklist.h"
#include "bitarray.h"
#include "progress_printer.h"

#include "huffnode.h"

namespace hf {

enum Action { ENCODE, DECODE };

typedef std::map<int, detail::HuffNode*> NodeMap;
typedef lnklist::LinkList<detail::HuffNode*> NodeList;
typedef bitarr::BitArray<detail::BitCell> CodeBitArray;

class Huffman {

    std::istream& src_;
    std::ostream& dest_;

    ProgressPrinter* progress_printer_ = nullptr;
    void update_progress(int bytes_processed);
    void finish_progress();
    int bytes_per_update_ = 10000;
    
    std::chrono::time_point<std::chrono::steady_clock> start_, end_;
    void timer_start();
    void timer_stop();
    void timer_print();

    NodeMap nodes;
    NodeList nodes_list;
    CodeBitArray bit_buffer;

    detail::NodePtr nyt;

    size_t input_bytes;
    size_t output_bytes;
    void write_to_stream_if_possible(bool last);

    void expand_nyt(uint8_t b_in);
    
    void encode_byte(uint8_t b_in);
    
    detail::NodePtr traverse_tree(detail::NodePtr node);
    void load_byte();

public:
    Huffman(std::istream& src, std::ostream& dest);
    ~Huffman();

    void set_progress_printer(ProgressPrinter* printer) { progress_printer_ = printer; }
    void set_bytes_per_update(int bytes) { bytes_per_update_ = bytes; }

    void encode();
    void decode();
};

} // end namespace

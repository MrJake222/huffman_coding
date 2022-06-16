#include "huffman.h"

#include "bitarray.h"

#include "huffnode.h"
using namespace detail;

#include <iostream>
using std::cout;
using std::endl;

#include <chrono>
using namespace std::chrono;

namespace hf {

Huffman::Huffman(std::istream& src, std::ostream& dest) : src_(src), dest_(dest), bit_buffer(bitarr::Mode::INCREMENT),
                                                          input_bytes(0), output_bytes(0) {
    nyt = new HuffNode(NYT_SYMBOL, 0, nodes_list.create_left());
    nodes[NYT_SYMBOL] = nyt;
}

Huffman::~Huffman() {

    // delete HuffNodes
    for (NodePtr node : nodes_list) {
        delete node;
    }
    
    progress_printer_ = nullptr;
}

void Huffman::update_progress(int bytes_processed) {
    if (progress_printer_) {
        progress_printer_->progress_update(bytes_processed);
    }
}

void Huffman::finish_progress() {
    if (progress_printer_) {
        progress_printer_->finish();
    }
}

void Huffman::timer_start() {
    start_ = std::chrono::steady_clock::now();
}

void Huffman::timer_stop() {
    end_ = std::chrono::steady_clock::now();
}

void Huffman::timer_print() {
    auto duration = duration_cast<microseconds>(end_ - start_);
    cout << "took " << std::fixed << duration.count() / 1000000. << "s" << endl;
}


void Huffman::write_to_stream_if_possible(bool last) {
    size_t bytes;
    
    if (last) {
        // if last cells use trim bytes
        while (bit_buffer.can_trim_byte()) {
            uint8_t buf = bit_buffer.trim_byte();
            dest_.put(buf);
            output_bytes++;
        }
    }
    else {
        // trim whole cells (faster)
        uint8_t buf[sizeof(BitCell)];
        while (bit_buffer.can_trim_cell()) {
            
            bit_buffer.trim_cell_into(buf, bytes);
            
            for (unsigned int i=0; i<bytes; i++) {
                dest_.put(buf[i]);
                output_bytes++;
            }
        }
    }
    
}

void Huffman::expand_nyt(uint8_t b_in) {
    // create nodes IN ORDER of increasing counts
    ListNodePtr value_listNode = nodes_list.create_left();
    ListNodePtr new_nyt_listNode = nodes_list.create_left();

    // make new nodes
    NodePtr value_node = new HuffNode(b_in, 1, value_listNode);
    nyt = nyt->expand(value_node, new_nyt_listNode);

    nodes[NYT_SYMBOL] = nyt;
    nodes[b_in] = value_node;
}


void Huffman::encode_byte(uint8_t b_in) {
        
    if (nodes.count(b_in)) {
        NodePtr node = nodes[b_in];
        // cout << b_in << " " << node->get_code() << endl;

        bit_buffer += node->get_code();
        node->increment();
    }

    else {
        // not yet transferred
        // cout << "# " << nyt->get_code() << endl << b_in << " " << std::bitset<8>(b_in) << endl;
        
        bit_buffer += nyt->get_code();
        bit_buffer <<= 8;
        bit_buffer |= b_in;
        
        expand_nyt(b_in);
    }
}

void Huffman::encode() {

    timer_start();

    uint8_t b_in;
    while (src_.get((char&)b_in)) {
        
        encode_byte(b_in);
        
        write_to_stream_if_possible(false);
        input_bytes += 1;

        if (input_bytes % bytes_per_update_ == 0) {
            update_progress(input_bytes);
        }
    }

    // terminating byte
    encode_byte(0);

    write_to_stream_if_possible(false);
    bit_buffer.pad_to_full_byte();
    write_to_stream_if_possible(true);
    
    timer_stop();
    finish_progress();
        
    cout << "bytes input " << input_bytes << " output " << output_bytes << endl;
    cout.precision(2);
    float percent = ((float)input_bytes - output_bytes) / input_bytes * 100;
    cout << "size reduction " << std::fixed << percent << "%" << (percent < 0 ? " (output bigger)" : "") << endl;
    timer_print();
}

/*
 * Consumes all bits in bit_buffer until it hits leaf
 * the bits may run out, then it returns internal node
 */
NodePtr Huffman::traverse_tree(NodePtr node) {
    
    while (!bit_buffer.is_empty() && !node->is_leaf()) {
        uint8_t bit = bit_buffer.trim_bit();
        node = node->go_via(bit);
    }
    
    return node;
}

void Huffman::load_byte() {
    uint8_t b_in;
    if (!src_.get((char&)b_in)) {
        throw std::runtime_error("load_byte beyond input");
    }

    bit_buffer <<= 8;
    bit_buffer |= b_in;
        
    input_bytes++;
    if (input_bytes % bytes_per_update_ == 0) {
        update_progress(input_bytes);
    }
}


void Huffman::decode() {
    
    timer_start();
    
    NodePtr root = nyt;
    while (true) {
        
        NodePtr node = root;
        while (!node->is_leaf()) {
            if (bit_buffer.is_empty()) {
                load_byte();
            }
            
            node = traverse_tree(node);
        }
                
        // node is leaf now
        if (node->is_nyt()) {
            // not yet transferred
            if (!bit_buffer.can_trim_byte()) {
                load_byte();
            }
            
            uint8_t b_in = bit_buffer.trim_byte();
            
            /*
             * Received ending byte. This disalows this to be used
             * with binary files. TODO encode number of bytes in file header
             */
            if (b_in == 0) {
                break;
            }
            
            dest_.put(b_in);
            expand_nyt(b_in);
        }
        else {
            uint8_t b_in = node->get_symbol();
            dest_.put(b_in);
            node->increment();
        }
        
        output_bytes++;
    }
    
    timer_stop();
    finish_progress();
        
    cout << "bytes input " << input_bytes << " output " << output_bytes << endl;
    timer_print();
}

} // end namespace

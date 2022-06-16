#pragma once

#include <cstddef>
#include <vector>
#include <sstream>
#include <exception>
#include <bitset>

#include <iostream>
using std::cout;
using std::endl;

namespace bitarr {

enum Mode { INCREMENT, DOUBLE };

template<typename Cell>
class BitArray {

    static const size_t bytes_per_cell_ = sizeof(Cell);
    static const size_t bits_per_cell_ = bytes_per_cell_ * 8;

    std::vector<Cell> cells_;
    size_t bits_used_ = 0;
    Mode mode_;
    
    void shift_whole_left(size_t pos);
    void shift_left(size_t pos);
    
    void shift_whole_right(size_t pos);
    void shift_right(size_t pos);
    
    BitArray& operator|=(BitArray rhs);

public:

    BitArray() : BitArray(Mode::INCREMENT) { }
    BitArray(Mode mode) : BitArray(0, mode) { }
    BitArray(size_t n_cells) : BitArray(n_cells, Mode::INCREMENT) { }
    
    BitArray(size_t n_cells, Mode mode);
    
    /*
     * Creates BitArray from string
     * Cares only about 0/1, all others characters are ignored
     * (you can call it with delimeters in binary strings)
     */
    BitArray(std::string str);
    
    size_t get_cells_used() const;
    size_t get_bits_used() const { return bits_used_; }
    bool is_empty() const { return bits_used_ == 0; }
    
    size_t get_full_cells() const { return bits_used_ / bits_per_cell_; }
    size_t get_last_cell_bits() const;
    size_t get_last_cell_free_bits() const { return bits_per_cell_ - get_last_cell_bits(); }
    size_t get_bits_left() const { return (cells_.size() * bits_per_cell_) - bits_used_; }
    
    void grow_to_atleast(size_t min_cells);
    
    /*
     * Overloaded operators to handle right hand side operations
     */
    BitArray& operator<<=(size_t count);
    //BitArray& operator>>=(size_t count);
    BitArray& operator|=(uint8_t lsb);
    //Cell operator&(Cell lsb) const;
    BitArray& operator+=(BitArray other);
    
    /*
     * Equality check
     * used in tests
     */
    bool operator==(const BitArray& other) const;

    /*
     * Pad to multiple of cell size (in bits)
     */
    void pad_to_full_cell();
    
    /*
     * Trim cell
     * Cut off rightmost cell (or merge 2 cells into one)
     */
    bool can_trim_cell();
    Cell trim_cell();
    void trim_cell_into(uint8_t* buf, size_t& n_bytes);
    
    // Pad to multiple of 8 bits
    void pad_to_full_byte();
    
    /*
     * Trim byte
     * Cut off rightmost byte (extract from 1-2 cells)
     */
    bool can_trim_byte();
    uint8_t trim_byte();
    uint8_t trim_bit();
    
    /*
     * Load byte from right side
     */
    //void load_bytes_from(uint8_t* buf, size_t n_bytes);

    static std::string cell_to_bits(Cell cell, size_t show_bits);

    template<class X>
    friend std::ostream& operator<<(std::ostream& os, const BitArray<X>& ba);
};


/*
 * Implementations
 */

template<typename Cell>
BitArray<Cell>::BitArray(size_t n_cells, bitarr::Mode mode) {
    cells_.resize(n_cells);
    mode_ = mode;
}

template<typename Cell>
BitArray<Cell>::BitArray(std::string str) : BitArray() {
    for (unsigned int i=0; i<str.length(); i++) {
        if (str[i] == '0') {
            *this <<= 1;
        }
        else if (str[i] == '1') {
            *this <<= 1;
            *this |= 1;
        }
    }
}

template<typename Cell>
size_t BitArray<Cell>::get_cells_used() const {
    size_t full_cells = get_full_cells();
    if (bits_used_ % bits_per_cell_) {
        full_cells++;
    }

    return full_cells;
}

template<typename Cell>
size_t BitArray<Cell>::get_last_cell_bits() const {
    size_t last = bits_used_ % bits_per_cell_;
    if (last == 0 && bits_used_ > 0) {
        last = bits_per_cell_;
    }
    
    return last;
}

template <typename Cell>
void BitArray<Cell>::grow_to_atleast(size_t min_cells) {
    size_t old_size = cells_.size();
    size_t new_size = old_size;

    if (old_size >= min_cells) {
        return;
    }

    // min_size > new_size
    switch (mode_) {
        case INCREMENT:
            //new_size += 1;
            new_size = min_cells;
            break;

        case DOUBLE:
            while (new_size < min_cells) {
                new_size *= 2;
            }
            break;
    }
    
    cells_.resize(new_size);
    // std::cout << "resized from " << old_size << " to " << new_size << " (mode=" << mode_ << ")\n";
}

/*
 * Below are the functions to handle left hand side operations
 */
template <typename Cell>
void BitArray<Cell>::shift_whole_left(size_t pos) {
    
    if (get_bits_left() < bits_per_cell_ * pos) {
        // end of space
        grow_to_atleast(cells_.size() + pos);
    }

    int N = get_cells_used() + pos;
    
    for (int i=N-1; i - ((int)pos)>=0; i--) {
        cells_[i] = cells_[i-pos];
    }

    for (unsigned int i=0; i<pos; i++) {
        cells_[i] = 0;
    }

    bits_used_ += bits_per_cell_ * pos;
}

template <typename Cell>
void BitArray<Cell>::shift_left(size_t pos) {
    
    if (get_bits_left() < pos) {
        // end of space
        grow_to_atleast(cells_.size() + 1);
    }
    
    int N = get_cells_used();
    if (get_last_cell_free_bits() < pos) N++;

    size_t downshift = bits_per_cell_ - pos;
        
    for (int i=N-1; i>0; i--) {
        cells_[i] <<= pos;

        Cell copy = cells_[i-1];
        copy >>= downshift;

        cells_[i] |= copy;
    }

    cells_[0] <<= pos;
    bits_used_ += pos;
}

template <typename Cell>
BitArray<Cell>& BitArray<Cell>::operator<<=(size_t pos) {
    
    int whole = pos / bits_per_cell_;
    int partial = pos % bits_per_cell_;
    // cout << "whole " << whole << " partial " << partial << endl;
    
    if (whole) {
        shift_whole_left(whole);
    }

    if (partial) {
        shift_left(partial);
    }

    return *this;
}

/*template <typename Cell>
void BitArray<Cell>::shift_whole_right(size_t pos) {
    
    int N = get_cells_used();
    for (int i=0; i + ((int)pos)<N; i++) {
        cells_[i] = cells_[i+pos];
    }

    for (int i=N-1; i>N-1-((int)pos); i--) {
        cells_[i] = 0;
    }

    bits_used_ -= bits_per_cell_ * pos;
}

template <typename Cell>
void BitArray<Cell>::shift_right(size_t pos) {
    
    size_t upshift = bits_per_cell_ - pos;

    int N = get_cells_used();
    for (int i=0; i<N-1; i++) {
        cells_[i] >>= pos;

        Cell copy = cells_[i+1];
        copy <<= upshift;

        cells_[i] |= copy;
    }

    cells_[N-1] >>= pos;
    bits_used_ -= pos;
}*/

/*template <typename Cell>
BitArray<Cell>& BitArray<Cell>::operator>>=(size_t pos) {
    
    int whole = pos / bits_per_cell_;
    int partial = pos % bits_per_cell_;

    if (whole) {
        shift_whole_right(whole);
    }

    if (partial) {
        shift_right(partial);
    }

    return *this;
}*/

template <typename Cell>
BitArray<Cell>& BitArray<Cell>::operator|=(uint8_t lsb) {
    cells_[0] |= lsb;

    return *this;
}

/*template <typename Cell>
Cell BitArray<Cell>::operator&(Cell rhs) const {
    return cells_[0] & rhs;
}*/

template <typename Cell>
BitArray<Cell>& BitArray<Cell>::operator|=(BitArray<Cell> rhs) {
    for (unsigned int i=0; i<rhs.get_cells_used(); i++) {
        this->cells_[i] |= rhs.cells_[i];
    }

    return *this;
}

template <typename Cell>
BitArray<Cell>& BitArray<Cell>::operator+=(BitArray<Cell> other) {
    *this <<= other.get_bits_used();
    *this |= other;

    return *this;
}

/*
 * Equality check
 */
template<typename Cell>
bool BitArray<Cell>::operator==(const BitArray<Cell>& other) const {
    if (bits_used_ != other.bits_used_)
        return false;
    
    for (unsigned int i=0; i<get_cells_used(); i++) {
        if (cells_[i] != other.cells_[i]) {
            return false;
        }
    }
    
    return true;
}


/*
 * Below functions used to "trim" bytes from the left side
 * i.e. remove them and return cutting the number of bits used
 */
template<typename Cell>
void BitArray<Cell>::pad_to_full_cell() {
    *this <<= get_last_cell_free_bits();
}

template<typename Cell>
bool BitArray<Cell>::can_trim_cell() {
    return bits_used_ >= bits_per_cell_;
}

/*
 * Can't be optimized to trim only full cells,
 * because of the fact that we won't be able to detect
 * when cells align exactly.
 */
template<typename Cell>
Cell BitArray<Cell>::trim_cell() {
    size_t full_cells = get_full_cells();
    size_t last_cell_bits = get_last_cell_bits();
    
    Cell last = 0;
    
    if (last_cell_bits == bits_per_cell_) {
        // last cell full
        // return this cell
        last = cells_[full_cells - 1];
    }
    else {
        Cell second_last = cells_[full_cells - 1];
        second_last >>= last_cell_bits;

        last = cells_[full_cells];
        last <<= (bits_per_cell_ - last_cell_bits);

        last |= second_last;
    }
        
    bits_used_ -= bits_per_cell_;
    
    return last;
}

/*
 * This could be done better by iterator for example
 */
template<typename Cell>
void BitArray<Cell>::trim_cell_into(uint8_t* buf, size_t& n_bytes) {
    Cell cell = trim_cell();

    n_bytes = 0;
    size_t N = bytes_per_cell_;
    for (unsigned int i=0; i<N; i++) {
        buf[i] = (cell >> 8*(N-i-1));
        n_bytes++;
    }
}

template<typename Cell>
void BitArray<Cell>::pad_to_full_byte() {
    size_t last_cell_bits = get_last_cell_bits();
    if (last_cell_bits % 8 == 0) {
        // divisible, there are already full bytes
        return;
    }
    
    *this <<= (8 - (last_cell_bits % 8));
}

template<typename Cell>
bool BitArray<Cell>::can_trim_byte() {
    return bits_used_ >= 8;
}

template<typename Cell>
uint8_t BitArray<Cell>::trim_byte() {
    size_t last_cell_bits = get_last_cell_bits();
    size_t N = get_cells_used();
        
    uint8_t result;
    
    if (last_cell_bits >= 8) {
        // can operate on the last cell only
        size_t downshift = last_cell_bits - 8;
        
        Cell copy = cells_[N-1];
        copy >>= downshift;
        
        result = copy;
    }
    else {
        // last byte crosses cell border
        size_t upshift = 8 - last_cell_bits;
        size_t downshift = bits_per_cell_ - upshift;
        
        Cell upper = cells_[N-1];
        upper <<= upshift;
        
        Cell lower = cells_[N-2];
        lower >>= downshift;
        
        result = (upper | lower);
    }
    
    bits_used_ -= 8;
    return result;
}

template<typename Cell>
uint8_t BitArray<Cell>::trim_bit() {
    size_t N = get_cells_used();
    
    Cell mask = 1 << (get_last_cell_bits() - 1);
    
    bits_used_--;
    return (cells_[N-1] & mask) ? 1 : 0;
}

/*
 * Used to load bits from the left side of the BitArray
 */
/*template<typename Cell>
void BitArray<Cell>::load_bytes_from(uint8_t* buf, size_t n_bytes) {
    
    size_t N = cells_.size();
    size_t n_cells = get_cells_used();
    
    grow_to_atleast(N + n_cells);
    
    size_t used_cells = get_cells_used();
    if (used_cells == 0) {
        // bitarray is empty
        for (unsigned int i=0; i<n_bytes; i++) {
            cells_[i] = buf[i];
        }
    }
    else {
        size_t last_cell_free_bits = get_last_cell_free_bits();
        size_t upshift_input = bits_per_cell_ - last_cell_free_bits;
        size_t downshift_input = last_cell_free_bits;
        size_t last_cell_index = used_cells - 1;
        
        for (unsigned int i=0; i<n_bytes; i++) {
            Cell cell_lower = ((Cell)buf[i]) << upshift_input;
            Cell cell_upper = ((Cell)buf[i]) >> downshift_input;
                    
            cells_[last_cell_index] |= cell_lower;
            cells_[last_cell_index+1] |= cell_upper;
                    
            last_cell_index++;
        }
    }
    
    bits_used_ += n_bytes * 8;
}*/


template <typename Cell>
std::string BitArray<Cell>::cell_to_bits(Cell cell, size_t show_bits) {
    std::string out;
    for (unsigned int i=0; i<show_bits; i++) {
        out = (char)('0' + (cell%2)) + out;
        cell /= 2;
    }

    return out;
}

template <typename Cell>
std::ostream& operator<<(std::ostream& os, const BitArray<Cell>& ba) {

    int N = ba.get_cells_used();
    for (int i=N-1; i>=0; i--) {

        size_t show_bits = ba.bits_per_cell_;
        if (i == N-1) {
            // last cell
            size_t before = i * ba.bits_per_cell_;
            show_bits = ba.bits_used_ - before;
        }

        os << BitArray<Cell>::cell_to_bits(ba.cells_[i], show_bits);
        os << " ";
    }

    return os;
}

} // end namespace

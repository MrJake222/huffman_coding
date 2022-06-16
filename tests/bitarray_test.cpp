#include <gtest/gtest.h>

#include "../libs/bitarray.hpp"

using namespace bitarr;

TEST (BitArrayTest, Creation) {
    BitArray<uint8_t> ba;
    
    ASSERT_EQ(ba.get_cells_used(), 0);
    ASSERT_EQ(ba.get_bits_used(), 0);
    ASSERT_EQ(ba.is_empty(), true);
    
    ASSERT_EQ(ba.get_full_cells(), 0);
    ASSERT_EQ(ba.get_last_cell_bits(), 0);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 8);
    ASSERT_EQ(ba.get_bits_left(), 0);
    
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), false);
}

TEST (BitArrayTest, CreationBig) {
    BitArray<uint8_t> ba(16);
    
    ASSERT_EQ(ba.get_cells_used(), 0);
    ASSERT_EQ(ba.get_bits_used(), 0);
    ASSERT_EQ(ba.is_empty(), true);
    
    ASSERT_EQ(ba.get_full_cells(), 0);
    ASSERT_EQ(ba.get_last_cell_bits(), 0);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 8);
    ASSERT_EQ(ba.get_bits_left(), 16 * 8);
    
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), false);
}

TEST (BitArrayTest, CountsShift) {
    BitArray<uint8_t> ba;
    
    ba <<= 5;
    
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba.get_bits_used(), 5);
    ASSERT_EQ(ba.is_empty(), false);
    
    ASSERT_EQ(ba.get_full_cells(), 0);
    ASSERT_EQ(ba.get_last_cell_bits(), 5);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 3);
    ASSERT_EQ(ba.get_bits_left(), 3);
    
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), false);
}

TEST (BitArrayTest, CountsShift32) {
    BitArray<uint32_t> ba;
    
    ba <<= 5;
    
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba.get_bits_used(), 5);
    ASSERT_EQ(ba.is_empty(), false);
    
    ASSERT_EQ(ba.get_full_cells(), 0);
    ASSERT_EQ(ba.get_last_cell_bits(), 5);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 32 - 5);
    ASSERT_EQ(ba.get_bits_left(), 32 - 5);
    
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), false);
}

TEST (BitArrayTest, CountsShiftFull) {
    BitArray<uint8_t> ba;
    
    ba <<= 8;
    
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba.get_bits_used(), 8);
    ASSERT_EQ(ba.is_empty(), false);
    
    ASSERT_EQ(ba.get_full_cells(), 1);
    ASSERT_EQ(ba.get_last_cell_bits(), 8);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 0);
    ASSERT_EQ(ba.get_bits_left(), 0);
    
    ASSERT_EQ(ba.can_trim_cell(), true);
    ASSERT_EQ(ba.can_trim_byte(), true);
}

TEST (BitArrayTest, CountsShiftFull32) {
    BitArray<uint32_t> ba;
    
    ba <<= 8;
    
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba.get_bits_used(), 8);
    ASSERT_EQ(ba.is_empty(), false);
    
    ASSERT_EQ(ba.get_full_cells(), 0);
    ASSERT_EQ(ba.get_last_cell_bits(), 8);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 32 - 8);
    ASSERT_EQ(ba.get_bits_left(), 32 - 8);
    
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), true);
}

TEST (BitArrayTest, CountsShiftMultiple) {
    BitArray<uint8_t> ba;
    
    ba <<= 15;
    ASSERT_EQ(ba.get_full_cells(), 1);
    ASSERT_EQ(ba.get_last_cell_bits(), 7);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 1);
    
    ba <<= 1;
    ASSERT_EQ(ba.get_full_cells(), 2);
    ASSERT_EQ(ba.get_last_cell_bits(), 8);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 0);
    
    ba <<= 1;
    ASSERT_EQ(ba.get_full_cells(), 2);
    ASSERT_EQ(ba.get_last_cell_bits(), 1);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 7);
}

TEST (BitArrayTest, CountsShiftMultiple32) {
    BitArray<uint32_t> ba;
    
    ba <<= 33;
    ASSERT_EQ(ba.get_full_cells(), 1);
    ASSERT_EQ(ba.get_last_cell_bits(), 1);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 31);
    
    ba <<= 63 - 33;
    ASSERT_EQ(ba.get_full_cells(), 1);
    ASSERT_EQ(ba.get_last_cell_bits(), 31);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 1);
    
    ba <<= 1;
    ASSERT_EQ(ba.get_full_cells(), 2);
    ASSERT_EQ(ba.get_last_cell_bits(), 32);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 0);
    
    ba <<= 1;
    ASSERT_EQ(ba.get_full_cells(), 2);
    ASSERT_EQ(ba.get_last_cell_bits(), 1);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 31);
}

TEST (BitArrayTest, StrCreation) {
    BitArray<uint8_t> ba("0110 1101");
    ASSERT_EQ(ba.get_bits_used(), 8);
    ASSERT_EQ(ba.is_empty(), false);
    ASSERT_EQ(ba.can_trim_cell(), true);
    ASSERT_EQ(ba.can_trim_byte(), true);
}

TEST (BitArrayTest, ShiftLeftInsideOneCell) {
    BitArray<uint8_t> ba("0110 1101");
    ba <<= 1;
    ASSERT_EQ(ba.get_bits_used(), 9);
    ASSERT_EQ(ba, BitArray<uint8_t>("0110 1101 0"));
    ba |= 1;
    ASSERT_EQ(ba.get_bits_used(), 9);
    
    ba <<= 4;
    ASSERT_EQ(ba, BitArray<uint8_t>("0110 1101 1 0000"));
    
    ba <<= 8;
    ASSERT_EQ(ba, BitArray<uint8_t>("0110 1101 1 0000 0000 0000"));
}

TEST (BitArrayTest, ShiftLeftInsideOneCellLarge) {
    BitArray<uint8_t> ba(4);

    ba <<= 8;
    ba |= 0x80;
    
    ba <<= 1;
    ASSERT_EQ(ba, BitArray<uint8_t>("1000 0000 0"));
}

TEST (BitArrayTest, ShiftLeftInsideOneCellSmall) {    
    BitArray<uint8_t> ba("1000 0000");
    
    ba <<= 1;
    ASSERT_EQ(ba, BitArray<uint8_t>("1000 0000 0"));
}

TEST (BitArrayTest, ShiftLeftInsideMultipleCells) {
    BitArray<uint8_t> ba("0110 1101");
     
    ba <<= 8;
    ASSERT_EQ(ba, BitArray<uint8_t>("0110 1101 0000 0000"));
    
    ba <<= 8;
    ASSERT_EQ(ba, BitArray<uint8_t>("0110 1101 0000 0000 0000 0000"));
}

TEST (BitArrayTest, ShiftLeftMixed) {
    BitArray<uint8_t> ba("0110 1101");
     
    ba <<= 13;
    ASSERT_EQ(ba, BitArray<uint8_t>("0110 1101 0000 0000 0000 0"));
    
    ba <<= 17;
    ASSERT_EQ(ba, BitArray<uint8_t>("0110 1101 0000 0000 0000 0 0000 0000 0000 0000 0"));
}

TEST (BitArrayTest, AppendShort) {
    BitArray<uint8_t> ba("11");
    
    ba += BitArray<uint8_t>("10");
    ASSERT_EQ(ba, BitArray<uint8_t>("11 10"));
    
    ba += BitArray<uint8_t>("11");
    ASSERT_EQ(ba, BitArray<uint8_t>("11 10 11 "));
    
    ba += BitArray<uint8_t>("01");
    ASSERT_EQ(ba, BitArray<uint8_t>("11 10 11 01"));
}

TEST (BitArrayTest, AppendLong) {
    BitArray<uint8_t> ba("1100");
    
    ba += BitArray<uint8_t>("0011");
    ASSERT_EQ(ba, BitArray<uint8_t>("1100 0011"));
    
    ba += BitArray<uint8_t>("1");
    ASSERT_EQ(ba, BitArray<uint8_t>("1100 0011 1"));
    
    ba += BitArray<uint8_t>("1101 1011 1");
    ASSERT_EQ(ba, BitArray<uint8_t>("1100 0011 1  1101 1011 1"));
}

TEST (BitArrayTest, PadToFullCell) {
    BitArray<uint8_t> ba("1100");
    ASSERT_EQ(ba.get_last_cell_free_bits(), 4);
    
    ba.pad_to_full_cell();
    ASSERT_EQ(ba.get_last_cell_free_bits(), 0);
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba, BitArray<uint8_t>("1100 0000"));
    
    ba.pad_to_full_cell();
    ASSERT_EQ(ba.get_last_cell_free_bits(), 0);
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba, BitArray<uint8_t>("1100 0000"));
}

TEST (BitArrayTest, PadToFullCell32) {
    BitArray<uint32_t> ba("1100");
    ASSERT_EQ(ba.get_last_cell_free_bits(), 32 - 4);
    
    ba.pad_to_full_cell();
    ASSERT_EQ(ba.get_bits_used(), 32);
    ASSERT_EQ(ba.get_last_cell_bits(), 32);
    ASSERT_EQ(ba.get_last_cell_free_bits(), 0);
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba, BitArray<uint32_t>("1100 0000 00000000 00000000 00000000"));
    
    ba.pad_to_full_cell();
    ASSERT_EQ(ba.get_last_cell_free_bits(), 0);
    ASSERT_EQ(ba.get_cells_used(), 1);
    ASSERT_EQ(ba, BitArray<uint32_t>("1100 0000 00000000 00000000 00000000"));
    
    ba <<= 1;
    ba |= 1;
    ASSERT_EQ(ba.get_last_cell_free_bits(), 32 - 1);
    ASSERT_EQ(ba.get_cells_used(), 2);
    ASSERT_EQ(ba, BitArray<uint32_t>("1100 0000 00000000 00000000 00000000 1"));
    
    ba.pad_to_full_byte();
    ASSERT_EQ(ba.get_last_cell_free_bits(), 32 - 8);
    ASSERT_EQ(ba.get_cells_used(), 2);
    ASSERT_EQ(ba, BitArray<uint32_t>("1100 0000 00000000 00000000 00000000 1000 0000"));
    
    ba.pad_to_full_byte();
    ASSERT_EQ(ba.get_last_cell_free_bits(), 32 - 8);
    ASSERT_EQ(ba.get_cells_used(), 2);
    ASSERT_EQ(ba, BitArray<uint32_t>("1100 0000 00000000 00000000 00000000 1000 0000"));
}

TEST (BitArrayTest, CanTrimCellByte) {
    BitArray<uint8_t> ba("1100");
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), false);
    
    ba <<= 4;
    ASSERT_EQ(ba.can_trim_cell(), true);
    ASSERT_EQ(ba.can_trim_byte(), true);
}

TEST (BitArrayTest, CanTrimCellByte32) {
    BitArray<uint32_t> ba("1100");
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), false);
    
    ba <<= 4;
    ASSERT_EQ(ba.can_trim_cell(), false);
    ASSERT_EQ(ba.can_trim_byte(), true);
    
    ba <<= 24;
    ASSERT_EQ(ba.can_trim_cell(), true);
    ASSERT_EQ(ba.can_trim_byte(), true);
}

TEST (BitArrayTest, TrimCellSmall) {
    BitArray<uint8_t> ba("1100 1100");
    
    uint8_t cell = ba.trim_cell();
    ASSERT_EQ(cell, 0xCC);
    ASSERT_EQ(ba.get_bits_used(), 0);
}

TEST (BitArrayTest, TrimCellLarge) {
    BitArray<uint8_t> ba("1100 1100 111");
    
    uint8_t cell = ba.trim_cell();
    ASSERT_EQ(cell, 0xCC);
    ASSERT_EQ(ba.get_bits_used(), 3);
}

TEST (BitArrayTest, TrimCellLarge32) {
    BitArray<uint32_t> ba("10100000 10100000 00001010 00000000 1111 111");
    
    uint32_t cell = ba.trim_cell();
    ASSERT_EQ(cell, 0xA0A00A00);
    ASSERT_EQ(ba.get_bits_used(), 7);
}

TEST (BitArrayTest, TrimCellInto32) {
    BitArray<uint32_t> ba("10100000 10100000 00001010 00000000 1111 111");
    
    uint8_t buf[4];
    size_t n_bytes;
    ba.trim_cell_into(buf, n_bytes);
    
    ASSERT_EQ(n_bytes, 4);
    ASSERT_EQ(buf[0], 0xA0);
    ASSERT_EQ(buf[1], 0xA0);
    ASSERT_EQ(buf[2], 0x0A);
    ASSERT_EQ(buf[3], 0x00);
    
    ASSERT_EQ(ba.get_bits_used(), 7);
}

TEST (BitArrayTest, TrimByte) {
    BitArray<uint8_t> ba("1010 111");
    
    ASSERT_EQ(ba.can_trim_byte(), false);
    
    ba <<= 1;
    ASSERT_EQ(ba.can_trim_byte(), true);
    ASSERT_EQ(ba.trim_byte(), 0xAE);
    ASSERT_EQ(ba.can_trim_byte(), false);
    ASSERT_EQ(ba.is_empty(), true);
}

TEST (BitArrayTest, TrimByte32) {
    BitArray<uint8_t> ba("1010 111");
    
    ASSERT_EQ(ba.can_trim_byte(), false);
    
    ba <<= 157;
    ASSERT_EQ(ba.can_trim_byte(), true);
    ASSERT_EQ(ba.trim_byte(), 0xAE);
    ASSERT_EQ(ba.can_trim_byte(), true);
    ASSERT_EQ(ba.trim_byte(), 0x00);
    ASSERT_EQ(ba.can_trim_byte(), true);
    ASSERT_EQ(ba.is_empty(), false);
}

TEST (BitArrayTest, TrimBit) {
    BitArray<uint32_t> ba("1011");
    
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 0);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.is_empty(), true);
}

TEST (BitArrayTest, TrimBit32) {
    BitArray<uint32_t> ba("11101 011101 00000 00001010 00000000");
    
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 0);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.get_bits_used(), 32 - 5);
    
    ba <<= 180;
    ASSERT_EQ(ba.trim_bit(), 0);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.trim_bit(), 0);
    ASSERT_EQ(ba.trim_bit(), 1);
    ASSERT_EQ(ba.get_bits_used(), 32 - 5 + 180 - 6);
}

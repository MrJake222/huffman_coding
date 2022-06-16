#pragma once

#include "bitarray.h"
#include "linklist.h"

namespace detail {

const int NYT_SYMBOL = -1;
const int INTERNAL_SYMBOL = -2;
const uint8_t BIT_LEFT = 0;
const uint8_t BIT_RIGHT = 1;

class HuffNode;
typedef HuffNode* NodePtr;
typedef lnklist::Node<detail::NodePtr>* ListNodePtr;
typedef uint64_t BitCell;

class HuffNode {

    int symbol_;
    int count_;
    ListNodePtr listNode_;

    NodePtr left_ = nullptr;
    NodePtr right_ = nullptr;
    NodePtr parent_ = nullptr;

    bitarr::BitArray<BitCell> code_;

public:
    HuffNode(int symbol, int count, ListNodePtr listNode);

    bool is_internal() const { return symbol_== INTERNAL_SYMBOL; }
    bool is_leaf() const { return !is_internal(); }
    bool is_nyt() const { return symbol_== NYT_SYMBOL; }
    
    int get_symbol() { return symbol_; }
    NodePtr go_via(uint8_t bit);
    
    bitarr::BitArray<BitCell> get_code() const { return code_; }

    void adjust_code_to_parent(uint8_t bit);
    NodePtr find_successor() const;
    void swap_with(NodePtr node);
    void increment();
    NodePtr expand(NodePtr value_node, ListNodePtr new_nyt_listNode);

    bool operator>(const HuffNode& rhs) const;
    bool operator<(const HuffNode& rhs) const { return rhs > *this; }
    bool operator>=(const HuffNode& rhs) const { return !(rhs > *this); }
    bool operator<=(const HuffNode& rhs) const { return !(rhs < *this); }

    friend std::ostream& operator<<(std::ostream& os, const HuffNode& node);
};

} // namespace end

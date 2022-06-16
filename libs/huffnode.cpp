#include "huffnode.h"

namespace detail {

HuffNode::HuffNode(int symbol, int count, ListNodePtr listNode) : symbol_(symbol), count_(count), listNode_(listNode) {
    listNode_->set_value(this);
}

NodePtr HuffNode::go_via(uint8_t bit) {
    switch (bit) {
        case BIT_LEFT:
            return left_;
    
        case BIT_RIGHT:
            return right_;
    }
    
    throw std::invalid_argument("invalid bit");
}


void HuffNode::adjust_code_to_parent(uint8_t bit) {

    code_ = parent_->code_;

    code_ <<= 1;
    code_ |= bit;

    if (left_) {
        left_->adjust_code_to_parent(BIT_LEFT);
    }
    if (right_) {
        right_->adjust_code_to_parent(BIT_RIGHT);
    }
}

NodePtr HuffNode::find_successor() const {
    ListNodePtr potential_next = listNode_->get_next();
    if (potential_next) {
        return potential_next->get_value();
    }

    return nullptr;
}

void HuffNode::swap_with(NodePtr node) {
    bool this_is_left = this->parent_->left_ == this;
    bool node_is_left = node->parent_->left_ == node;

    if (this_is_left) {
        this->parent_->left_ = node;
    } else {
        this->parent_->right_ = node;
    }

    if (node_is_left) {
        node->parent_->left_ = this;
    } else {
        node->parent_->right_ = this;
    }

    using std::swap;

    swap(parent_, node->parent_);
    adjust_code_to_parent(node_is_left ? BIT_LEFT : BIT_RIGHT);
    node->adjust_code_to_parent(this_is_left ? BIT_LEFT : BIT_RIGHT);

    swap(*listNode_, *node->listNode_);
    swap(listNode_, node->listNode_);
}


void HuffNode::increment() {
    count_++;

    NodePtr p_parent = parent_;

    while (true) {
        NodePtr node = find_successor();
        if (!node || *this <= *node) {
            break;
        }

        if (node == this->parent_) {
            // check if not parent
            // this occurs when node is sibling to NYT
            // break the loop, parent's value is going to be incremented
            break;
        }

        // slide node while it's not greater than it's successor
        // excluding parent - child swap
        swap_with(node);
    }

    if (is_internal()) {
        // internal node skips all nodes that counts are equal
        // so it swapped with same-valued node
        // need to increment old path
        // p_parent because parents are swapped
        if (p_parent) {
            p_parent->increment();
        }
    }
    else {
        // leaf node doesn't skip same valued nodes
        // so it swapped with lower-valued node
        // need to increment new path
        if (parent_) {
            parent_->increment();
        }
    }
}

NodePtr HuffNode::expand(NodePtr value_node, ListNodePtr new_nyt_listNode) {
    NodePtr new_nyt = new HuffNode(NYT_SYMBOL, 0, new_nyt_listNode);
    new_nyt->parent_ = this;
    left_ = new_nyt;

    value_node->parent_ = this;
    right_ = value_node;

    new_nyt->adjust_code_to_parent(BIT_LEFT);
    value_node->adjust_code_to_parent(BIT_RIGHT);

    symbol_ = INTERNAL_SYMBOL;
    increment();

    return new_nyt;
}

bool HuffNode::operator>(const HuffNode& rhs) const {
    int lcount = count_ * 2;
    int rcount = rhs.count_ * 2;

    if (is_internal()) {
        lcount++;
    }
    if (rhs.is_internal()) {
        rcount++;
    }

    return lcount > rcount;
}

std::ostream& operator<<(std::ostream& os, const HuffNode& node) {

    os << "{";

    if (node.is_nyt()) {
        os << "NYT";
    } else if (node.is_internal()) {
        os << "#";
    } else {
        os << (uint8_t)node.symbol_;
    }

    os << " code=";
    os << node.get_code();

    os << " cnt=";
    os << node.count_;

    os << "}";

    return os;
}

} // end namespace

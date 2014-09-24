/*
 * =====================================================================================
 *
 *       Filename:  fenwick-tree.hpp
 *
 *    Description:  http://community.topcoder.com/tc?module=Static&d1=tutorials&d2=binaryIndexedTrees
 *
 *        Version:  1.0
 *        Created:  09/22/2014 06:41:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __FENWICK_TREE_HPP__
#define __FENWICK_TREE_HPP__

#include <vector>
#include <type_traits>
template <typename T>
class FenwickTree
{
private:
    std::vector<T> _tree;
    T _total;
    T MostSignificationBit;

public:
#define LS1Bit(idx) (idx & -idx)

    FenwickTree() {}

    FenwickTree(std::size_t sz, T initialValue) { assign(sz, initialValue); }

    void assign(std::size_t sz, T initialValue) {
        MostSignificationBit = 1ULL << (sizeof(T)*8 - 1);
        while (MostSignificationBit > sz) MostSignificationBit >>= 1;

        _tree.resize(sz+1);
        for (std::size_t i = 1; i <= sz; ++i)
            _tree[i] = LS1Bit(i) * initialValue;
        _total = sz * initialValue;
    }

    T cumulate_freq(std::size_t idx) const
    {
        T sum = 0;
        while (idx) {
            sum += _tree[idx];
            idx -= LS1Bit(idx);
        }
        return sum;
    }

    T freq(std::size_t idx) const
    {
        T sum = _tree[idx];
        if (idx > 0) {
            std::size_t z = idx - LS1Bit(idx);
            --idx;
            while (idx != z) {
                sum -= _tree[idx];
                idx -= LS1Bit(idx);
            }
        }
        return sum;
    }

    void update(std::size_t idx, typename std::make_signed<T>::type diff)
    {
        while (idx < _tree.size()) {
            _tree[idx] += diff;
            idx += LS1Bit(idx);
        }
        _total += diff;
    }

    T total() const { return _total; }

    void scaleUp(T c) { 
        for (auto &f : _tree) f *= c; 
        _total = cumulate_freq(_tree.size() - 1);
    }

    void scaleDown(T c) { 
        for (auto &f : _tree) f /= c; 
        _total = cumulate_freq(_tree.size() - 1);
    }

    void scaleDown(T c, T min) { 
/*         for (std::size_t idx = 1; idx < _tree.size(); ++idx)
 * //            _tree[idx] = std::max(_tree[idx] / c, min);
 *             _tree[idx] = std::max(_tree[idx] / c, min * LS1Bit(idx));
 *         _total = cumulate_freq(_tree.size() - 1);
 */
        // TODO: O(sz) algorithm
//        assign(_tree.size() - 1, min);
        std::vector<T> freq_table(_tree.size() - 1);
        for (std::size_t idx = 0; idx < _tree.size() - 1; ++idx)
            freq_table[idx] = freq(idx + 1);
        assign(_tree.size() - 1, 0);
        for (std::size_t idx = 1; idx < _tree.size(); ++idx) {
            update(idx, std::max(freq_table[idx-1] / c, min));
        }
        _total = cumulate_freq(_tree.size() - 1);
    }

    // if in tree exists more than one index with a same
    // cumulative frequency, this procedure will return 
    // some of them (we do not know which one)

    // bitMask - initialy, it is the greatest bit of MaxVal
    // bitMask store interval which should be searched
    typename std::enable_if<std::is_unsigned<T>::value, std::size_t>::type
    upper_bound(T cumFre) const {
        std::size_t idx = 0; // this var is result of function
        std::size_t bitMask = MostSignificationBit;

        while ((bitMask != 0) && (idx < _tree.size() - 1)) { // nobody likes overflow :)
            std::size_t tIdx = idx + bitMask; // we make midpoint of interval
            if (cumFre == _tree[tIdx]) // if it is equal, we just return idx
                return tIdx;
            else if (cumFre > _tree[tIdx]) { 
                // if tree frequency "can fit" into cumFre,
                // then include it
                idx = tIdx; // update index 
                cumFre -= _tree[tIdx]; // set frequency for next loop 
            }
            bitMask >>= 1; // half current interval
        }
        
        return idx;
    }
#undef LS1Bit
};

#endif // __FENWICK_TREE_HPP__

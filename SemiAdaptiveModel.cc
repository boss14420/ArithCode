/*
 * =====================================================================================
 *
 *       Filename:  SemiAdaptiveModel.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 08:02:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "SemiAdaptiveModel.hh"
#include <algorithm>
#include <iostream>

SemiAdaptiveModel::SemiAdaptiveModel()
{}

void SemiAdaptiveModel::init(std::vector<value_type> const& freq)
{
    // freq table
//    _freq_table[EOS] = 2;

//    for (Word w = 0; w != nwords; ++w)
//        _freq_table[w] = (w * 1000000) % 256 + 200;
////        _freq_table[w] = 200;
    
    // accumulate table
    value_type total = std::accumulate(freq.begin(), freq.end(), 0);
    _accumulate_table.assign(nwords + 1, 0);
    for (Word w = 0; w != nwords-1; ++w) {
//        _accumulate_table[w+1] = _accumulate_table[w] + freq[w];

//        auto range = freq[w] * Top_value / 3 * 2 / total;
        auto range = std::max(2UL, freq[w] * Max_freq / total);
//        if (range < 1 && freq[w] > 0) range = 1;
        _accumulate_table[w+1] = _accumulate_table[w] + range;
    }
    _accumulate_table[EOS+1] = _accumulate_table[EOS] +
            std::max(2UL, Max_freq / total);
//    std::cout << _accumulate_table[EOS+1] << '\n';
}

void SemiAdaptiveModel::update(Word w)
{}

SemiAdaptiveModel::value_type
SemiAdaptiveModel::total() const
{
    return _accumulate_table.back();
}

SemiAdaptiveModel::value_type
SemiAdaptiveModel::range_start(Word w) const
{
    return _accumulate_table[w];
}

SemiAdaptiveModel::value_type
SemiAdaptiveModel::range_end(Word w) const
{
    return _accumulate_table[w+1];
}

SemiAdaptiveModel::Word
SemiAdaptiveModel::find_word(value_type value, value_type low, 
                                value_type high) const
{
    auto range = high - low + 1;
    auto accumulate = (((value - low) + 1) * total() - 1) / range;

    // binary search
    Word w = std::upper_bound(_accumulate_table.begin(),
                              _accumulate_table.end(), accumulate)
            - _accumulate_table.begin() - 1;
    return w;
}

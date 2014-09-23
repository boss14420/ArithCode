/*
 * =====================================================================================
 *
 *       Filename:  AdaptiveModel.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/21/2014 10:57:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */


#include "AdaptiveModel.hh"

AdaptiveModel::AdaptiveModel()
{}

void AdaptiveModel::init(std::vector<value_type> const& freq)
{
    _ft.assign(nwords, 1);
}

void AdaptiveModel::update(Word w)
{
    if (_ft.total() == Top_value)
        _ft.scaleDown(2, 1);
    _ft.update(w + 1, 1);
//    if (_accumulate_table[w+1] == Top_value) {
//        // shrink by factor 2
//        for (auto &i : _accumulate_table) i /= 2;
//    }
//
//    // TODO: use Binary Indexed tree
//    // increase 
//    for (; w != nwords; ++w)
//        ++_accumulate_table[w+1];
}

AdaptiveModel::value_type
AdaptiveModel::total() const {
    return _ft.total();
}

AdaptiveModel::value_type
AdaptiveModel::range_start(Word w) const {
    return _ft.cumulate_freq(w);
}

AdaptiveModel::value_type
AdaptiveModel::range_end(Word w) const {
    return _ft.cumulate_freq(w+1);
}

AdaptiveModel::Word
AdaptiveModel::find_word(value_type value, value_type low, 
                         value_type high) const 
{
    auto range = high - low + 1;
    auto accumulate = (((value - low) + 1) * total() - 1) / range;
    Word w = _ft.upper_bound(accumulate);
    return w;
}

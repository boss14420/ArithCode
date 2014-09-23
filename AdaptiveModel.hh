/*
 * =====================================================================================
 *
 *       Filename:  AdaptiveModel.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/21/2014 10:51:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __ADAPTIVEMODEL_HH__
#define __ADAPTIVEMODEL_HH__

#include <vector>
#include "model.hh"
#include "fenwick_tree.hpp"

class AdaptiveModel : public CodingModel
{
private:
    FenwickTree<value_type> _ft;

public:
    static const ModelType Type = AdaptiveModelType;

public:
    AdaptiveModel();

    void init(std::vector<value_type> const &freq);
    void update(Word w);
    value_type total() const;
    value_type range_start(Word w) const;
    value_type range_end(Word w) const;

    Word find_word(value_type value, value_type low, 
                   value_type high) const;
};

template <>
struct MagicNumber<AdaptiveModel> {
    static const std::uint32_t magic_number = 0x41434100; // "ACA\0"
};

#endif // __ADAPTIVEMODEL_HH__

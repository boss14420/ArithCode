/*
 * =====================================================================================
 *
 *       Filename:  SemiAdaptiveModel.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 08:00:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __SEMIADAPTIVEMODEL_HH__
#define __SEMIADAPTIVEMODEL_HH__

#include <vector>
#include <cstdint>
#include "model.hh"

class SemiAdaptiveModel : public CodingModel
{
private:
    std::vector<value_type> _accumulate_table;

public:
    static const ModelType Type = SemiAdaptiveModelType;

public:
    SemiAdaptiveModel();

    void init(std::vector<value_type> const &freq);
    void update(Word w);
    value_type total() const;
    value_type range_start(Word w) const;
    value_type range_end(Word w) const;

    Word find_word(value_type value, value_type low, 
                   value_type high) const;
};

template <>
struct MagicNumber<SemiAdaptiveModel> {
    static const std::uint32_t magic_number = 0x41437300; // "ACS\0"
};
#endif // __SEMIADAPTIVEMODEL_HH__

/*
 * =====================================================================================
 *
 *       Filename:  model.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/21/2014 10:54:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __MODEL_HH__
#define __MODEL_HH__

#include <cstdint>
#include <vector>
#include <cstdio>

struct CodingModel {
    typedef std::uint64_t value_type;
    typedef std::uint16_t Word;

    static const std::uint8_t _word_length = 8;
    static const Word nwords = ((Word)1 << _word_length) + 1;
    static const Word EOS = ((Word)1 << _word_length);

    static const value_type Code_value_bits = 22;
    static const value_type Top_value 
                    = ((value_type)1 << Code_value_bits) - 1;
    static const value_type First_qtr = Top_value/4 + 1;
    static const value_type Half = 2 * First_qtr;
    static const value_type Third_qtr = 3 * First_qtr;

    static const value_type TOP_MASK = 1 << (Code_value_bits - 1);
    static const value_type SECOND_MASK = 1 << (Code_value_bits - 2);

public:
    enum ModelType { InvalidModelType, AdaptiveModelType, SemiAdaptiveModelType };

public:
    CodingModel();

    static ModelType get_model(FILE *f);
};

template <typename Model> struct MagicNumber;

#endif // __MODEL_HH__

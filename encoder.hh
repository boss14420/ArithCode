/*
 * =====================================================================================
 *
 *       Filename:  encoder.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 07:31:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __ENCODER_HH__
#define __ENCODER_HH__

#include "bitstream.hh"

template <typename Model>
class Encoder
{
public:
    typedef typename Model::value_type value_type;
    typedef typename Model::Word Word;

    static const int _word_length = Model::_word_length;
    static const value_type First_qtr = Model::First_qtr;
    static const value_type Third_qtr = Model::Third_qtr;
    static const value_type Half = Model::Half;
    static const value_type Top_value = Model::Top_value;

private:
    value_type _high, _low, _bits_to_follow;
    BitStream &_ibs, &_obs;
    Model _model;

public:
    Encoder(BitStream &ibs, BitStream &obs, Model &model);

    void encode();

private:
    void start_encoding();
    void encode_word(Word w);
    void done_encoding();
    void output_bit_plus_follow(unsigned bit);
};

#include "encoder.cc"

#endif // __ENCODER_HH__

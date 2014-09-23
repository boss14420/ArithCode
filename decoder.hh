/*
 * =====================================================================================
 *
 *       Filename:  decoder.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 11:54:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __DECODER_HH__
#define __DECODER_HH__

#include "bitstream.hh"

template <typename Model>
class Decoder
{
    typedef typename Model::value_type value_type;
    typedef typename Model::Word Word;

    static const int _word_length = Model::_word_length;
    static const value_type First_qtr = Model::First_qtr;
    static const value_type Third_qtr = Model::Third_qtr;
    static const value_type Half = Model::Half;
    static const value_type Top_value = Model::Top_value;
    static const std::uint8_t Code_value_bits = Model::Code_value_bits;

private:
    value_type _high, _low, _value;
    BitStream &_ibs, &_obs;
    Model _model;

public:
    Decoder(BitStream &ibs, BitStream &obs, Model &model);

    void decode();

private:
    void start_decoding();
    Word decode_word();
};

#include "decoder.cc"

#endif // __DECODER_HH__

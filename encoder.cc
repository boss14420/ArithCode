/*
 * =====================================================================================
 *
 *       Filename:  encoder.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 06:31:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */


#include "bitstream.hh"
#include "encoder.hh"
#include <cstdio>

template <typename Model>
Encoder<Model>::Encoder(BitStream &ibs, BitStream &obs, Model &model)
    : _ibs(ibs), _obs(obs), _model(model)
{}

template <typename Model>
void Encoder<Model>::encode()
{
    start_encoding();
    Word w;
//    std::size_t encoded_words = 0;
    while (!_ibs.eof()) {
        w = _ibs.read<Word>(_word_length);
//        if (encoded_words == 0x38b383f) {
//            std::printf("offset 0x%lX\nw = 0x%X, [%lu, %lu] / %lu\nhigh = %lu, low = %lu, follow = %lu\n",
//                         encoded_words, w, _model.range_start(w), _model.range_end(w),
//                         _model.total(), _high, _low, _bits_to_follow);
//        }
        encode_word(w);
//        if (encoded_words == 0x38b383f) {
//            std::printf("high = %lu, low = %lu\n", _high, _low);
//        }
        _model.update(w);
//        ++encoded_words;
    }
    encode_word(Model::EOS);
    done_encoding();
    _obs.flush();
}

template <typename Model>
void Encoder<Model>::start_encoding()
{
    _low = 0;
    _high = Top_value;
    _bits_to_follow = 0;
}

template <typename Model>
void Encoder<Model>::encode_word(Word w)
{
    auto range = _high - _low + 1;
    _high = _low + (range * _model.range_end(w)) / _model.total() - 1;
    _low += (range * _model.range_start(w)) / _model.total();

    // msb of _high = msb of _low
    while ( ((_low ^ _high) & Model::TOP_MASK) == 0 ) {
        output_bit_plus_follow(_low >> (Model::Code_value_bits - 1));
        _low = (_low << 1) & Top_value;
        _high = ((_high << 1) & Top_value) | 1;
    } 

    // _low = 01xxxx && _high = 10yyyy
    while ( (_low & ~_high) & Model::SECOND_MASK ) {
        ++_bits_to_follow;
        // _low <-- xxxx0 = (_low - First_qtr) * 2
        _low = (_low << 1) & (Top_value >> 1);
        // _high <-- 1yyyy1 = (_high - First_qtr) * 2 + 1
        _high = (((_high | First_qtr) << 1) & Top_value) | 1;
    }
}

template <typename Model>
void Encoder<Model>::done_encoding()
{
    ++_bits_to_follow;
    if (_low < First_qtr) output_bit_plus_follow(0);
    else output_bit_plus_follow(1);
}

template <typename Model>
void Encoder<Model>::output_bit_plus_follow(unsigned bit)
{
    _obs.write(bit, 1);
    BitStream::Int value, fill = bit ^ 1;
    while (_bits_to_follow > BitStream::nbits) {
//        value = (fill << BitStream::nbits) - fill;
        value = BitStream::Int(0) - fill;
        _obs.write(value, BitStream::nbits);
        _bits_to_follow -= BitStream::nbits;
    }

    value = (fill << _bits_to_follow) - fill;
    _obs.write(value, _bits_to_follow);
    _bits_to_follow = 0;
}

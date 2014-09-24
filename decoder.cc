/*
 * =====================================================================================
 *
 *       Filename:  decoder.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 11:12:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */


#include "bitstream.hh"
#include "decoder.hh"

template <typename Model>
Decoder<Model>::Decoder(BitStream &ibs, BitStream &obs, Model &model)
    : _ibs(ibs), _obs(obs), _model(model)
{}

template <typename Model>
void Decoder<Model>::decode()
{
//    std::size_t decoded_words = 0;
    start_decoding();
    Word w;
    while (!_ibs.eof()) {
        // TODO: error at offset 0x38b383f whene Code_value_bits == 16
//        if (decoded_words == 0x38b383f) {
//            auto range = _high - _low + 1;
//            auto accumulate = (((_value - _low) + 1) * _model.total() - 1) / range;
//            std::printf("offset 0x%lX\nvalue = %lu, acc = %lu\n"
//                        "high = %lu, low = %lu\n",
//                         decoded_words, _value, accumulate,
//                         _high, _low);
//        }
        w = decode_word();
//        if (decoded_words == 0x38b383f) {
//            std::printf("w = 0x%X, [%lu, %lu] / %lu\n",
//                         0xFF, _model.range_start(0xFF), _model.range_end(0xFF), _model.total());
//            std::printf("w = 0x%X, [%lu, %lu] / %lu\nhigh = %lu, low = %lu\n",
//                         w, _model.range_start(w), _model.range_end(w),
//                         _model.total(), _high, _low);
//        }
        if (w == Model::EOS) break;
        _obs.write(w, _word_length);
        _model.update(w);
//        ++decoded_words;
    }
    
    if (w != Model::EOS)
        while (_value) {
            w = decode_word();
            if (w == Model::EOS) break;
            _obs.write(w, _word_length);
            _model.update(w);
//            ++decoded_words;
        }

    _obs.flush();
}

template <typename Model>
void Decoder<Model>::start_decoding()
{
    _low = 0;
    _high = Top_value;
    _value = _ibs.read<value_type>(Code_value_bits);
}

template <typename Model>
typename Decoder<Model>::Word Decoder<Model>::decode_word()
{
    auto range = _high - _low + 1;
    Word w = _model.find_word(_value, _low, _high);

    _high = _low + (range * _model.range_end(w)) / _model.total() - 1;
    _low += (range * _model.range_start(w)) / _model.total();
    if (_high < _low) _high = _low + 1;

    // msb of _high = msb of _low
    while (((_low ^ _high) & Model::TOP_MASK) == 0) {
        _low = (_low << 1) & Top_value;
        _high = ((_high << 1) & Top_value) | 1;
        _value = ((_value << 1) & Top_value) | _ibs.read<value_type>(1);
    }

    // _low = 01xxxx && _high = 10yyyy
    while ( (_low & ~_high) & Model::SECOND_MASK ) {
        // _low <-- xxxx0 = (_low - First_qtr) * 2
        _low = (_low << 1) & (Top_value >> 1);
        // _high <-- 1yyyy1 = (_high - First_qtr) * 2 + 1
        _high = (((_high | First_qtr) << 1) & Top_value) | 1;
        // _value <-- (_value - First_qtr) * 2 + read(1)
        _value = (((_value ^ First_qtr) << 1) & Top_value)
                    | _ibs.read<value_type>(1);
    }

    return w;
}

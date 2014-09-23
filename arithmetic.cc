/*
 * =====================================================================================
 *
 *       Filename:  arithmetic.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 05:26:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */


#include <cstdio>
#include <iostream>
#include <limits>
#include <algorithm>
#include <vector>
#include <deque>
#include <queue>
#include <cstring>
#include <type_traits>

#include "bitstream.hh"
#include "arithmetic.hh"
#include "integer.hpp"
#include "encoder.hh"
#include "decoder.hh"
#include "SemiAdaptiveModel.hh"

#define CANNOT_COMPRESS 0
#define END_OFFSET (_filesize*8 - _num_words*_word_length - _offset)

template <typename Model>
Arithmetic<Model>::Arithmetic(FILE* is, Action action) 
    : _is(is), _action(action)
{
    _word_length = DefaultWordLength;
    if (_action == Compress)
        init_compress();
    else if (_action == Decompress)
        init_decompress();
}

template <typename Model>
void Arithmetic<Model>::compress(FILE *os)
{
//    std::cout << "compress ...\n";
    if (_action == Compress) {
        write_header(os);
        encode(os);
    } else {
        throw InvalidAction();
    }
}

template <typename Model>
void Arithmetic<Model>::decompress(FILE *os)
{
//    std::cout << "decompress ...\n";
    if (_action == Decompress) {
        decode(os);
    } else {
        throw InvalidAction();
    }
}


////////////////////////////////////////////////////////////////////////////
//////// Compress methods //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

template <typename Model>
void Arithmetic<Model>::init_compress()
{
    auto pos = std::ftell(_is);
    _is_pos = pos;
//    std::fseek(_is, 0, SEEK_END);
    _filesize = std::ftell(_is) - pos;
//    std::fseek(_is, pos, SEEK_SET);

    _offset = 0;
    _num_words = (_filesize * 8 - _offset) / _word_length;
    _header_size = sizeof(magic_number) + 4 + 8 + 8 + 1;

    if (std::is_same<Model, SemiAdaptiveModel>::value)
        calculate_freq_table();
}

template <typename Model>
void Arithmetic<Model>::calculate_freq_table()
{
    auto pos = std::ftell(_is);
    std::fseek(_is, 0, SEEK_END);
    _filesize = std::ftell(_is) - pos;
    std::fseek(_is, pos, SEEK_SET);
    _offset = 0;
    _num_words = (_filesize * 8 - _offset) / _word_length;
//    _header_size += (1 << _word_length) * sizeof(Model::value_type);

    Word word;
    _freq_table.assign(1 << _word_length, 0);
    _is_pos = std::ftell(_is);
    BitStream ibs(_is, BitStream::Input);
    ibs.read<Word>(_offset); // first extra bits

    std::uint64_t reed_words = 0;
//    while (!ibs.eof()) {
    while (reed_words != _num_words) {
        word = ibs.read<Word>(_word_length);
//    while (!_is.eof()) {
//        _is.read(reinterpret_cast<char*>(&word), _word_length);
        ++_freq_table[word];
        ++reed_words;
    }
    ibs.read<Word>(END_OFFSET); // last extra bits
}

template <typename Model>
double Arithmetic<Model>::entropy(std::vector<std::size_t> const &_freq_table)
{
    //        std::size_t size = 0;
    double H = 0;
    //        for (auto freq : _freq_table) {
    //            size += _freq_table;
    //
    //        }
    return H;
}

template <typename Model>
void Arithmetic<Model>::encode(FILE* os) const noexcept
{
    std::clearerr(_is);
    std::fseek(_is, _is_pos, SEEK_SET);
    if (!CANNOT_COMPRESS) {
//        std::size_t encoded_words = 0;
//        Word word;
        BitStream obs(os, BitStream::Output);
        BitStream ibs(_is, BitStream::Input);
        
        Model model;
        model.init(_freq_table);
        Encoder<Model> encoder(ibs, obs, model);
//        word = ibs.read<Word>(_offset);
//        obs.write(word, _offset);

        encoder.encode();

//        word = ibs.read<Word>(END_OFFSET);
//        obs.write(word, END_OFFSET);

    } else {
        // copy file
        std::vector<char> buff(4 << 20); // 4MiB buffer
        while (!std::feof(_is)) {
            auto bytes = std::fread(buff.data(), 1, buff.size(), _is);
            std::fwrite(buff.data(), bytes, 1, os);
        }
    }
}

template <typename Model>
void Arithmetic<Model>::write_header(FILE* os) const
{
    char mn[sizeof(magic_number)];
    std::fwrite(int_to_bytes<decltype(magic_number)>(mn, magic_number), 
                sizeof(magic_number), 1, os);

    decltype(_header_size) freq_tab_size = 0;
    char value_size = 0;
    // TODO: precopute _header_size
    if (!CANNOT_COMPRESS && 
            std::is_same<Model, SemiAdaptiveModel>::value)
    {
        auto max = *std::max_element(_freq_table.begin(), _freq_table.end());
        for (; max; max >>= 1, ++value_size);
        freq_tab_size = 1 + (value_size * (1 << _word_length)) / 8;
    }

    char bytes[sizeof(_filesize)];
    // write header size
    std::fwrite(int_to_bytes<std::uint32_t>(bytes, 
                                    _header_size + freq_tab_size), 
                4, 1, os);
    // write file size
    std::fwrite(int_to_bytes<std::uint64_t>(bytes, _filesize), 8, 1, os);
    // write num of words
    std::fwrite(int_to_bytes<std::uint64_t>(bytes, _num_words), 8, 1, os);

    std::fwrite(&_word_length, 1, 1, os);

    if (!CANNOT_COMPRESS) {
        if (!std::is_same<Model, SemiAdaptiveModel>::value) return;

        // write freq table
        std::fwrite(&value_size, 1, 1, os);
        BitStream obs(os, BitStream::Output);
        for (auto f : _freq_table)
            obs.write(f, value_size);
        obs.flush();
    }
}


////////////////////////////////////////////////////////////////////////////
//////// Decompress methods ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

template <typename Model>
void Arithmetic<Model>::init_decompress()
{
//    std::cout << "read_header ...\n";
    read_header();
}

template <typename Model>
void Arithmetic<Model>::decode(FILE* os) const noexcept
{
    std::clearerr(_is);
    std::fseek(_is, _header_size, SEEK_SET);

    if (!CANNOT_COMPRESS) {
        BitStream obs(os, BitStream::Output);
        BitStream ibs(_is, BitStream::Input);
        
        Model model;
        model.init(_freq_table);
        Decoder<Model> decoder(ibs, obs, model);
//        word = ibs.read<Word>(_offset);
//        obs.write(word, _offset);

        decoder.decode();

//        word = ibs.read<Word>(END_OFFSET);
//        obs.write(word, END_OFFSET);
    } else {
        // copy file
        std::vector<char> buff(4 << 20); // 4MiB buffer
        while (!std::feof(_is)) {
            auto bytes = std::fread(buff.data(), 1, buff.size(), _is);
            std::fwrite(buff.data(), bytes, 1, os);
        }
    }
}

template <typename Model>
void Arithmetic<Model>::read_header()
{
    char mn[sizeof(magic_number)];
    std::fread(mn, 1, sizeof(magic_number), _is);
    if (bytes_to_int<decltype(magic_number)>(mn) != magic_number) {
        throw InvalidCompressFile();
    }

    char bytes[sizeof(_filesize)];
    std::fread(bytes, 1, sizeof(_header_size), _is);
    _header_size = bytes_to_int<decltype(_header_size)>(bytes);
    std::fread(bytes, 1, sizeof(_filesize), _is);
    _filesize = bytes_to_int<decltype(_filesize)>(bytes);
    std::fread(bytes, 1, sizeof(_num_words), _is);
    _num_words = bytes_to_int<decltype(_num_words)>(bytes);

    std::fread(&_word_length, 1, 1, _is);
    // variable word length not implemented yet
    _word_length = DefaultWordLength; 

    _is_pos = std::ftell(_is);

    if (CANNOT_COMPRESS) return;
    if (!std::is_same<Model, SemiAdaptiveModel>::value) return;

    // read freq table
    _freq_table.assign(1 << _word_length, 0);
    char value_size;
    std::fread(&value_size, 1, 1, _is);
    BitStream ibs(_is, BitStream::Input, value_size*(1<<_word_length)/8);
    if (std::is_same<Model, SemiAdaptiveModel>::value)
        for (Word w = 0; w != (Word)1 << _word_length; ++w) {
            _freq_table[w] = ibs.read<typename Model::value_type>(value_size);
        }

    _is_pos = std::ftell(_is);
}


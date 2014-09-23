/*
 * =====================================================================================
 *
 *       Filename:  arithmetic.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2014 07:58:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __ARITHMETIC_HH__
#define __ARITHMETIC_HH__

#include <cstdio>
#include <vector>
#include <unordered_map>
#include "model.hh"

struct InvalidAction : public std::exception {
    virtual char const *what() const noexcept override {
        return "Invalid action";
    }
};

struct InvalidCompressFile : public std::exception {
    virtual char const *what() const noexcept override {
        return "Invalid compress file";
    }
};

template <typename Model>
class Arithmetic {
public:
    typedef typename Model::Word Word;
    typedef std::uint64_t CodeWord;
    typedef std::uint8_t CodeLength;
//    typedef std::vector<bool> BitSet;
//    typedef std::bitset<64> BitSet;
    typedef CodeWord BitSet;
    static const std::uint32_t magic_number = MagicNumber<Model>::magic_number;
    static const std::uint8_t DefaultWordLength = Model::_word_length;

    enum Action { Compress, Decompress };

private:
    FILE *_is;
    Action _action;
    // TODO: variable word length
    std::uint8_t _word_length;

    long _is_pos;
    std::uint32_t _header_size;
    std::uint64_t _filesize;
    std::uint64_t _num_words;
    std::uint8_t _offset;
    std::vector<std::size_t> _freq_table;

public:
    Arithmetic(FILE *is, Action action);

    void compress(FILE *os);
    void decompress(FILE *os);

private:

////////////////////////////////////////////////////////////////////////////
//////// Compress methods //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    void init_compress();

    void calculate_freq_table();

    static double entropy(std::vector<std::size_t> const &freq_table);

    void encode(FILE* os) const noexcept;

    void write_header(FILE* os) const;

////////////////////////////////////////////////////////////////////////////
//////// Compress methods //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    void init_decompress();

    void read_header();

    void decode(FILE* os) const noexcept;
};

#include "arithmetic.cc"

#endif // __ARITHMETIC_HH__

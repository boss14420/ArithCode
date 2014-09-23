/*
 * =====================================================================================
 *
 *       Filename:  bitstream.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/07/2014 09:38:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BITSTREAM_HH__
#define __BITSTREAM_HH__

#include <cstdint>
#include <cstdio>
#include <vector>

#include "integer.hpp"

//template <typename Int = std::uint8_t>
class BitStream
{
public:
    typedef std::uint64_t Int;
    enum Type { Input, Output };

    static const std::size_t nbytes = sizeof(Int);
    static const std::size_t nbits = nbytes << 3;
    static const Int ONE = 1;

private:
    std::vector<Int> _buffer;
    unsigned _remainingBits, _end_offset;
    FILE *_ios;
    long _is_pos;
    Type _type;
    std::size_t _bufferSize;
    Int _bufftop;
    typename std::vector<Int>::iterator _bufferindex;

#define MASK_AND(n) ((ONE << n) - 1)

public:
    BitStream(FILE *ios, Type type, std::size_t bufferSize = 4 << 20);

    template<typename T> T read(int count)
    {
        T ret = 0;
        
        int diff = count - _remainingBits;
        if (diff < 0) {
            ret = (_bufftop >> -diff) & MASK_AND(count);
            _remainingBits = -diff;
        } else {
//        } else if (diff < (int)nbits) {
            // add first '_remainingBits' bits
            ret = _bufftop & MASK_AND(_remainingBits);
            seek_buffer();
            // add other 'diff' bits
            (ret <<= diff) |= (_bufftop >> (nbits - diff)) & MASK_AND(diff);
            _remainingBits = nbits - diff;
        } 
        // TODO: integer larger than 64 bits
/*         else {
 *             // add first '_remainingBits' bits
 *             ret = _bufftop & MASK_AND(_remainingBits);
 * 
 *             seek_buffer();
 *             while (diff >= (int)nbits) {
 *                 (ret <<= nbits) |= _bufftop;
 *                 diff -= nbits;
 *                 seek_buffer();
 *             }
 * 
 *             // add last 'diff' bits
 *             (ret <<= diff) |= (_bufftop >> (nbits - diff)) & MASK_AND(diff);                         
 *             _remainingBits = nbits - diff;
 *         }
 */

        return ret;
    }

    // write 'count' LSBs
    BitStream& write(Int value, int count);

    template <typename BitSet>
    friend 
    BitStream& operator<< (BitStream& stream, BitSet const &bitset)
    {
        return stream.write(bitset.begin(), bitset.end());
    }

    void flush(bool writeExtraBits = true);

    std::size_t bit_count() const {
        return nbits * _buffer.size() + (nbits - _remainingBits);
    }

    std::size_t remaining_bits() const { return _remainingBits; }

    bool eof() const {
        return (_bufferindex == _buffer.end() 
                || (_bufferindex + 1 == _buffer.end()
                        && _remainingBits <= _end_offset))
            && std::feof(_ios);
    }

    void reset();

private:
    void seek_buffer() {
        if (++_bufferindex == _buffer.end())
            fill_buffer();
        _bufftop = *_bufferindex;
    }

    void fill_buffer();

#undef MASK_AND
};

//extern template BitStream::Int 
//BitStream::read<BitStream::Int>(BitStream::Int);

#endif // __BITSTREAM_HH__

/*
 * =====================================================================================
 *
 *       Filename:  integer.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/05/2014 03:53:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __INTEGER_HH__
#define __INTEGER_HH__

#include <type_traits>
#if 0
#include "endian.hh"

template<int Pos>
void assign_big_endian(std::uint8_t *dst, std::uint8_t const *src) {
    *dst = *src;
    assign_big_endian<Pos - 1>(dst + 1, src - 1);
}

template<>
void assign_big_endian<0>(std::uint8_t *dst, std::uint8_t const *src) {
    *dst = *src;
}

///////////////////////////
//// int_to_bytes

template <typename T>
std::uint8_t*
int_to_bytes(std::uint8_t* bytes, 
             typename std::enable_if<HL_ENDIANNESS == HL_LITTLE_ENDIAN, T>::type integer)
{
    *reinterpret_cast<T*>(bytes) = integer;
    return bytes;
}

template <typename T>
std::uint8_t*
int_to_bytes(std::uint8_t* bytes,
             typename std::enable_if<HL_ENDIANNESS == HL_BIG_ENDIAN, T>::type integer)
{
    std::uint8_t* src = reinterpret_cast<std::uint8_t*>(&integer);
    assign_big_endian<sizeof(T) - 1>(bytes, src + sizeof(T) - 1);
    return bytes;
}


///////////////////////////
///// bytes_to_int

template <typename T>
typename std::enable_if<HL_ENDIANNESS == HL_LITTLE_ENDIAN, T>::type
bytes_to_int(std::uint8_t const *bytes) {
    return *reinterpret_cast<T const*>(bytes);
}

template <typename T>
typename std::enable_if<HL_ENDIANNESS == HL_BIG_ENDIAN, T>::type
bytes_to_int(std::uint8_t const *bytes) {
    T integer;
    std::uint8_t *ptr= reinterpret_cast<std::uint8_t*>(&integer);
    assign_big_endian<sizeof(T) - 1>(ptr, bytes + sizeof(T) - 1);
    return integer;
}
#endif // 0

template <int Pos, typename T>
struct _big_endian_helper {
    static void assign(char *dst, T integer) {
        *dst = static_cast<char>(integer & 0xFF);
        _big_endian_helper<Pos-1, T>::assign(dst - 1, integer >> 8);
    }

    static T int_value(std::uint8_t const *bytes) {
        return ((T)*bytes << Pos) | _big_endian_helper<Pos-8, T>::int_value(bytes+1);
    }
};

template <typename T>
struct _big_endian_helper<0, T> {
    static void assign(char *dst, T integer) {
        *dst = static_cast<char>(integer);
    }

    static std::uint8_t int_value(std::uint8_t const *bytes) {
        return *bytes;
    }
};


///////////////////////////
//// int_to_bytes

template <typename T>
typename std::enable_if<std::is_integral<T>::value, char*>::type
int_to_bytes(char* bytes, T integer)
{
//    *reinterpret_cast<T*>(bytes) = integer;
    typedef typename std::make_unsigned<T>::type UT;
    _big_endian_helper<sizeof(T) - 1, UT>::assign(bytes + sizeof(T) - 1, 
                                                static_cast<UT>(integer));
    return bytes;
}

///////////////////////////
//// bytes_to_int

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
bytes_to_int(char const* bytes)
{
    typedef typename std::make_unsigned<T>::type UT;
    return _big_endian_helper<(sizeof(T)-1) << 3, UT>::int_value(
                            reinterpret_cast<std::uint8_t const*>(bytes));
}

#endif // __INTEGER_HH__

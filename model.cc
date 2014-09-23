/*
 * =====================================================================================
 *
 *       Filename:  model.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/21/2014 11:07:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "model.hh"
#include "AdaptiveModel.hh"
#include "SemiAdaptiveModel.hh"
#include <algorithm>
#include "integer.hpp"

CodingModel::CodingModel()
{}

CodingModel::ModelType
CodingModel::get_model(FILE *f) {
    char magic_number[4] = {0, 0, 0, 0};
    std::fread(magic_number, 4, 1, f);
    std::ungetc(magic_number[3], f);
    std::ungetc(magic_number[2], f);
    std::ungetc(magic_number[1], f);
    std::ungetc(magic_number[0], f);

    switch (bytes_to_int<std::uint32_t>(magic_number)) {
        case MagicNumber<AdaptiveModel>::magic_number:
            return AdaptiveModelType;
        case MagicNumber<SemiAdaptiveModel>::magic_number:
            return SemiAdaptiveModelType;
        default:
            return InvalidModelType;
    }
}

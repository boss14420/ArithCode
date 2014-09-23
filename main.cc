/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/27/2014 05:20:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:  
 *
 * =====================================================================================
 */


#include "arithmetic.hh"
#include "SemiAdaptiveModel.hh"
#include "AdaptiveModel.hh"
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>

#define HELP_STRING (std::string("Usage: ") + argv[0] + " <command> <infile> <outfile> [a]\n" + \
                        "a: use adaptive model (only in compression mode)\n" + \
                        "<command>:\n c: compress <infile>\n x: decompress <infile>\n help: print this help\n" \
                        "<infile> = \"-\" use stdin\n" \
                        "<outfile> = \"-\" use stdout\n")

typedef Arithmetic<SemiAdaptiveModel> SemiAdaptiveArith;
typedef Arithmetic<AdaptiveModel> AdaptiveArith;

int main(int argc, char *argv[]) {

    FILE *is = NULL, *os = NULL;

    int retval = 0;
    if (argc > 1 && !std::strcmp(argv[1], "c")) {
        if (argc >= 4) {
            is = std::strcmp(argv[2], "-") ? std::fopen(argv[2], "rb") 
                                                : stdin;
            os = std::strcmp(argv[3], "-") ? std::fopen(argv[3], "wb")
                                                : stdout;

            if (!is) {
                std::cerr << "Cannot open file " << argv[2] << '\n';
                retval = 1;
                goto end;
            } else if (!os) {
                std::cerr << "Cannot open file " << argv[3] << '\n';
                retval = 1;
                goto end;
            }
        }

        bool use_adaptive = false;
        if (argc >= 5) {
            if (!std::strcmp(argv[4], "a"))
                use_adaptive = true;
        }

        try {
            if (use_adaptive || is == stdin) {
                AdaptiveArith ac1(is, AdaptiveArith::Compress);
                ac1.compress(os);
            } else {
                SemiAdaptiveArith ac2(is, SemiAdaptiveArith::Compress);
                ac2.compress(os);
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
            retval = 1;
            goto end;
        }
    } else if (argc > 1 && !std::strcmp(argv[1], "x")) {
        if (argc >= 4) {
            is = std::strcmp(argv[2], "-") ? std::fopen(argv[2], "rb") 
                                                : stdin;
            os = std::strcmp(argv[3], "-") ? std::fopen(argv[3], "wb")
                                                : stdout;

            if (!is) {
                std::cerr << "Cannot open file " << argv[2] << '\n';
                retval = 1;
                goto end;
            } else if (!os) {
                std::cerr << "Cannot open file " << argv[3] << '\n';
                retval = 1;
                goto end;
            }
        }

        try {
            switch (CodingModel::get_model(is)) {
                case AdaptiveModel::Type:
                {
                    AdaptiveArith ac1(is, AdaptiveArith::Decompress);
                    ac1.decompress(os);
                    break;
                }
                case SemiAdaptiveModel::Type:
                {
                    SemiAdaptiveArith ac2(is, SemiAdaptiveArith::Decompress);
                    ac2.decompress(os);
                    break;
                }
                default: 
                    throw InvalidCompressFile();
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
            retval = 1;
            goto end;
        }
    } else if (argc > 1 && !std::strcmp(argv[1], "help")) {
        std::cout << HELP_STRING << '\n';
    } else {
        std::cerr << "Invalid command\n\n";
        std::cerr << HELP_STRING << '\n';
        retval = 1;
    }

end:
    if(is) std::fclose(is);
    if(os) std::fclose(os);

    return retval;
}

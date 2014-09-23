#!/bin/bash - 
#===============================================================================
#
#          FILE: compare.sh 
# 
#         USAGE: ./compare.sh [file_list]
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: BOSS14420 (), 
#  ORGANIZATION: 
#       CREATED: 09/09/2014 08:34
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

AC_HOME="$(dirname $0)"
AC="$AC_HOME/ac"
HZ_HOME="$(dirname $0)/../huffman/"
HZ="$HZ_HOME/hz"

hline(){
    python2 -c "print '|' + '-'*17 + '|' + '-'*12 + '|' + '-'*17 +'|' + '-'*10 + \
                '|' + '-'*11 + '|' + '-'*15 + '|' + '-'*10 + '|' + '-'*9 + '|'"
}

compress_test(){
    file="$1"
    prog="$2"
    opt="$3"
    ext="$4"
    xext="$5"

    ctime=$(/usr/bin/time -f "%e" "$prog" c "$file" "$file".$ext $opt 2>&1 >/dev/null)
    hcsz=$(/bin/ls -lh "$file".$ext | cut -d" " -f5)iB
    sz=$(stat --printf "%s" "$file")
    csz=$(stat --printf "%s" "$file".$ext)
    ratio=$(echo "$csz / $sz" | bc -l)
    ratio=$(printf "%.03f" $ratio)
    printf " %-8s | %-09s | %-13s |" $hcsz $ratio "$ctime"s

    xtime=$(/usr/bin/time -f "%e" "$prog" x "$file".$ext "$file".$xext 2>&1 >/dev/null)
    printf " %-8s |" "$xtime"s

    md51=$(md5sum "$file" | cut -d" " -f1)
    md52=$(md5sum "$file".$xext | cut -d" " -f1)
    if [[ $md51 != $md52 ]]; then
        printf " %-9s |" "Lỗi"
    else
        printf " %-7s |" "OK"
        rm "$file".$ext "$file".$xext
    fi
    printf "\n"
}

hline
printf "| %-15s | %-10s | %-18s | %-8s | %-9s | %-13s | %-8s | %-6s |\n" \
    "File" "Dung lượng" "Thuật toán" "File nén" "Tỉ lệ nén" "Thời gian nén" "Giải nén" "K/t MD5"
hline

for file in "$@"
do
    filename=$(basename "$file")
    hsz=$(/bin/ls -lh "$file" | cut -d" " -f5)iB
    printf "| %-15s | %-10s |" "$filename" $hsz

    ## Arithmetic coding - adaptive algo
    algo="AC - adaptive"
    printf " %-15s |" "$algo"
    compress_test "$file" "$AC" "a" "aca" "acax"

    ## Arithmetic coding - semi-adpative algo
    algo="AC - semi-adapt"
    printf "|%-17s|%-12s| %-15s |" "" "" "$algo"
    compress_test "$file" "$AC" "s" "acs" "acsx"

    if [[ -x "$HZ" ]]; then
        ## Huffman coding - word length = 8
        algo="Huffman 8 bit"
        printf "|%-17s|%-12s| %-15s |" "" "" "$algo"
        compress_test "$file" "$HZ" "8" "hz8" "hz8x"

        ## Huffman coding - word length = 16
        algo="Huffman 16 bit"
        printf "|%-17s|%-12s| %-15s |" "" "" "$algo"
        compress_test "$file" "$HZ" "16" "hz16" "hz16x"
    fi

    hline
done

#hline

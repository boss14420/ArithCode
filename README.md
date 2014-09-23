# CHƯƠNG TRÌNH NÉN VÀ GIẢI NÉN FILE SỬ DỤNG MÃ SỐ HỌC (Arithmetic Coding)
===================================

## Thiết kế chương trình

### Thuật toán

Thuật toán nén và giải nén được sử dụng ở đây là thuật toán mã số học
(Arithmetic Coding). Trong đó: 
1. Sử dụng số nguyên để biểu diễn các khoảng, mặc định là [0, 2^22-1).
2. Độ dài mỗi từ (ký tự) là 8 bit, ngoài ra còn sử dụng thêm ký tự `EOS` để đánh dấu
việc kết thúc quá trình nén/giải nén. Như vậy bảng chữ cái có kích thước 257.

Thông tin về các thông số này được quy định trong file `model.hh`.

### Các mô hình (Model)
Trong quá trình nén/giải nén, ta cần truy xuất thông tin về các khoảng biểu
diễn tương ứng với mỗi ký tự. Để làm việc này, trong chương trình sử dụng hai
mô hình khác nhau.

#### Mô hình bán thích nghi (semi-adaptive model)
File mã nguồn: `SemiAdaptiveModel.hh`, `SemiAdaptiveModel.cc`.
Trong mô hình này, đầu tiên ta đọc hết file từ đầu đến cuối để xác định được
tần suất xuất hiện của mỗi ký tự. Dựa vào bảng tần suất này tính được khoảng
biểu diễn tương ứng cho mỗi ký tự.
Trong file nén cần phải lưu bảng tần suất để quá trình giải nén có thể khôi
phục lại được các thông tin về khoảng biểu diễn.

Nhược điểm của mô hình này là cần phải đọc file 2 lần, do đó không thích hợp
với đầu vào là dữ liệu thời gian thực.

#### Mô hình thích nghi (adaptive model)
File mã nguồn: `AdaptiveModel.hh`, `AdaptiveModel.cc`.
Trong mô hình này, ban đầu tần suât của tất cả các ký tự đều được gán cùng một
giá trị (ở đây là bằng 1). Sau đó trong quá trình nén/giải nén, cứ mỗi ký tự
được mã hóa/giải mã thì tần suất của nó được tăng thêm 1, đồng thời khoảng
tương ứng được cập nhật.
Trong quá trình cập nhật, nếu vượt quá khoảng lớn nhất cho phép (2^22 - 1) thì
ta giảm (scale down) đồng loạt tần suất của tất cả ký tự xuống còn một nửa (nếu
sau khi giảm tần suất của ký tự nào đó bằng 0 thì gán bằng 1).

Để việc cập nhật được xử lý một cách hiệu quả, ta sử dụng cây Fenwick (hay còn
gọi là cây chỉ số nhị phân - Binary Indexed Tree). Do đó mỗi một phép cập nhật
và truy xuất khoảng đều mất O(log m) thời gian (m là kích thước bảng chữ cái).
Nếu dùng mảng tổng các tần suất như mô hình bán thích nghi thì mất O(m) thời
gian cập nhật và O(1) thời gian truy xuất.

Ưu điểm của mô hình này là nén được dữ liệu thời gian thực. Tuy nhiên chậm hơn
so với mô hình nửa thích nghi vì các phép thao tác với khoảng chậm hơn.

### Cấu trúc file nén
Nội dung 1 file nén bao gồm:

1. Phần tiêu đề (xem phương thức `Arithmetic::write_header()`)
    1. Magic number: số `0x41434100` (`ACA\0`) nếu nén theo mô hình thích nghi,
    `0x41435300` (`ACS\0`) nếu nén theo mô hình bán thích nghi. 4 byte.
    2. Kích thước phần tiêu đề tính theo byte (4 byte, Big Endian)
    3. Kích thước file gốc: 8 byte, được ghi theo kiểu Big Endian
    4. Số lượng từ trong file gốc: 8 byte, Big Endian
    5. Độ dài một từ (`wl` tính theo bit): 1 byte. Hiện tại chương trình chưa
    cài đặt tính năng cho phép độ dài từ tùy ý nên giá trị của trường này luôn
    là 8.
    6. Số bit để biểu diễn tần suất của mỗi ký tự, chính là số bit để biểu diễn
    tần suất lớn nhất: 1 byte. (trong trường hợp 
    7. Bảng tần suất: gồm 256 phần tử, mỗi phần tử biểu diễn bằng số bit được
    quy định ở trưòng 6.
    8. Nếu 7 phần trên chưa tròn byte thì chèn thêm các bit 0 vào cuối phần tiêu đề cho tròn byte.

2. Phần nội dung:
    1. Kết quả của thuật toán mã số học tương ứng với file đầu vào,
    2. Mã số học tương ứng với ký tự EOS,
    3. Chèn thêm các bit 0 vào sau nếu chưa đủ 1 byte.

Chương trình giải nén cần đọc thông tin từ phần tiêu để đề khôi phục các cấu
trúc dữ liệu cần cho việc giải nén theo thuật toán đã nói trên. Phương thức
`Arithmetic::read_header()` sẽ làm việc này.

---
## Chạy chương trình
    
Để nén một file `filename` và tạo ra file nén `filename.hz`, ta sử dụng lệnh:
    `./ac c filename filename.ac`

Để sử dụng mô hình thích nghi (adaptive model, mặc định là mô hình bán thích
nghi - semiadaptive model), ta thêm `a` vào cuối câu lệnh:
    `./ac c filename filename.ac a`

Giải nén file nén trên thành file `filename.ex`, ta sử dụng lệnh:
    `./ac x filename.ac filename.ex`

Để nén dữ liệu thời gian thực (từ đầu vào chuẩn `stdin`), ta sử dụng `-` thay
cho tên file đầu vào. VD: nén thư mục hiện tại, kết hợp với `tar`:
    `tar cvf - . | ./ac c - directory.tar.ac a`
Chú ý: chỉ có mô hình thích nghi mới nén được dữ liệu thời gian thực. Nếu không
thêm `a` vào cuối câu lệnh thì chương trình cũng sẽ tự động chọn mô hình này.

Để giải nén ra đầu ra chuẩn (`stdout`), ta sử dụng `-` thay cho tên file đầu
ra. VD: giải nén thư mục vừa nén vào thư mục `tmp`, kết hợp với `tar`:
    `mkdir tmp`
    `./ac x directory.tar.ac - | tar xvf - -C tmp`

----
## Kết quả chạy

Chương trình được chạy thử trên máy tính với CPU Intel Pentium B940 2GHz, RAM 3GiB, tốc độ quay của HDD là 5400rpm. Hệ điều hành Linux 3.16.1, 64bit.

Bảng kết quả sau đầy là kết quả của chạy 4 thuật toán với cùng tập file đầu
vào:
1. Mã số học, mô hình thích nghi
2. Mã số học, mô hình bán thích nghi
3. Mã Huffman, độ dài từ là 8 bit (https://github.com/boss14420/Huffman)
4. Mã Huffman, độ dài từ là 16 bit.

| File            | Dung lượng | Thuật toán      | File nén | Tỉ lệ nén | Thời gian nén | Giải nén | K/t MD5 |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| E.coli          | 4.5MiB     | AC - adaptive   | 1.2MiB   | 0.250     | 0.53s         | 0.66s    | OK      |
|                 |            | AC - semi-adapt | 1.2MiB   | 0.250     | 0.42s         | 0.58s    | OK      |
|                 |            | Huffman 8 bit   | 1.2MiB   | 0.250     | 0.14s         | 0.08s    | OK      |
|                 |            | Huffman 16 bit  | 1.2MiB   | 0.250     | **0.06s**         | **0.05s**    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| alice29.txt     | 149KiB     | AC - adaptive   | 86KiB    | 0.573     | 0.02s         | 0.02s    | OK      |
|                 |            | AC - semi-adapt | 86KiB    | 0.574     | 0.02s         | 0.02s    | OK      |
|                 |            | Huffman 8 bit   | 86KiB    | 0.577     | 0.02s         | 0.02s    | OK      |
|                 |            | Huffman 16 bit  | **77KiB**    | **0.516**    | 0.02s         | 0.02s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| asyoulik.txt    | 123KiB     | AC - adaptive   | 74KiB    | 0.603     | 0.02s         | 0.02s    | OK      |
|                 |            | AC - semi-adapt | 74KiB    | 0.605     | 0.02s         | 0.02s    | OK      |
|                 |            | Huffman 8 bit   | 75KiB    | 0.606     | **0.01s**         | 0.02s    | OK      |
|                 |            | Huffman 16 bit  | **66KiB**    | **0.533**     | 0.02s         | 0.02s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| bible.txt       | 3.9MiB     | AC - adaptive   | 2.1MiB   | 0.543     | 0.68s         | 0.70s    | OK      |
|                 |            | AC - semi-adapt | 2.1MiB   | 0.543     | 0.54s         | 0.60s    | OK      |
|                 |            | Huffman 8 bit   | 2.2MiB   | 0.548     | 0.09s         | 0.17s    | OK      |
|                 |            | Huffman 16 bit  | **1.9MiB**   | **0.478**     | **0.06s**         | **0.11s**    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| cp.html         | 25KiB      | AC - adaptive   | 16KiB    | 0.663     | **0.00s**         | **0.00s**    | OK      |
|                 |            | AC - semi-adapt | 17KiB    | 0.669     | **0.00s**         | **0.00s**    | OK      |
|                 |            | Huffman 8 bit   | 16KiB    | 0.664     | 0.01s         | 0.01s    | OK      |
|                 |            | Huffman 16 bit  | **16KiB**    | **0.641**     | 0.01s         | 0.01s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| fields.c        | 11KiB      | AC - adaptive   | 7.1KiB   | 0.644     | **0.00s**         | **0.00s**    | OK      |
|                 |            | AC - semi-adapt | 7.3KiB   | 0.663     | **0.00s**         | **0.00s**    | OK      |
|                 |            | Huffman 8 bit   | 7.0KiB   | 0.642     | 0.01s         | 0.01s    | OK      |
|                 |            | Huffman 16 bit  | **6.8KiB**   | **0.616**     | 0.02s         | 0.01s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| grammar.lsp     | 3.7KiB     | AC - adaptive   | 2.3KiB   | 0.625     | **0.00s**         | **0.00s**    | OK      |
|                 |            | AC - semi-adapt | 2.5KiB   | 0.673     | **0.00s**         | **0.00s**    | OK      |
|                 |            | Huffman 8 bit   | **2.3KiB**   | **0.615**     | 0.01s         | 0.01s    | OK      |
|                 |            | Huffman 16 bit  | 2.5KiB   | 0.661     | 0.02s         | 0.01s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| kennedy.xls     | 1006KiB    | AC - adaptive   | 450KiB   | 0.447     | 0.15s         | 0.16s    | OK      |
|                 |            | AC - semi-adapt | 450KiB   | 0.447     | 0.13s         | 0.13s    | OK      |
|                 |            | Huffman 8 bit   | 452KiB   | 0.449     | **0.03s**         | 0.04s    | OK      |
|                 |            | Huffman 16 bit  | **405KiB**   | **0.402**     | **0.03s**         | **0.03s**    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| lcet10.txt      | 417KiB     | AC - adaptive   | 244KiB   | 0.584     | 0.07s         | 0.07s    | OK      |
|                 |            | AC - semi-adapt | 244KiB   | 0.585     | 0.06s         | 0.06s    | OK      |
|                 |            | Huffman 8 bit   | 245KiB   | 0.587     | **0.02s**         | 0.03s    | OK      |
|                 |            | Huffman 16 bit  | **217KiB**   | **0.520**     | **0.02s**         | **0.02s**    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| plrabn12.txt    | 471KiB     | AC - adaptive   | 267KiB   | 0.567     | 0.08s         | 0.08s    | OK      |
|                 |            | AC - semi-adapt | 268KiB   | 0.568     | 0.06s         | 0.07s    | OK      |
|                 |            | Huffman 8 bit   | 270KiB   | 0.572     | **0.02s**         | 0.03s    | OK      |
|                 |            | Huffman 16 bit  | **236KiB**   | **0.500**     | **0.02s**         | **0.02s**    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| ptt5            | 502KiB     | AC - adaptive   | **77KiB**    | **0.152**     | 0.05s         | 0.06s    | OK      |
|                 |            | AC - semi-adapt | 77KiB    | 0.153     | 0.04s         | 0.05s    | OK      |
|                 |            | Huffman 8 bit   | 105KiB   | 0.208     | **0.02s**         | 0.04s    | OK      |
|                 |            | Huffman 16 bit  | 80KiB    | **0.158**     | **0.02s**         | **0.02s**    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| sum             | 38KiB      | AC - adaptive   | 26KiB    | 0.671     | **0.00s**         | **0.00s**    | OK      |
|                 |            | AC - semi-adapt | 26KiB    | 0.679     | 0.01s         | **0.00s**    | OK      |
|                 |            | Huffman 8 bit   | 26KiB    | 0.678     | 0.01s         | 0.01s    | OK      |
|                 |            | Huffman 16 bit  | **25KiB**    | **0.649**     | 0.02s         | 0.01s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| world192.txt    | 2.4MiB     | AC - adaptive   | 1.5MiB   | 0.625     | 0.48s         | 0.45s    | OK      |
|                 |            | AC - semi-adapt | 1.5MiB   | 0.625     | 0.36s         | 0.38s    | OK      |
|                 |            | Huffman 8 bit   | 1.5MiB   | 0.630     | 0.06s         | 0.11s    | OK      |
|                 |            | Huffman 16 bit  | **1.3MiB**   | **0.545**     | **0.05s**         | **0.08s**    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| xargs.1         | 4.2KiB     | AC - adaptive   | 2.7KiB   | 0.653     | **0.00s**         | **0.00s**    | OK      |
|                 |            | AC - semi-adapt | 2.9KiB   | 0.695     | **0.00s**         | **0.00s**    | OK      |
|                 |            | Huffman 8 bit   | **2.7KiB**   | **0.643**     | 0.01s         | 0.01s    | OK      |
|                 |            | Huffman 16 bit  | 3.0KiB   | 0.720     | 0.01s         | 0.01s    | OK      |


Thử nghiệm với những file kích thước lớn:

| File            | Dung lượng | Thuật toán      | File nén | Tỉ lệ nén | Thời gian nén | Giải nén | K/t MD5 |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| doc.pdf         | 253MiB     | AC - adaptive   | 249MiB   | 0.984     | 76.06s        | 64.86s   | OK      |
|                 |            | AC - semi-adapt | 249MiB   | 0.984     | 56.46s        | 55.44s   | OK      |
|                 |            | Huffman 8 bit   | 250MiB   | 0.988     | 5.59s         | **8.77s**    | OK      |
|                 |            | Huffman 16 bit  | **239MiB**   | **0.946**     | **3.89s**         | 8.96s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| music.flac      | 38MiB      | AC - adaptive   | **38MiB**    | **0.999**     | 10.65s        | 9.46s    | OK      |
|                 |            | AC - semi-adapt | **38MiB**    | **0.999**     | 7.87s         | 8.34s    | OK      |
|                 |            | Huffman 8 bit   | 38MiB    | 1.000     | **0.28s**         | **0.05s**    | OK      |
|                 |            | Huffman 16 bit  | 38MiB    | 1.003     | 0.65s         | 0.89s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| libwireshark.so | 65MiB      | AC - adaptive   | 35MiB    | 0.539     | 12.87s        | 11.18s   | OK      |
|                 |            | AC - semi-adapt | 40MiB    | 0.610     | 9.45s         | 9.41s    | OK      |
|                 |            | Huffman 8 bit   | 40MiB    | 0.613     | 1.28s         | 2.44s    | OK      |
|                 |            | Huffman 16 bit  | **33MiB**    | **0.510**     | **0.90s**         | **2.11s**    | OK      |

### Nhận xét:
Nhìn chung thì thuật toán Huffman với từ 16bit cho thời gian chạy và tỉ lệ nén
tốt nhất với những file lớn. Tuy nhiên với cùng độ dài từ 8 bit thì hai mô hình AC cho
tỉ lệ nén tốt hơn, trong đó mô hình thích nghi có tỉ lệ nén tốt hơn một
ít so với mô hình bán thích nghi. Có thể là do mô hình thích nghi không phải
lưu bảng tần suất, và do tính chất `recency` của dữ liệu đầu vào.

Về thời gian chạy thì thuật toán Huffman tốt hơn AC bán thích nghi, AC thích
nghi là kém nhất. Đó là do khối lượng tính toán để mã hóa được một từ của AC
lớn hơn.

Bảng kết quả trên được tạo ra bằng cách chạy script `compare.sh` đi kèm:
    `./compare.sh <all test file>`
Với `<all test file>` là các file cần nén.

Để so sánh được với các thuật toán Huffman thì cần có file thực thi `hz` (mã
ngồn trong link github ở trên), mặc định là ở thư mục `../huffman/`. Nếu không
có ở đó thì có thể sửa file `compare.sh`.

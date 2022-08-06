# Data compression algorithms

There are implementation of some lossless data compression algorithms: Huffman algorithm, Arithmetic coding and BWT + RLE;

## Huffman codes

Typical [Huffman coding](https://en.wikipedia.org/wiki/Huffman_coding).

### Strength

 - fast (two passes through data)
 - good compression when symbol frequencies
   are not the same
 - simple realisation

### Weaknesses

- useless when symbol frequencies are the same
- need knowledge of probability mass functions

## Arithmetic coding

[wiki](https://en.wikipedia.org/wiki/Arithmetic_coding)

My optimization: I cut the number when they are too long 
[...010101; ...011101]  ->(cutting) [...01011; ...0111]).

### Strength

- usually compress better than Huffman coding

### Weaknesses

- much slower than Huffman coding
- complex realization.
- need knowledge of probability mass functions

## BWT + RLE

BWT is [Burrows–Wheeler transform](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform)

RLE is [Run-length encoding](https://en.wikipedia.org/wiki/Run-length_encoding)

### Strength

- don't require knowledge of probability mass functions

### Weaknesses

- don't compress chaotic flow of byte even if there are not all bytes (don't compress pi number).

## Comparison

Ratio = initial file size / output file size

#### Sample1 (some text)

| Algorithm   | Ratio   | Compression time | Decompression time |
|-------------|---------|------------------|--------------------|
| Huffman     | 1.5218  | 0.005078 sec     | 0.006693 sec       |
| Arithmetic  | 1.52645 | 0.033228 sec     | 0.234215sec        |
| BWT + RLE   | 1.22188 | 0.01762sec       | 0.003987sec        |

#### Sample2 (bmp file with mono color background)

| Algorithm   | Ratio    | Compression time | Decompression time |
|-------------|----------|------------------|--------------------|
| Huffman     | 3.92078  | 0.00752 sec      | 0.027075 sec       |
| Arithmetic  | 4.58842  | 0.235208 sec     | 2.79652 sec        |
| BWT + RLE   | 16.5014  | 0.075627 sec     | 0.029445 sec       |

#### Sample3 (million digits of pi)

| Algorithm   | Ratio    | Compression time | Decompression time |
|-------------|----------|------------------|--------------------|
| Huffman     | 2.26211  | 0.078179 sec     | 0.088413 sec       |
| Arithmetic  | 2.34981  | 1.06736 sec      | 15.3487 sec        |
| BWT + RLE   | 0.996731 | 0.455272 sec     | 0.071235 sec       |


## How to run

```
cmake .
make dca
./dca -c (-u) -f [input file] -o [output file] -[algo name]
```

-c - compression
-u - decompression

algo names: btw - BTW + RLE, huff - Huffman coding, ar - Arithmetic coding

You can find samples and program arguments for them is `samples` folder.
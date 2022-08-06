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


## How to run

```
cmake .
make dca
./dca -c (-u) -f [input file] -o [output file] -[algo name]
```

-c - compression
-u - decompression

algo names: btw - BTW + RLE, huff - Huffman coding, ar - Arithmetic coding

You can find samples and program arguments for them is `sample` folder.
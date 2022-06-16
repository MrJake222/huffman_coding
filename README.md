# Adaptive Huffman coding in C++

## Bits and pieces used
* Vitter algorithm,
* Own BitArray implementation (extensible array of bits implemented as a template over numeric types),
* Own LinkList implementation as a template,
* Own progress printer (simple ASCII one),
* [CLI11](https://github.com/CLIUtils/CLI11) command line options parser,
* Extracts from Wikipedia as test data.

## Usage
### Quick test run
```
$ make -j4 runtests
```

### Help
```
$ ./main --help                                         
Adaptive Huffman coding compressor/decompressor
Usage: ./main [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -p,--pack Excludes: --unpack
                              Pack/compress
  -u,--unpack Excludes: --pack
                              Unpack/decompress
  -s,--source TEXT REQUIRED   Source/input file
  -d,--destination TEXT REQUIRED
                              Destination/output file

no action specified, use exactly one of pack/unpack options
```

### Encode
```
$ ./main --pack -s txt/1-passages-head_1K.tsv -d out.bin
```

### Decode
```
$ ./main --unpack -s out.bin -d decoded.txt
```

## Results
Effective for files as small as 1 KiB:
```
encoding: txt/1-passages-head_1K.tsv --> out.bin
[####################################################################################################] 100% done
bytes input 1024 output 709
size reduction 30.76%
took 0.00s
```

Example run for 10MB:
```
encoding: txt/5-passages-head_10M.txt --> out.bin
[####################################################################################################] 100% done
bytes input 10485760 output 6774613
size reduction 35.39%
took 0.85s
```

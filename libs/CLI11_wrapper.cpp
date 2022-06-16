#include "CLI11_wrapper.h"

// source: https://github.com/CLIUtils/CLI11
#include "external/CLI11.hpp"

int parse(int argc, char** argv, std::string& source_path, std::string& destination_path, bool& encode, bool& decode) {

    CLI::App app{"Adaptive Huffman coding compressor/decompressor"};

    CLI::Option* pack = app.add_flag("-p,--pack", encode, "Pack/compress");
    CLI::Option* unpack = app.add_flag("-u,--unpack", decode, "Unpack/decompress");
    pack->excludes(unpack);
    unpack->excludes(pack);

    app.add_option("-s,--source", source_path, "Source/input file")->required();
    app.add_option("-d,--destination", destination_path, "Destination/output file")->required();

    CLI11_PARSE(app, argc, argv);

    return 0;
}

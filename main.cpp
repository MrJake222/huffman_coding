#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
#include <string>
#include <filesystem>

#include "libs/huffman.hpp"
#include "libs/CLI11_wrapper.hpp"
#include "libs/progress_printer.hpp"

int main(int argc, char** argv) {

    // parse command line options using non-standard library
    // CLI11 (https://github.com/CLIUtils/CLI11)
    std::string source_path, destination_path;
    bool encode = false, decode = false;
    if (parse(argc, argv, source_path, destination_path, encode, decode)) {
        return 1;
    }

    if (!encode && !decode) {
        // neither given
        cout << "no action specified, use exactly one of pack/unpack options" << endl;
        return 1;
    }

    try {
        // get file size of input
        std::filesystem::path path(source_path);
        std::uintmax_t source_size = std::filesystem::file_size(path);

        // open file streams
        std::fstream in, out;
        in.open(source_path, std::ios::in | std::ios::binary);
        out.open(destination_path, std::ios::out | std::ios::binary);

        // create Huffman coder
        hf::Huffman coder(in, out);

        // create progress printer
        ProgressPrinter printer(source_size);
        coder.set_progress_printer(&printer);
        coder.set_bytes_per_update(source_size / 1000 + 1); // update every 0.1%

        // do the job
        if (encode) {
            cout << "encoding: " << source_path << " --> " << destination_path << endl;
            coder.encode();
        }
        else if (decode) {
            cout << "decoding: " << source_path << " --> " << destination_path << endl;
            coder.decode();
        }

        // close file streams
        in.close();
        out.close();
    }
    catch (const std::filesystem::filesystem_error& e) {
        cout << e.what() << endl;
    }
}

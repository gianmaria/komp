#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::string_view;
using std::ifstream;
using std::ofstream;
using std::vector;

namespace fs = std::filesystem;

#include "win_cpp_crypt.h"

int compress_file(const string& input_file, const string& output_file)
{
    ifstream ifs(input_file, std::ios::binary);
    if (!ifs)
    {
        cerr << "Error: cannot open input file '" << input_file << "'" << endl;
        return 1;
    }

    ofstream ofs(output_file, std::ios::binary);
    if (!ofs)
    {
        cerr << "Error: cannot open output file '" << output_file << "'" << endl;
        return 1;
    }

    auto input_file_size = fs::file_size(input_file);

    vector<char> input_file_content(input_file_size);

    ifs.read(input_file_content.data(), input_file_content.size());

    if (not ifs.good())
    {
        cerr << "Error: only " << ifs.gcount() << " bytes could be read from " << input_file << endl;
        return 1;
    }

    cout << "compressing " << input_file << " to " << output_file << endl;

    auto compressed = WinCppCrypt::Util::compress(input_file_content.data(), input_file_content.size());

    ofs.write(reinterpret_cast<char*>(compressed.data()), compressed.size());

    if (not ofs.good())
    {
        cerr << "Error: only " << ofs.tellp() << " bytes could be written to " << output_file << endl;
        return 1;
    }

    cout << "original size  : " << input_file_content.size() << " bytes" << endl;
    cout << "compressed size: " << compressed.size() << " bytes" << endl;
    float c = compressed.size();
    float d = input_file_content.size();
    float saved = (d - c) / d * 100;
    cout << "saved: " << saved << "%" << endl;

    return 0;
}

int decompress_file(const string& input_file, const string& output_file)
{
    ifstream ifs(input_file, std::ios::binary);
    if (!ifs)
    {
        cerr << "Error: cannot open input file '" << input_file << "'" << endl;
        return 1;
    }

    ofstream ofs(output_file, std::ios::binary);
    if (!ofs)
    {
        cerr << "Error: cannot open output file '" << output_file << "'" << endl;
        return 1;
    }

    auto input_file_size = fs::file_size(input_file);

    vector<char> compressed_file_content(input_file_size);

    ifs.read(compressed_file_content.data(), compressed_file_content.size());

    if (not ifs.good())
    {
        cerr << "Error: only " << ifs.gcount() << " bytes could be read from " << input_file << endl;
        return 1;
    }

    cout << "decompressing " << input_file << " to " << output_file << endl;

    auto decompressed = WinCppCrypt::Util::decompress(compressed_file_content.data(), compressed_file_content.size());

    ofs.write(reinterpret_cast<char*>(decompressed.data()), decompressed.size());

    if (not ofs.good())
    {
        cerr << "Error: only " << ofs.tellp() << " bytes could be written to " << output_file << endl;
        return 1;
    }

    cout << "done" << endl;

    return 0;
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            cerr << "Usage:" << endl
                << "    komp (-c|-d) <input_file> <output_file>" << endl;

            return 1;
        }

        string_view mode = argv[1];

        if (not (mode == "-c" || mode == "-d"))
        {
            cerr << "Error: invalid mode '" << mode << "'" << endl;
            return 1;
        }

        if (mode == "-c")
        {
            return compress_file(argv[2], argv[3]);
        }
        else
        {
            return decompress_file(argv[2], argv[3]);
        }

    }
    catch (const std::exception& e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

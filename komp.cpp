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

    vector<char> buffer(input_file_size);

    ifs.read(buffer.data(), buffer.size());

    if (not ifs.good())
    {
        cerr << "Error: only " << ifs.gcount() << " bytes could be read from " << input_file << endl;
        return 1;
    }

    cout << "compressing " << input_file << " to " << output_file << endl;

    auto compressed = WinCppCrypt::Util::compress(buffer.data(), buffer.size());

    ofs.write(reinterpret_cast<char*>(compressed.data()), compressed.size());

    if (not ofs.good())
    {
        cerr << "Error: only " << ofs.tellp() << " bytes could be written to " << output_file << endl;
        return 1;
    }

    cout << "original size  : " << buffer.size() << " bytes" << endl;
    cout << "compressed size: " << compressed.size() << " bytes" << endl;
    float c = compressed.size();
    float d = buffer.size();
    float saved = (d - c) / d * 100;
    cout << "saved: " << saved << "%" << endl;

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
            // TODO:
            // decompress_file(argv[2], argv[3]);
        }

    }
    catch (const std::exception& e)
    {
        cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}

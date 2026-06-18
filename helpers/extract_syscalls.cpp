//
// Created by matevz on 6/18/26.
//

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

// <td style="text-align:center">0</td>
// <td style="text-align:center">io_setup</td>
// <td style="text-align:center">man/ cs/</td>
// <td style="text-align:center">0</td>
// <td style="text-align:center">unsigned nr_reqs</td>
// <td style="text-align:center">aio_context_t *ctx</td>
// <td style="text-align:center">-</td>
// <td style="text-align:center">-</td>
// <td style="text-align:center">-</td>
// <td style="text-align:center">-</td>

struct TableEntry {
    std::string nr;
    std::string name;
    std::string references;
    std::string hex;
    std::string x0;
    std::string x1;
    std::string x2;
    std::string x3;
    std::string x4;
    std::string x5;
};

// case 25: return {
//     .nr = 25,
//     .name = "fcntl",
//     .args = {
//         {0, "fd"},
//         {1, "cmd"},
//         {2, "arg"}
//     }
// };

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <output-file>\n";
        return 1;
    }

    std::vector<TableEntry> entries;

    for (int k = 0; k <= 291; k++)
    {
        TableEntry e;

        std::getline(std::cin, e.nr, '\n');
        std::getline(std::cin, e.name, '\n');
        std::getline(std::cin, e.references, '\n');
        std::getline(std::cin, e.hex, '\n');
        std::getline(std::cin, e.x0, '\n');
        std::getline(std::cin, e.x1, '\n');
        std::getline(std::cin, e.x2, '\n');
        std::getline(std::cin, e.x3, '\n');
        std::getline(std::cin, e.x4, '\n');
        std::getline(std::cin, e.x5);

         if (!std::cin) {
            std::cerr << "failed to read entry " << k << '\n';
            return 1;
        }

        entries.push_back(e);
    }


    std::ofstream file(argv[1], std::ios::app);
    if (!file.is_open())
    {
        throw std::runtime_error("file not open");
    }

    for (std::size_t k = 0; k < entries.size(); k++)
    {

        // .args = {
        //     {0, "fd"}
        //     {1, "cmd"}
        //     {2, "arg"
        // }
        TableEntry& e = entries[k];
        std::vector args = {
            e.x0, e.x1, e.x2, e.x3, e.x4, e.x5
        };

        file
            << "case " << e.nr << ": return {\n"
            << "\t.nr = " << e.nr << ",\n"
            << "\t.name = \"" << e.name << "\",\n"
            << "\t.args = {\n";

        for (std::size_t i = 0; i < args.size(); ++i) {
            if (args[i] != "-") {
                file << "\t\t{" << i << ", \"" << args[i] << "\"},\n";
            }
        }

        file
            << "\t}\n"
            << "};\n";
    }

    file.close();

    return 0;
}
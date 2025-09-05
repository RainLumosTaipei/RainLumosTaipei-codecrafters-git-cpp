//
// Created by zhou_zhengming on 2025/8/20.
//

#include "cmd/cmd.h"
#include "zlib.h"
#include <openssl/sha.h>
#include <iostream>
#include <filesystem>
#include <fstream>

using CmdFn = int(*)(Cmd&);
using CmdMap = std::unordered_map<std::string, CmdFn>;

namespace {
    int init(Cmd& cmd){
        try {
            std::filesystem::create_directory(".git");
            std::filesystem::create_directory(".git/objects");
            std::filesystem::create_directory(".git/refs");

            std::ofstream headFile(".git/HEAD");
            if (headFile.is_open()) {
                headFile << "ref: refs/heads/main\n";
                headFile.close();
            } else {
                std::cerr << "Failed to create .git/HEAD file.\n";
                return EXIT_FAILURE;
            }

            std::cout << "Initialized git directory\n";
            return EXIT_SUCCESS;

        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
    }

    int catFile(Cmd& cmd){
        std::string hashStr = *cmd.args.rbegin();
        std::string dir = ".git/objects/" + hashStr.substr(0,2);
        std::ifstream file(dir + "/" + hashStr.substr(2));
        if(!file) return EXIT_FAILURE;

        std::stringstream ss;
        ss << file.rdbuf();
        std::string sourceStr = ss.str();

        uLongf destLen = 10 * sourceStr.size();
        std::vector<Bytef> destBuffer(destLen);

        int res = uncompress(
                destBuffer.data(),
                &destLen,
                reinterpret_cast<const Bytef*>(sourceStr.data()),
                sourceStr.size()
                );

        destBuffer.resize(destLen);

        //if(res != Z_OK) return EXIT_FAILURE;

        int i = 5;
        for(; i < destLen ; ++i)
            if(destBuffer[i] == '\0') break;

        std::cout << destBuffer.data() + i + 1;

        return EXIT_SUCCESS;
    }

    int hashObject(Cmd& cmd){
        std::ifstream file(*cmd.args.rbegin(), std::ios::binary);
        if (!file) return EXIT_FAILURE;

        std::stringstream ss;
        ss << file.rdbuf();

        // 加上header
        std::string sourceContent = ss.str();
        uLongf sourceContentLen = sourceContent.size();

        std::string header = "blob " + std::to_string(sourceContentLen) + std::string(1,'\0');
        std::string sourceStr = header + sourceContent;
        uLongf sourceLen = sourceStr.size();

        // sha
        unsigned char sha1_hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char*>(sourceStr.data()), sourceLen, sha1_hash);

        std::stringstream shaStr;
        for (unsigned char i : sha1_hash) {
            shaStr << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
        }

        // 压缩
        uLongf destLen = compressBound(sourceLen);
        std::vector<Bytef> destBuffer(destLen);

        int result = compress(
                destBuffer.data(),
                &destLen,
                reinterpret_cast<const Bytef*>(sourceStr.data()),
                sourceLen
        );
        destBuffer.resize(destLen);

        if (result != Z_OK) return EXIT_FAILURE;

        std::string hashStr = shaStr.str();
        std::string dir = ".git/objects/" + hashStr.substr(0,2);
        std::filesystem::create_directory(dir);
        std::ofstream outFile(dir + "/" + hashStr.substr(2));
        if (!outFile) return EXIT_FAILURE;

        outFile << destBuffer.data();
        std::cout << hashStr;

        return EXIT_SUCCESS;
    }
}


CmdMap cmdMap {
    {"init", init},
    {"cat-file", catFile},
    {"hash-object", hashObject},
};

int switchCmd(Cmd& cmd){
    auto fn = cmdMap.find(cmd.name);
    if(fn != cmdMap.end())
        return fn->second(cmd);
    std::cerr << "Unknown command " << cmd.name << '\n';
    return EXIT_FAILURE;
}

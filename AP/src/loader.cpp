#include "loader.h"
#include <openssl/md5.h>
#include <algorithm>
#include <cstddef>


namespace loader
{
    std::string compute_file_md5(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            std::cout << "[Error]: could not open file" << std::endl;
            return "";
        }

        size_t filesize = file.tellg();
        file.seekg(0);
        //std::cout << "---FilePath: " <<  path << "\t\t-----filesize: " << filesize << std::endl;
        MD5_CTX md5_ctx;
        MD5_Init(&md5_ctx);
        const unsigned long block_size = (1 << 20);
        char *buf = new char[block_size];
        for (size_t i = 0; i < filesize; i += block_size)
        {
            size_t read_size = std::min(block_size, filesize - i);
            file.read(buf, read_size);
            MD5_Update(&md5_ctx, buf, read_size);
        }
        unsigned char md5[MD5_DIGEST_LENGTH];
        MD5_Final(md5, &md5_ctx);
        char md5_str[33];
        for (int i = 0; i < 16; ++i)
        {
            sprintf(md5_str + i * 2, "%02x", md5[i]);
        }
        md5_str[32] = '\0';
        delete[] buf;
        file.close();
        return std::string(md5_str);
    }

    std::string md5_to_entityId(const std::string &md5_str)
    {
        mpz_class result;
        // transfer from hex to decimal
        mpz_set_str(result.get_mpz_t(), md5_str.c_str(), 16);
        mpz_class nonce = utils::rand_big_num(128);
        result = result + nonce;

        mpz_nextprime(result.get_mpz_t(), result.get_mpz_t());
        return result.get_str(16);
    }

    void md5_to_id_test()
    {
        std::string md5_str = "dce0588874fd369ce493ea5bc2a21d99";
        std::string res = md5_to_entityId(md5_str);
        std::cout << "md5_str: " << md5_str << std::endl;
        std::cout << "md5_str_bitInt_safeprime: " << res << std::endl;

        mpz_class decoded_pid;
        mpz_set_str(decoded_pid.get_mpz_t(), res.c_str(), 16);
        std::cout << "decoded_pid: " << decoded_pid.get_str() << std::endl;
        std::cout << "decoded_pid_is_safeprime: "
                  << utils::is_safe_prime(decoded_pid) << std::endl;
    }

    void get_file_list(const char *&path, std::vector<std::string> &fileList)
    {
        DIR *dir;
        struct dirent *ent;
        std::string base_name = std::string(path);

        if ((dir = opendir(path)) == nullptr)
        {
            std::cerr << "[Error]: could not open directory" << std::endl;
            return;
        }

        while ((ent = readdir(dir)) != nullptr)
        {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            {
                continue;
            }
            std::string filename = base_name + std::string(ent->d_name);
            fileList.push_back(filename);  // we don't care about the file type
        }
        closedir(dir);
    }

    void get_md5_list(std::vector<std::string> &fileList,
                      std::vector<std::string> &md5List)
    {
        for (const auto &path : fileList)
        {
            // 这里计算 md5，主要问题在这
            std::string md5_str = loader::compute_file_md5(path);
            // 这里计算 md5
            md5List.push_back(std::string(md5_str));
        }
    }

    void write_json(const char *&path, std::vector<std::string> &fileList,
                    std::vector<std::string> &md5List)
    {
        Json::Value root;
        Json::Value arrayObj;
        for (int i = 0; i < fileList.size(); ++i)
        {
            Json::Value item;
            size_t l_pos = fileList[i].find_last_of('/') + 1;
            // size_t r_pos = fileList[i].find_last_of('.');
            size_t r_pos = fileList[i].length();
            item["file"] = fileList[i].substr(l_pos, r_pos - l_pos);
            item["md5"] = md5List[i];
            item["pid"] = loader::md5_to_entityId(md5List[i]);
            arrayObj.append(item);
        }
        root["packages"] = arrayObj;
        Json::StyledWriter writer;
        std::ofstream file(path);
        if (!file)
        {
            std::cerr << "[Error]: could not open file" << std::endl;
            return;
        }
        file << writer.write(root);
        file.close();
    }
    /*
        bool write_json_single(const std::string &path, const std::string &filename,
                               std::string &md5, std::string &pid)
        {
            Json::Reader reader;
            Json::StyledWriter writer;
            Json::Value root;
            Json::Value item;
            Json::Value item1;
            std::ifstream ifs;
            ifs.open(path);
            item1["file"] = filename;
            item1["md5"] = md5;
            item1["pid"] = pid;

            if (!ifs.good())
            {
                std::ofstream file(path);
                if (!file)
                {
                    std::cerr << "[Error]: could not open file" << std::endl;
                    return false;
                }
                file.write("{}", sizeof("{}"));
                file.close();
            }
            ifs.close();
            ifs.open(path);

            if (reader.parse(ifs, root))
            {
                Json::Value detail = root["packages"];
                const int size = root["packages"].size();
                for (int i = 0; i < size; ++i)
                {
                    item = root["packages"][i];
                    std::string str_md5 = item["md5"].asString();
                    if (str_md5 == md5)
                    {
                        ifs.close();
                        return true;
                    }
                }
                ifs.close();
                root["packages"].append(item1);

                std::ofstream file2(path);
                if (!file2)
                {
                    std::cerr << "[Error]: could not open file" << std::endl;
                    return false;
                }

                file2 << writer.write(root);
                file2.close();
                return true;
            }
            else
            {
                return false;
            }
        }

    */
    void get_pid_from_json(const char *&path, std::vector<mpz_class> &pids)
    {
        std::ifstream ifs(path);
        Json::Reader reader;
        Json::Value root;
        Json::Value item;
        if (!reader.parse(ifs, root, false))
        {
            std::cerr << "parse error" << std::endl;
            return;
        }
        const int size = root["packages"].size();
        for (int i = 0; i < size; ++i)
        {
            item = root["packages"][i];
            std::string pid_str = item["pid"].asString();
            mpz_class pid_val;
            mpz_set_str(pid_val.get_mpz_t(), pid_str.c_str(), 16);
            pids.emplace_back(pid_val);
        }
    }




    bool withdraw_file_json(const std::string &path, const std::string &filepath)
    {
        std::ifstream ifs;
        Json::Value root;
        Json::Value item;
        Json::Reader reader;
        Json::Value del;
        Json::StyledWriter writer;
        ifs.open(path);
        if (reader.parse(ifs, root))
        {
            Json::Value detail = root["packages"];
            const int size = root["packages"].size();
            for (int i = 0 ; i < size; ++i)
            {
                item = root["packages"][i];
                std::string str_filepath = item["filepath"].asString();
                if (str_filepath == filepath)
                {
                    root["packages"].removeIndex(i, &del);
                    break;
                }
            }
            ifs.close();

            std::ofstream file2(path);
            if (!file2)
            {
                std::cerr << "[Error]: could not open file" << std::endl;
                return false;
            }
            file2 << writer.write(root);
            file2.close();
            return true ;

        }
        else
        {
            return false;
        }
    }

    bool write_json_single(const std::string &path, const std::string &filename,
                           const std::string &filepath, std::string &md5, std::string &pid,
                           std::string partial_key1, std::string partial_key2, std::string file_acc_wit)
    {
        Json::Reader reader;
        Json::StyledWriter writer;
        Json::Value root;
        Json::Value item;
        Json::Value item1;
        std::ifstream ifs;
        Json::Value del;
        ifs.open(path);
        item1["file"] = filename;
        item1["filepath"] = filepath;
        item1["md5"] = md5;
        item1["pid"] = pid;
        item1["partial_key1"] = partial_key1;
        item1["partial_key2"] = partial_key2;
        item1["file_acc_wit"] = file_acc_wit;


        if (!ifs.good())
        {
            std::ofstream file(path);
            if (!file)
            {
                std::cerr << "[Error]: could not open file" << std::endl;
                return false;
            }
            file.write("{}", sizeof ("{}"));
            file.close();
        }
        ifs.close();
        ifs.open(path);


        if (reader.parse(ifs, root))
        {
            Json::Value detail = root["packages"];
            const int size = root["packages"].size();
            for (int i = 0 ; i < size; ++i)
            {
                item = root["packages"][i];
                std::string str_filepath = item["filepath"].asString();
                if (str_filepath == filepath)
                {

                    return true;
                }
            }
            ifs.close();
            root["packages"].append(item1);

            std::ofstream file2(path);
            if (!file2)
            {
                std::cerr << "[Error]: could not open file" << std::endl;
                return false;
            }
            file2 << writer.write(root);
            file2.close();
            return true ;
        }
        else
        {
            return false;
        }
    }


    std::vector<file_struct> read_file_json(const std::string &path)
    {
        Json::Reader reader;
        std::ifstream ifs;
        Json::Value root;
        Json::Value item;
        ifs.open(path);

        std::vector<file_struct> vector_file_json ;

        if (reader.parse(ifs, root))
        {
            ifs.close();
            Json::Value detail = root["packages"];
            int size = detail.size();
            for (int i = 0 ; i < size; ++i)
            {
                file_struct read_struct;
                item = root["packages"][i];
                read_struct.file_name = item["file"].asString();
                read_struct.file_path = item["filepath"].asString();
                read_struct.file_pid = item["pid"].asString();
                read_struct.file_md5 = item["md5"].asString();
                read_struct.partial_key1 = item["partial_key1"].asString();
                read_struct.partial_key2 = item["partial_key2"].asString();
                read_struct.file_acc_wit = item["file_acc_wit"].asString();

                vector_file_json.push_back(read_struct);
            }
            return vector_file_json;

        }
        else
        {
            return vector_file_json;
        }
    }

    std::vector<std::string> get_file_partial_key(const std::string &path, std::string filepath)
    {
        Json::Reader reader;
        std::ifstream ifs;
        Json::Value root;
        Json::Value item;
        ifs.open(path);
        std::vector<std::string> partial_key(2) ;
        if (reader.parse(ifs, root))
        {
            ifs.close();
            Json::Value detail = root["packages"];
            int size = detail.size();
            for (int i = 0 ; i < size; ++i)
            {
                item = root["packages"][i];
                if (filepath == item["filepath"].asString())
                {
                    partial_key[0] = item["partial_key1"].asString();
                    partial_key[1] = item["partial_key2"].asString();
                    break;
                }
            }


        }
        return partial_key;

    }

    std::string get_file_pid(const std::string &path, std::string filepath)
    {
        Json::Reader reader;
        std::ifstream ifs;
        Json::Value root;
        Json::Value item;
        ifs.open(path);
        if (reader.parse(ifs, root))
        {
            ifs.close();
            Json::Value detail = root["packages"];
            int size = detail.size();
            for (int i = 0 ; i < size; ++i)
            {
                item = root["packages"][i];
                if (filepath == item["filepath"].asString())
                {

                    return item["pid"].asString();
                }
            }


        }
        return "";
    }


};  // namespace loader


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




    bool withdraw_host_file_json(const std::string &path, const std::string &filepath, const std::string &host_name, std::string port)
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

            const int size = root[host_name][port].size();
            for (int i = 0; i < size; i++)
            {
                item = root[host_name][port][i];
                std::string str_filepath = item[port].asString();

                if (str_filepath == filepath)
                {
                    root[host_name][port].removeIndex(i, &del);
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
            ifs.close();
            return false;
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

    bool write_json_single(const std::string &path, const std::string &filename, const std::string &filepath, std::string &md5, std::string &pid)
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
                vector_file_json.push_back(read_struct);
            }
            return vector_file_json;

        }
        else
        {
            return vector_file_json;
        }
    }

    std::vector<host_file_struct> read_host_file_json(const std::string &path, const std::string &host_name, std::string port)
    {
        Json::Reader reader;
        std::ifstream ifs;
        Json::Value root;
        Json::Value item;
        ifs.open(path);

        std::vector<host_file_struct> vector_host_file_json ;
        if (reader.parse(ifs, root))
        {
            int size =  root[host_name][port].size();
            for (int i = 0 ; i < size; ++i)
            {
                host_file_struct read_struct;
                item = root[host_name][port][i];
                read_struct.file_name = item["file"].asString();
                read_struct.file_path = item["filepath"].asString();
                read_struct.file_md5 = item["md5"].asString();
                read_struct.file_pid = item["pid"].asString();
                read_struct.partial_key1 = item["partial_key1"].asString();
                read_struct.partial_key2 = item["partial_key2"].asString();

                vector_host_file_json.push_back(read_struct);

            }
            return vector_host_file_json;
        }
        else
        {
            return vector_host_file_json;
        }

    }



    bool write_json_host_file(const std::string &path, const std::string &filename, const std::string &filepath, const std::string &hash, const std::string &pid,
                              const std::string &client_ip, std::string port, std::string partial_key1, std::string partial_key2)
    {
        Json::Reader reader;
        Json::StyledWriter writer;
        Json::Value root;
        Json::Value item;
        std::ifstream ifs;
        ifs.open(path);
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
            //判断有没有host_name项
            if (root.isMember(client_ip) && root[client_ip].isMember(port))
            {
                Json::Value detail = root[client_ip][port];
                const int size = detail.size();
                //跳过重复项
                for (int i = 0 ; i < size; ++i)
                {
                    item = detail[i];
                    std::string str_filepath = item["filepath"].asString();
                    if (str_filepath == filepath)
                    {

                        return true;
                    }
                }
                ifs.close();


                Json::Value item1;
                item1["file"] = filename;
                item1["filepath"] = filepath;
                item1["md5"] = hash;
                item1["pid"] = pid;
                item1["partial_key1"] = partial_key1;
                item1["partial_key2"] = partial_key2;
                //覆盖写入文件
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
                Json::Value item2;
                item2["file"] = filename;
                item2["filepath"] = filepath;
                item2["md5"] = hash;
                item2["pid"] = pid;
                item2["partial_key1"] = partial_key1;
                item2["partial_key2"] = partial_key2;
                root[client_ip][port].append(item2);

                //覆盖写入文件
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
        }
        else
        {
            return false;
        }

    }

    bool write_json_accumulator_kgc(const std::string &path, const std::string &acc_public_key, const std::string &acc_serect_part_one,
                                    const std::string &acc_serect_part_two, const std::string &acc_init, const std::string &kgc_s, const std::string &kgc_q, const int kgc_k)
    {
        Json::Reader reader;
        Json::StyledWriter writer;
        Json::Value root;
        Json::Value acc;
        Json::Value kgc;
        std::ifstream ifs;
        ifs.open(path);
        acc["acc_public_key"] = acc_public_key;
        acc["acc_serect_part_one"] = acc_serect_part_one;
        acc["acc_serect_part_two"] = acc_serect_part_two;
        acc["acc_init"] = acc_init;
        kgc["kgc_s"] = kgc_s;
        kgc["kgc_q"] = kgc_q;
        kgc["kgc_k"] = kgc_k;

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
            root["accumulator"] = acc;
            root["kgc"] = kgc;
            std::ofstream file(path);
            if (!file)
            {
                std::cerr << "[Error]: could not open file" << std::endl;
                return false;
            }
            file << writer.write(root);
            file.close();
            return true;
        }
        else
        {
            return false;
        }


    }

    acc_kgc_struct *read_acc_kgc_json(const std::string &path)
    {
        Json::Reader reader;
        std::ifstream ifs;
        Json::Value root;
        ifs.open(path);
        acc_kgc_struct *this_struct = new acc_kgc_struct;
        if (reader.parse(ifs, root))
        {
            this_struct->acc_init = root["accumulator"]["acc_init"].asString();
            this_struct->acc_public_key = root["accumulator"]["acc_public_key"].asString();
            this_struct->acc_serect_part_one = root["accumulator"]["acc_serect_part_one"].asString();
            this_struct->acc_serect_part_two = root["accumulator"]["acc_serect_part_two"].asString();
            this_struct->kgc_s = root["kgc"]["kgc_s"].asString();
            this_struct->kgc_q = root["kgc"]["kgc_q"].asString();
            this_struct->kgc_k = root["kgc"]["kgc_k"].asInt();
            return this_struct;
        }
        else
        {
            return nullptr;
        }
    }

    // namespace loader
}

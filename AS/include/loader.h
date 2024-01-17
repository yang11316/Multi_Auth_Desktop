#ifndef LOADER_H
#define LOADER_H

#pragma once
#include <dirent.h>  //only available in linux
#include <jsoncpp/json/json.h>
#include <openssl/md5.h>

#include <fstream>
#include <iostream>
#include <string>

#include "utils.h"




struct acc_kgc_struct
{
    std::string acc_init = "";
    std::string acc_public_key = "";
    std::string acc_serect_part_one = "";
    std::string acc_serect_part_two = "";
    std::string kgc_s = "";
    std::string kgc_q = "";
    int kgc_k = 0;
};

struct host_file_struct
{
    std::string file_name = "";
    std::string file_path = "";
    std::string file_md5 = "";
    std::string file_pid = "";
    std::string partial_key1 = "";
    std::string partial_key2 = "";

    bool operator == (host_file_struct &e)
    {
        return (this->file_path == e.file_path);
    };
};


struct file_struct
{
    std::string file_name = "";
    std::string file_path = "";
    std::string file_md5 = "";
    std::string file_pid = "";


    bool operator == (file_struct &e)
    {
        return (this->file_path == e.file_path);
    };
    bool operator == (host_file_struct &e)
    {
        return (this->file_path == e.file_path);
    };
};


struct host_struct
{

    std::string host_name = "";
    std::string host_ip = "";

};




namespace loader
{
    /**
     * @brief compute the md5 checksum of a file
     *
     * @param path
     * @return std::string
     */
    std::string compute_file_md5(const std::string &path);

    /**
     * @brief covert md5 checksum to entityId (PID as proposed in the drafts)
     *  for decode: mpz_class result;
        mpz_set_str(result.get_mpz_t(), pid_str.c_str(), 16);
     * @param md5str
     * @return std::string
     */
    std::string md5_to_entityId(const std::string &md5str);

    /**
     * @brief Get the File List object
     *
     * @param path  the path of the binary files directory
     * @param fileList : store the path of each binary file
     */
    void get_file_list(const char *&path, std::vector<std::string> &fileList);

    /**
     * @brief
     *
     * @param fileList path of each binary file
     * @param md5List  md5 checksum of each binary file
     */
    void get_md5_list(std::vector<std::string> &fileList,
                      std::vector<std::string> &md5List);

    /**
     * @brief write the assets to json file
     *
     * @param path
     * @param fileList
     * @param md5List
     */
    void write_json(const char *&path, std::vector<std::string> &fileList,
                    std::vector<std::string> &md5List);

    /**
     * @brief Get the pid from json object
     *
     * @param path json file path
     * @param pids
     */
    void get_pid_from_json(const char *&path, std::vector<mpz_class> &pids);

    void md5_to_id_test();

    //    client存在文件撤销
    bool withdraw_host_file_json(const std::string &path, const std::string &filepath, const std::string &host_name, std::string port);
    //    filechoose已完成授权文件撤销
    bool withdraw_file_json(const std::string &path, const std::string &filepath);



    //    计算后存储到filechoose的json
    bool write_json_single(const std::string &path, const std::string &filename, const std::string &filepath,
                           std::string &md5, std::string &pid);

    bool write_json_host_file(const std::string &path, const std::string &filename, const std::string &filepath,
                              const std::string &hash, const std::string &pid,  const std::string &client_ip, std::string client_port,
                              std::string partial_key1, std::string partial_key2
                             );


    //从filechoose的json中读取
    std::vector<file_struct> read_file_json(const std::string &path);

    std::vector<host_file_struct> read_host_file_json(const std::string &path, const std::string &host_name, std::string port);

    bool write_json_accumulator_kgc(const std::string &path, const std::string &acc_public_key,
                                    const std::string &acc_serect_part_one, const std::string &acc_serect_part_two,
                                    const std::string &acc_init, const std::string &kgc_s, const std::string &kgc_q, const int kgc_k);

    acc_kgc_struct *read_acc_kgc_json(const std::string &path);

}  // namespace loader
#endif  // LOADER_H

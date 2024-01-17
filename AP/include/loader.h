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
    std::string kgc_p = "";
    std::string kgc_p0 = "";
};

struct host_file_struct
{
    std::string file_name = "";
    std::string file_path = "";
    std::string file_md5 = "";
    std::string file_pid = "";
    std::string partical_key = "";

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
    std::string partial_key1 = "";
    std::string partial_key2 = "";
    std::string file_acc_wit = "";
    std::string calculate = "";

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

    /*
    bool write_json_single(const std::string &path, const std::string &filename,
                         std::string &md5, std::string &pid);
    */
    //    filechoose已完成授权文件撤销
    bool withdraw_file_json(const std::string &path, const std::string &filepath);
    //    计算后存储到filechoose的json
    bool write_json_single(const std::string &path, const std::string &filename, const std::string &filepath,
                           std::string &md5, std::string &pid, std::string partial_key1, std::string partial_key2,
                           std::string file_acc_wit );


    //从filechoose的json中读取
    std::vector<file_struct> read_file_json(const std::string &path);

    std::vector<std::string> get_file_partial_key(const std::string &path, std::string filepath);
    std::string get_file_pid(const std::string &path, std::string filepath);


}  // namespace loader
#endif  // LOADER_H

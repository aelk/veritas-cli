#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <map>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

typedef std::vector<std::string> string_list;
typedef std::pair<std::string, std::string> string_pair;

const int DEFAULT_SEQ_LENGTH = 6;
const int DEFAULT_THRESHOLD = 0;

template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p);
template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src);
bool valid_directory(fs::path target_dir);
string_list get_filenames(fs::path target_dir);
std::map<std::string, string_list> seq_to_filenames(string_list file_names, const int seq_len);
std::map<string_pair, unsigned int> start_comparisons(string_list file_names,
                    std::map<std::string, string_list> seq_map, const int seq_len);
void show_results(std::map<string_pair, unsigned int> commmon_seqs,
                  const unsigned int threshold, const unsigned int seq_len);
void display_help(bool help);
bool valid_args(int argc, char *argv[], unsigned int& seq_len, unsigned int& threshold);
string_list split_str(const std::string file_str);
bool not_alnum(const char c);
const std::string file_to_str(const std::string file_name);
string_list get_sequences(const string_list words, const int seq_len);
const std::string clean_file_name(const std::string file_name);

#endif

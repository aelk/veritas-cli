#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>
#include "utils.hpp"

// source: https://stackoverflow.com/questions/5056645
template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                   flip_pair<A,B>);
    return dst;
}

bool valid_directory(fs::path target_dir)
{
    if (exists(target_dir)) {
        if (is_regular_file(target_dir)) {
            std::cerr << target_dir << " is a file." << std::endl;
            return false;
        } else if (!is_directory(target_dir)) {
            std::cerr << target_dir << " is not a directory." << std::endl;
            return false;
        }
    } else {
        std::cerr << target_dir << " does not exist." << std::endl;
        return false;
    }
    return true;
}

string_list get_filenames(fs::path target_dir)
{
    string_list file_names;
    fs::directory_iterator it(target_dir), eod;

    BOOST_FOREACH(fs::path const &p, std::make_pair(it, eod)) {
        if (is_regular_file(p)) {
            file_names.push_back(p.string());
        }
    }

    return file_names;
}

std::map<std::string, string_list> seq_to_filenames(string_list file_names, const int seq_len)
{
    std::map<std::string, string_list> seq_map;
    for (auto file_name : file_names) {
        const std::string file_str = file_to_str(file_name);
        string_list seqs = get_sequences(split_str(file_str), seq_len);
        const std::string fname = clean_file_name(file_name);

        for (auto seq : seqs) {
            if (std::find(seq_map[seq].begin(), seq_map[seq].end(), fname)
                    == seq_map[seq].end()) {
                seq_map[seq].push_back(fname);
            }
        }
    }
    return seq_map;
}

std::map<string_pair, unsigned int> start_comparisons(string_list file_names,
                                std::map<std::string, string_list> seq_map, const int seq_len)
{
    std::map<string_pair, unsigned int> common_seqs;
    const size_t NUM_FILES = file_names.size();
    for (size_t i = 0; i != NUM_FILES; ++i) {
        std::cout << "\rProcessing files... " << ceil(100*(static_cast<double>(i)/NUM_FILES)) << "%";
        fflush(stdout);
        const std::string file_str = file_to_str(file_names[i]);
        string_list seqs = get_sequences(split_str(file_str), seq_len);
        const std::string fname = clean_file_name(file_names[i]);

        for (auto seq : seqs) {
            for (auto fname_val : seq_map[seq]) {
                if (fname_val != fname &&
                    common_seqs.count(std::make_pair(fname_val, fname)) == 0)
                {
                    ++common_seqs[std::make_pair(fname, fname_val)];
                }
            }
        }
    }
    std::cout << "\rProcessing files.. 100%" << std::endl;
    return common_seqs;
}

void show_results(std::map<string_pair, unsigned int> commmon_seqs,
                  const unsigned int threshold, const unsigned int seq_len)
{
    auto sorted_map = flip_map(commmon_seqs);
    int num_writes = 0;

    for (auto it = sorted_map.rbegin(); it != sorted_map.rend(); ++it) {
        if (it->first >= threshold) {
            std::cout << "Files: " << (it->second).first << ", " << (it->second).second << std::endl;
            std::cout << "Number of common " << seq_len << "-word sequences: " << it->first << std::endl;
            std::cout << std::endl;
            ++num_writes;
        }
    }

    if (num_writes == 0) {
        std::cout << "No two files had " << threshold << "or more "
			  seq_len << "-word sequences in common." << std::endl;
    }
}

void display_help(bool help)
{
    std::cout << "Usage: ./veritas <path> [optional:] <sequence length> <threshold>" << std::endl;
    if (help) {
    std::cout << "Default for optional parameters: ";
        std::cout << "sequence length = " << DEFAULT_SEQ_LENGTH << " and threshold = " << DEFAULT_THRESHOLD << "." << std::endl;
        std::cout << std::endl << "Example: ./veritas ~/Desktop/some_papers 5 100" << std::endl;
        std::cout << "This displays each pair of papers in the directory ";
        std::cout << "some_papers which have 100 or more 5-word sequences in common." << std::endl;
    }
    std::cout << std::endl << "For help, type: ./veritas help" << std::endl;
}

bool valid_args(int argc, char *argv[], unsigned int& seq_len, unsigned int& threshold)
{
    if (argc < 2) {
        display_help(false);
        return false;
    } else if (argc == 2) {
        std::string arg2(argv[1]);
        std::transform(arg2.begin(), arg2.end(), arg2.begin(), ::tolower);
        if (arg2.compare("help") == 0) {
            display_help(true);
            return false;
        }

        seq_len = DEFAULT_SEQ_LENGTH;
        threshold = DEFAULT_THRESHOLD;
    } else {
        // read sequence length:
        std::istringstream ss (argv[2]);
        if (!(ss >> seq_len) || (seq_len < 1)) {
            std::cerr << argv[2] << " is an invalid sequence length." << std::endl;
            return false;
        }

        // read threshold size if given (otherwise it's 0):
        if (argc == 4) {
            std::istringstream ss (argv[3]);
            if (!(ss >> threshold) || (threshold < 0)) {
                std::cerr << argv[3] << " is an invalid threshold." << std::endl;
                return false;
            }
        } else {
            threshold = DEFAULT_THRESHOLD;
        }
    }
    return true;
}

string_list split_str(const std::string file_str)
{
    string_list words;
    boost::tokenizer<> tok(file_str);
    for (auto it = tok.begin(); it != tok.end(); ++it) {
        words.push_back(static_cast<std::string>(*it));
    }
    return words;
}

bool not_alnum(const char c)
{
    // predicate used in file_to_str function
    // keeps spaces and alphanumeric characters; removes everything else
    if (isspace(c)) {
        return false;
    } else {
        return !std::isalnum(c);
    }
}

const std::string file_to_str(const std::string file_name)
{
    std::ifstream file(file_name);
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string file_str = buffer.str();
    // remove all non-alphanumeric characters from file_str:
    file_str.erase(std::remove_if(file_str.begin(), file_str.end(),
        not_alnum), file_str.end());

    return file_str;
}

string_list get_sequences(const string_list words, const int seq_len)
{
    string_list seqs;
    std::string seq = "";
    size_t start = 0, end = seq_len;
    while (end < words.size() + 1) {
        for (size_t i = start; i < end; ++i) {
            seq += words[i] + " ";
        }
        seq.pop_back();
        seqs.push_back(seq);
        seq = "";
        ++start;
        ++end;
    }
    return seqs;
}

const std::string clean_file_name(const std::string file_name)
{
    boost::filesystem::path p(file_name);
    return p.filename().string();
}

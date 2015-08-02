#include "utils.hpp"

/*
    Compile with
    g++ -Wall -std=c++14 main.cpp -o veritas -lm utils.cpp -lboost_system -lboost_filesystem
*/

int main(int argc, char *argv[])
{
    unsigned int sequence_length, num_seqs_threshold;

    if (!valid_args(argc, argv, sequence_length, num_seqs_threshold)) {
        return 1;
    }

    fs::path target_dir (argv[1]);
    if (!valid_directory(target_dir)) {
        display_help(false);
        return 1;
    }

    auto file_names = get_filenames(target_dir);

    auto seq_map = seq_to_filenames(file_names, sequence_length);

    auto common_seqs = start_comparisons(file_names, seq_map, sequence_length);

    show_results(common_seqs, num_seqs_threshold, sequence_length);

    return 0;
}

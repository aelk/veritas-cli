veritas (command line interface)
================================
Description
------
veritas is a plagiarism detection tool which finds the number of common sequences between every pair of files in a given directory.

Usage
------
1) 'git clone https://github.com/aelk/veritas-cli'

2) Compile using 'g++ -Wall -std=c++14 main.cpp -o veritas -lm utils.cpp -lboost_system -lboost_filesystem'

3) Run './veritas /path/to/directory/'

4) For help, run './veritas help'

Options
-------
veritas accepts two additional parameters: (1) the length of the sequences searched for (e.g. 5), which tells veritas to compare each 5-word sequence in every file in the directory, and (2) a threshold (e.g. 100) which means that veritas will only display files which have 100 or more n-word sequences in common.

The default sequence length searched for is 6, and the default threshold is 1, meaning that only pairs of files with 1 or more 6-word sequences in common will be displayed.

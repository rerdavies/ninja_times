// MIT License
//
// Copyright (c) 2023 Robin Davies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <iostream>
#include "ninja_log.hpp"
#include <iomanip>
#include "CommandLineParser.hpp"


using namespace twoplay;
using namespace std;




int main(int argc, const char**argv)
{
#ifdef ENABLE_GLOBMATCHER_UNIT_TEST
    try {
        GlobMatcherTest();
    } catch (const std::exception &e)
    {
        cout << "Error: Test failed. " << e.what() << endl;
        return EXIT_FAILURE;
    }
#endif
    bool help = false;
    bool error = false;

    bool history = false;
    std::string filename;
    std::string pattern = "*";

    try {
        CommandLineParser parser;
        parser.AddOption("-h",&help);
        parser.AddOption("--help",&help);
        parser.AddOption("--history",&history);
        parser.AddOption("--match",&pattern);


        parser.Parse(argc,argv);

        if (parser.ArgumentCount() == 0)
        {
            help = true;
        } else if (parser.ArgumentCount() == 1)
        {
            filename = parser.Argument(0);
        } else {
            throw std::logic_error("Incorrect number of arguments.");
        }
    } catch (const std::exception& e)
    {
        cout << "Error: " << e.what() << endl;
        cout << endl;
        error = true;
    }

    if (help || error)
    {   cout << "ninja_times: Anaylyzes .ninja_log files for per-file build times." << endl;
        cout << "Copyright (c) 2023 Robin Davies." << endl;
        cout << endl;
        cout << "Syntax: ninja_times filename [options]" << endl;
        cout << "   filename: path of a .ninja_log file." << endl;
        cout << "Options:" << endl;
        cout << "   -h, --help Display this message.:" << endl;
        cout << "   --history  Display history of file build times." << endl;
        cout << "   --match [pattern]" << endl;
        cout << "              A glob pattern that selects which files will be displayed." << endl;
        cout << "              ? matches a character. * matches zero or more characters. " << endl;
        cout << "              [abc] matches 'a', 'b' or 'c' [!abc] matches anything but." << endl;
        cout << endl;
        cout << "ninja_times analyzes file build times in .ninja_log files." << endl;
        cout << endl;
        cout << "By default, ninja_time displays the most recent build times for " << endl;
        cout << "all files in the project. If a --match argument is provied, only " << endl;
        cout << "files that match are displayed. " << endl;
        cout << endl;
        cout << "The --history option allows you to display the history of build times " << endl;
        cout << "for one or more files over time." << endl;
        cout << endl;
        cout << "Examples:" << endl;
        cout << "     # display build times for all files in a project." << endl;
        cout << "     ninja_times build/.ninja_log   # display build times for all files." << endl;
        cout << endl;
        cout << "     # display recent build times for the file PiPedalModel.ccp.o" << endl;
        cout << "     ninja_times build/.ninja_log --match PiPedalModel.cpp.o --history" << endl;
        cout << endl;

        return error? EXIT_FAILURE: EXIT_SUCCESS;
    }

    try {
        if (history)
        {
            NinjaHistory history;
            history.load(filename,pattern);

            cout << history;
            cout << endl;

        } else {

            NinjaLog log;
            log.load(filename,pattern);

            for (const auto&file : log.files())
            {
                cout << setw(8) << setprecision(3) << fixed << (file.duration_ms() / 1000.00) << " " << file.file_name() << endl;
            }
        }
    } catch (const std::exception &e)
    {
        cout << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

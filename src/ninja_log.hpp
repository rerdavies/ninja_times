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
#pragma once


#include <vector>
#include <cstdint>
#include <string>
#include <chrono>
#include <iostream>

using ninja_clock_t = std::chrono::system_clock;


class NinjaFile {
public:
    NinjaFile();
    NinjaFile(const std::string &line);

    uint64_t start_time_ms() const;
    uint64_t end_time_ms() const;
    uint64_t duration_ms() const;

    const ninja_clock_t::time_point &time() const;

    const std::string&file_name() const;

    bool operator<(const NinjaFile&other) { return duration_ms() > other.duration_ms(); }


private:

    uint64_t start_time_,end_time_;
    ninja_clock_t::time_point time_;
    std::string filename_;
};

class NinjaLog {
public:
    void load(const std::string&filename,const std::string&pattern);

    const std::vector<NinjaFile> &files() const;

private:
    std::vector<NinjaFile> files_;
};

class NinjaFileHistoryEntry {
public:
    NinjaFileHistoryEntry();
    NinjaFileHistoryEntry(const NinjaFile&file);

    uint64_t start_time_ms() const { return startTime_; }
    uint64_t end_time_ms() const { return endTime_; }
    uint64_t duration_ms() const { return endTime_-startTime_; }
    const ninja_clock_t::time_point &time() const { return time_; }
private:
    uint64_t startTime_;
    uint64_t endTime_;
    ninja_clock_t::time_point time_;

};
class NinjaFileHistory {
public:
    NinjaFileHistory();
    NinjaFileHistory(const std::string& fileName);
    const std::string&filename() const { return filename_; }

    std::vector<NinjaFileHistoryEntry> entries() const  { return entries_; }


    void add_file(const NinjaFile&file);
    void sort();
private:
    std::string filename_;
    std::vector<NinjaFileHistoryEntry> entries_;
};


class NinjaHistory {
public:
    void load(const std::string&filename,const std::string&pattern);

    const std::vector<NinjaFileHistory> &file_histories() const ;
private:
    std::vector<NinjaFileHistory> file_histories_;
};


std::ostream&operator<<(std::ostream&s,const NinjaHistory &history);
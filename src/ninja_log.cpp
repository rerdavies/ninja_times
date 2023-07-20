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
#include "ninja_log.hpp"
#include <stdexcept>
#include "ss.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "GlobMatcher.hpp"
#include <filesystem>
#include <unordered_set>

using namespace std;

struct FileKey {
    std::string name;
    int64_t time;
    bool operator==(const FileKey&other) const { 
        return time == other.time && name == other.name;
    }
};

template<>
struct std::hash<FileKey>
{
    std::size_t operator()(FileKey const& v) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(v.name);
        std::size_t h2 = std::hash<int64_t>{}(v.time);
        return h1 * (h2 *0x010013UL); 
    }
};

void NinjaHistory::load(const std::string&filename, const std::string&pattern)
{
    GlobMatcher matcher { pattern};

    std::string line;

    std::string history = filename + ".history";
    std::vector<NinjaFile> allFiles;

    std::unordered_set<FileKey> existingRecords{1000};

    if (std::filesystem::exists(history))
    {
        ifstream f(history);
        if (!f.is_open())
        {
            throw std::invalid_argument(SS("Can't open file" << history));
        }
        while (std::getline(f,line))
        {
            if (line.length() != 0 && !line.starts_with('#'))
            {
                NinjaFile file{line};
                allFiles.push_back(file);
                FileKey key {
                    file.file_name(),
                    file.time().time_since_epoch().count()
                };
                existingRecords.insert(key);
            }
        }
    }

    bool recordAdded = false;
    {
        ifstream f;
        f.open(filename);
        if (!f.is_open()) 
        {
            throw std::invalid_argument(SS("Can't open file " << filename));
        }
        if (!std::getline(f,line))
        {
            throw std::invalid_argument("Empty log file.");
        }
        {
            if (line != "# ninja log v5")
            {
                if (line.starts_with("# ninja log"))
                {
                    throw std::invalid_argument("Invalid ninja log version. Expecting: '# ninja log v5'");
                } else {
                    throw std::invalid_argument("Not a valid ninja file file.. Expecting: '# ninja log v5'");
                }
            }
        }
        while (std::getline(f,line))
        {
            if (line.length() != 0 && !line.starts_with('#'))
            {
                NinjaFile file{line};
                FileKey key {
                    file.file_name(),
                    file.time().time_since_epoch().count()
                };
                if (!existingRecords.contains(key))
                {
                    existingRecords.insert(key);
                    allFiles.push_back(file);
                    recordAdded = true;
                }
            }
        }
    }
    if (recordAdded)
    {
        std::string tmpFile = history + ".$$$";

        ofstream f(tmpFile);
        if (!f.is_open())
        {
            throw std::invalid_argument(SS("Can't open file" << tmpFile));
        }
        f << "# ninja log v5" << endl;
        for (auto& file : allFiles)
        {
            f << file << endl;
        }
        f.close();

        if (std::filesystem::exists(history))
        {
            std::filesystem::remove(history);
        }
        std::filesystem::rename(tmpFile,history);

    }

 

    std::unordered_map<std::string, NinjaFileHistory> fileMap;
    for (auto&file: allFiles)
    {
        const std::string&name = file.file_name();
        if (matcher.Matches(name))
        {
            if (!fileMap.contains(name))
            {
                fileMap[name] = NinjaFileHistory(file.file_name());
            }
            fileMap[name].add_file(file);
        }
    }

    for (auto &entry : fileMap)
    {
        entry.second.sort();
        this->file_histories_.push_back(entry.second);
    }

    struct
    {
        bool operator()(const NinjaFileHistory &v1, const NinjaFileHistory &v2)
        {
            return v1.filename() < v2.filename();
        }
    } Compare;
    std::sort(this->file_histories_.begin(), this->file_histories_.end(), Compare);
}

void NinjaLog::load(const std::string& filename, const std::string&pattern)
{
    GlobMatcher matcher(pattern);
    fstream f(filename);
    if (!f.is_open())
    {
        throw std::invalid_argument(SS("Can't open file" << filename));
    }
    std::string line;
    std::unordered_map<std::string, NinjaFile> fileMap;
    while (std::getline(f, line))
    {
        if (line.starts_with('#'))
        {
            continue;
        }
        if (line.length() != 0)
        {
            NinjaFile file{line};
            if (matcher.Matches(file.file_name()))
            {
                std::string name = file.file_name();
                fileMap[name] = std::move(file);
            }
        }
    }

    for (const auto &entry : fileMap)
    {
        this->files_.push_back(entry.second);
    }

    struct
    {
        bool operator()(const NinjaFile &v1, const NinjaFile &v2)
        {
            return v1.duration_ms() > v2.duration_ms();
        }
    } Compare;
    std::sort(this->files_.begin(), this->files_.end(), Compare);
}

const std::vector<NinjaFile> &NinjaLog::files() const
{
    return files_;
}

static std::vector<std::string> split(const std::string &line, char separator)
{
    std::vector<std::string> result;
    std::stringstream s(line);
    std::string t;
    while(getline(s,t,separator)) {
        result.push_back(t);
    }
    return result;
}
template<typename T> 
void convert(const std::string&str,T*result)
{
    std::stringstream s(str);
    s >> *result;
    if (s.fail())
    {
        throw std::logic_error("Invalid file format.");
    }

}
NinjaFile::NinjaFile(const std::string &line)
{
    uint64_t iFileTime;
    std::string discard;
    std::vector<std::string> fields = split(line,'\t');
    if (fields.size() < 5) {
        throw std::logic_error("Invalid file format.");
    }
    convert(fields[0],&start_time_);
    convert(fields[1],&end_time_);
    convert(fields[2],&iFileTime);
    filename_ = fields[3];
    extra_ = fields[4];

    ninja_clock_t::duration durationSinceEpoch(iFileTime);
    ninja_clock_t::time_point fileTime{durationSinceEpoch};
    this->time_ = fileTime;
}

uint64_t NinjaFile::start_time_ms() const { return start_time_; }
uint64_t NinjaFile::end_time_ms() const { return end_time_; }
uint64_t NinjaFile::duration_ms() const { return end_time_ - start_time_; }
const std::string &NinjaFile::file_name() const { return filename_; }
const ninja_clock_t::time_point &NinjaFile::time() const
{
    return time_;
}

NinjaFileHistory::NinjaFileHistory()
{

}
NinjaFileHistory::NinjaFileHistory(const std::string &fileName__) : filename_(fileName__)
{
}

NinjaFile::NinjaFile()
    : start_time_(0), end_time_(0)
{
}

const std::vector<NinjaFileHistory> &NinjaHistory::file_histories() const
{
    return file_histories_;
}

void NinjaFileHistory::add_file(const NinjaFile &file)
{
    this->entries_.push_back(NinjaFileHistoryEntry(file));
}
void NinjaFileHistory::sort()
{
    struct
    {
        bool operator()(const NinjaFileHistoryEntry &v1, const NinjaFileHistoryEntry &v2)
        {
            return v1.time() < v2.time();
        }
    } Compare;

    std::sort(this->entries_.begin(), this->entries_.end(), Compare);
}

NinjaFileHistoryEntry::NinjaFileHistoryEntry(const NinjaFile &file)
    : startTime_(file.start_time_ms()),
      endTime_(file.end_time_ms()),
      time_(file.time())

{
}
NinjaFileHistoryEntry::NinjaFileHistoryEntry()
   : startTime_(0),
      endTime_(0)
{
}

std::string timeToString(const ninja_clock_t::time_point &time)
{
    std::stringstream ss;
    auto in_time_t = ninja_clock_t::to_time_t(time);
                ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
    
}
std::ostream&operator<<(std::ostream&os,const NinjaHistory &history)
{
    for (const auto& history: history.file_histories())
    {
        os << history.filename() << endl;
        for (const auto &entry: history.entries())
        {


            os << setw(22) << timeToString(entry.time())
            << setw(8) 
            << setprecision(3) << fixed << (entry.duration_ms() / 1000.00) 
            << endl;
            ;
 
        }
        os << endl;
}
    return os;
}

std::ostream&operator<<(std::ostream&s, const NinjaFile&ninjaFile)
{
    s << ninjaFile.start_time_ms() 
    << '\t' << ninjaFile.end_time_ms()
    << '\t' << ninjaFile.time().time_since_epoch().count()
    << '\t' << ninjaFile.file_name()
    << '\t' << ninjaFile.extra();
    return s;
}

#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
    filestream.close();
  }
  return value;
}

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

//Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memTotal = 0.0;
  float memFree = 0.0;
  string line;
  string key;
  string value;

  //look for memTotal and memFree in file /proc/meminfo
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::remove(line.begin(), line.end(), ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        // Search key memTotal
        if(key == "MemTotal"){
          memTotal = std::stof(value);
        }
        // Search key memFree
        else if (key == "MemFree"){
          memFree = std::stof(value);
          break;
        }
      }
    }
    filestream.close();
  }
  // Total used memory = (memTotal - memFree) / memTotal
  return ((memTotal - memFree)/memTotal);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string wholeTime;
  // look for uptime in file /proc/uptime
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> wholeTime) {
        try {
          // return uptime value
          if(!wholeTime.empty()){
            return std::stol(wholeTime);
          }else{
            return 0;
          } 
        } catch (const std::invalid_argument& arg) {
          return 0;
        }
      }
    }
    filestream.close();
  }
  return 0;
}

long LinuxParser::Jiffies() { return 0; }

long LinuxParser::ActiveJiffies() { return 0; }

long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuValues{};
  string line;
  string key;
  string vuser, vnice, vsystem, vidle, viowait, virq, vsoftirq, vsteal, vguest, vguest_nice;
  // look cpu values in file /proc/stat
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> vuser >> vnice >> vsystem >> vidle >> viowait >> virq >> vsoftirq >> vsteal >> vguest >> vguest_nice) {
        // return cpuValues
        if (key == "cpu") {
          cpuValues.push_back(vuser);
          cpuValues.push_back(vnice);
          cpuValues.push_back(vsystem);
          cpuValues.push_back(vidle);
          cpuValues.push_back(viowait);
          cpuValues.push_back(virq);
          cpuValues.push_back(vsoftirq);
          cpuValues.push_back(vsteal);
          cpuValues.push_back(vguest);
          cpuValues.push_back(vguest_nice);
          return cpuValues;
        }
      }
    }
    filestream.close();
  }
  return {};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  // look for number of processes in file /proc/stat
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if (key == "processes"){
          try {
              return std::stol(value);
          }catch (const std::invalid_argument& arg){
            return 0;
          }
        }
      }
    }
    filestream.close();
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  // look for running processes in file /proc/stat
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if (key == "procs_running"){
          try {
              return std::stol(value);
          }catch (const std::invalid_argument& arg){
            return 0;
          }
        }
      }
    }
    filestream.close();
  }
  return 0;
}

//Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string value = "";
  // look for pid command in file /proc/[pid]/cmdline
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()){
    std::getline(filestream, value);
    filestream.close();
    return value;
  }
  return value;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value = "";
  // look for pid VmSize in file /proc/[pid]/status
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        // VmData then it gives the exact physical memory being used as a part of Physical RAM.
        if(key == "VmData"){ 
          return value;
        }
      }
    }
    filestream.close();
  }
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value = "";
  // look for user ID associated  with a process in file /proc/[pid]/status
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "Uid"){
          return value;
        }
      }
    }
    filestream.close();
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line;
  string key;
  string value = "";
  string other;
  // look for user associated with a process in file /etc/passwd
  std::ifstream filestream(kPasswordPath);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> value >> other >> key){
        if(key == uid){
          return value;
        }
      }
    }
    filestream.close();
  }
  return value;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  // Time the process started after system boot, means in order to get the unit of time it has been running since start
  //susbtract it from the UpTime() of the system.
  long upTimePid = 0;
  string value;
  // look for uptime of a process in file /proc/[pid]/stat
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      for(int i = 1; i <= kStartTime_; i++){
        linestream >> value;
        if(i == kStartTime_){
          try{
            //susbtract it from the UpTime() of the system.
            upTimePid = LinuxParser::UpTime() - std::stol(value) / sysconf(_SC_CLK_TCK);
            return upTimePid;
          } catch(const std::invalid_argument& arg){
            return 0;
          }
        } 
      }
    }
    filestream.close();
  }
  return upTimePid;
}

vector<float> LinuxParser::CpuUtilization(int pid){
  vector<float> cpuValues{};
  string line;
  float time = 0;
  string value;
  // look for cpu values asociated with a process in file /proc/[pid]/stat
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      for(int i = 1; i <= kStartTime_; i++){
        linestream >> value;
        if(i == kUTime_ || i == kSTime_ || i == kCuTime_ || i == kCsTime_ || i == kStartTime_){
          time = std::stof(value)/ sysconf(_SC_CLK_TCK);
          cpuValues.push_back(time);
        }
      }
    }
    filestream.close();
  }
  return cpuValues;
}
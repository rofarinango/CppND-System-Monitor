#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : processId_(pid){
    getCpuUsage();
    getUser();
    getCommand();
    getRam();
    getUpTime();
}

int Process::Pid() { return processId_; }

float Process::CpuUtilization() const { return cpuUsage_; }

string Process::Command() { return command_; }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return upTime_; }

// Calculate Cpu usage of a process
void Process::getCpuUsage(){
    long upTime = LinuxParser::UpTime();
    vector<float> value = LinuxParser::CpuUtilization(Pid());
    if(value.size() == 5){
        float totalTime = value[kUTime_] + value[kSTime_] + value[kCuTime_] + value[kCsTime_];
        float seconds = upTime - value[kStartTime_];
        cpuUsage_ = totalTime / seconds;
    }
    else{
        cpuUsage_ = 0;
    }
}

// Determine user of a pid.
void Process::getUser(){ user_ = LinuxParser::User(Pid());}
// Determine command of a pid
void Process::getCommand(){ command_ = LinuxParser::Command(Pid());}
// determinar usage ram of a pid in MB
void Process::getRam(){
    string value = LinuxParser::Ram(Pid());
    try{
        long MB = std::stol(value)/1024;
        ram_ = std::to_string(MB);
    } catch (const std::invalid_argument& arg){
        ram_ = "0";
    }
}
//Determine uptime of a pid.
void Process::getUpTime(){
    upTime_ = LinuxParser::UpTime(Pid());
}
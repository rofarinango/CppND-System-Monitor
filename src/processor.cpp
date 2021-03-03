#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

Processor::Processor() : prevTotalCpuTime(0.0), prevIdleCpuTime(0.0){};

// Return the aggregate CPU utilization
float Processor::Utilization() {
    std::vector<long> cpuValues = convertToLong(LinuxParser::CpuUtilization());
    float totalCpuTime = cpuValues[LinuxParser::kUser_] + cpuValues[LinuxParser::kNice_] +
                         cpuValues[LinuxParser::kSystem_] + cpuValues[LinuxParser::kIdle_] +
                         cpuValues[LinuxParser::kIOwait_] + cpuValues[LinuxParser::kIRQ_] +
                         cpuValues[LinuxParser::kSoftIRQ_] + cpuValues[LinuxParser::kSteal_];
    float idleCpuTime = cpuValues[LinuxParser::kIdle_] + cpuValues[LinuxParser::kIOwait_];

    //Cpu usage since last update
    float diffIdle = idleCpuTime - prevIdleCpuTime;
    float diffTotal = totalCpuTime - prevTotalCpuTime;
    float diffUsage = (diffTotal - diffIdle) / diffTotal;
    prevIdleCpuTime = idleCpuTime;
    prevTotalCpuTime = totalCpuTime;
    return diffUsage;
}

std::vector<long> Processor::convertToLong(std::vector<std::string> values){
    std::vector<long> convertedValues{};
    for(int i = 0; i < (int)values.size(); i++){
        try{
            convertedValues.push_back(std::stol(values[i]));
        } catch (const std::invalid_argument& arg){
            convertedValues.push_back((long)0);
        }
    }
    return convertedValues;
}
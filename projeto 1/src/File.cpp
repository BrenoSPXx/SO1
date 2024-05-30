#pragma once

#include <iostream>
#include <fstream>
#include <vector>

struct ProcessParams {
    int creation_time;
    int duration;
    int period;
    int deadline;
    int static_priority;
};

class File
{
public:
	File() {
		myfile.open("entrada.txt");
		if (!myfile.is_open()) {
			std::cout << "Erro ao abrir o arquivo!" << std::endl;
		}
	}
	
	std::vector<ProcessParams> read_file() {
		if (!myfile.is_open()) {
			std::cout << "Arquivo não está aberto!" << std::endl;
		}

        std::vector<ProcessParams> process_params;
        while (true) {
            int creation_time, duration, period, deadline, static_priority;
            myfile >> creation_time >> duration >> period >> deadline >> static_priority;
            if (!myfile) break;
            process_params.push_back({creation_time, duration, period, deadline, static_priority});
        }

//		cout << "Quantidade de processos lidos do arquivo: " << process_params.size() << endl;

        return process_params;
	}

private:
	std::ifstream myfile; 
};


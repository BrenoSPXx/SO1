#include <vector>
#include <cstdint>
#include <stdio.h>

#include "read_file.cc"

enum class ProcessState {
    not_created,
    created,
    ready,
    running,
    blocked,
    terminated,
};

class CPUContext {};

class PCB {
public:
    PCB(
        int pid_,
        int creation_time_,
        int start_,
        int end_,
        int duration_,
        int executed_time_,
        int static_priority_,
        ProcessState state_,
        CPUContext* context_
    ) :
        pid(pid_),
        creation_time(creation_time_),
        start(start_),
        end(end_),
        duration(duration_),
        executed_time(executed_time_),
        static_priority(static_priority_),
        state(state_),
        context(context_)
        {}

    int pid;

    int creation_time;
    int start;
    int end;

    int duration;
    int executed_time;

    int static_priority;

    ProcessState state;
    CPUContext* context;
};

class BaseScheduler {
public:
    bool execute(int time, std::vector<PCB>& process_table) {
        // update state before next_process
        for (PCB& pcb : process_table) {
            if (pcb.creation_time == time) {
                pcb.state = ProcessState::ready;
            }
            if (pcb.state == ProcessState::running) {
                if (pcb.executed_time >= pcb.duration) {
                    pcb.state = ProcessState::terminated;
                } else {
                    pcb.state = ProcessState::ready;
                }
            }
        }

        int curr_pid = next_process(process_table);
        if (curr_pid < 0) return false;

        printf("%2d-%2d ", time, time+1);

        for (PCB& pcb : process_table) {
            // update state after next_process
            if (pcb.pid == curr_pid) {
                pcb.state = ProcessState::running;
            }

            char const* text = 0;
            switch (pcb.state) {
            case ProcessState::not_created:
            case ProcessState::terminated: {
                text = "  ";
            } break;
            case ProcessState::running: {
                text = "##";
            } break;
            default: {
                text = "--";
            } break;
            }

            printf(" %s", text);

            if (pcb.state == ProcessState::running) {
                pcb.executed_time++;
            }
        }
        printf("\n");

        time++;

        return true;
    }

    virtual int next_process(std::vector<PCB>& process_table) = 0;
};

class DumbScheduler : public BaseScheduler {
public:
    virtual int next_process(std::vector<PCB>& process_table) override {
        for (PCB& pcb : process_table) {
            if (   pcb.state != ProcessState::not_created 
                && pcb.state != ProcessState::terminated
            ) {
                return pcb.pid;
            }
        }
        return -1;
    }
};

class CPU {
public:
    CPU(CPUContext* context_) : time(0), context(context_) {}

    int time;
    CPUContext* context;

    void advance_time() {
        time++;
    }

    ~CPU() { delete context; }
};

class INE5412Context : public CPUContext {
public:
    uint64_t registers[6];
    uint64_t sp;
    uint64_t pc;
    uint64_t st;
};

class INE5412 : public CPU {
public:
    INE5412() : CPU(new INE5412Context) {}
};

class System {
public:
    System(CPU* cpu_, BaseScheduler* scheduler_) : cpu(cpu_), scheduler(scheduler_) {}

    CPU* cpu;
    BaseScheduler* scheduler;
    std::vector<PCB> process_table;
    int next_pid = 1;

    void add_process(int creation_time, int duration, int static_priority, CPUContext* context) {
        process_table.emplace_back(
            next_pid++,

            creation_time,
            -1,
            -1,

            duration,
            0,

            static_priority,

            ProcessState::not_created,
            context
        );
    }

    void run() {
        printf("tempo ");
        for (PCB& pcb : process_table) {
            printf(" P%d", pcb.pid);
        }
        printf("\n");

        while (true) {
            if (!scheduler->execute(cpu->time, process_table)) {
                break;
            }

            for (PCB& pcb : process_table) {
                if (pcb.state == ProcessState::running) {
                    *cpu->context = *pcb.context;
                    break;
                }
            }

            cpu->advance_time();
        }
    }
};

int main() {
    File f;
    f.read_file();
    vector<ProcessParams*> const& params = f.get_processes_params();

    INE5412 cpu;
    DumbScheduler scheduler;
    std::vector<INE5412Context> contexts;

    System system(&cpu, &scheduler);
    for (ProcessParams* param : params) {
        contexts.push_back(INE5412Context());
        system.add_process(
            param->get_creation_time(),
            param->get_duration(),
            param->get_priority(),
            &contexts.back()
        );
    }
    system.run();
}

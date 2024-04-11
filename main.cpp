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

    PCB(PCB&) = delete;
    PCB& operator=(PCB&) = delete;

    PCB(PCB&& other) {
        pid = other.pid;
        creation_time = other.creation_time;
        start = other.start;
        end = other.end;
        duration = other.duration;
        executed_time = other.executed_time;
        static_priority = other.static_priority;
        state = other.state;
        context = other.context;

        other.context = 0;
    }
    PCB& operator=(PCB&& other) {
        delete context;

        pid = other.pid;
        creation_time = other.creation_time;
        start = other.start;
        end = other.end;
        duration = other.duration;
        executed_time = other.executed_time;
        static_priority = other.static_priority;
        state = other.state;
        context = other.context;

        other.context = 0;

        return *this;
    }

    int pid;

    int creation_time;
    int start;
    int end;

    int duration;
    int executed_time;

    int static_priority;

    ProcessState state;
    CPUContext* context;

    ~PCB() {
        if (context) delete context;
    }
};

class BaseScheduler {
public:
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

    CPU(CPU&) = delete;
    CPU(CPU&&) = delete;
    CPU& operator=(CPU&) = delete;
    CPU& operator=(CPU&&) = delete;

    int time;
    CPUContext* context;

    void advance_time() {
        time++;
    }

    virtual CPUContext* new_context() = 0;

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
    INE5412() : CPU(new_context()) {}

    virtual CPUContext* new_context() override {
        return new INE5412Context;
    }
};

class System {
public:
    System(CPU* cpu_, BaseScheduler* scheduler_) : cpu(cpu_), scheduler(scheduler_) {}

    CPU* cpu;
    BaseScheduler* scheduler;
    std::vector<PCB> process_table;
    int next_pid = 1;

    void add_process(int creation_time, int duration, int static_priority) {
        process_table.emplace_back(
            next_pid++,

            creation_time,
            -1,
            -1,

            duration,
            0,

            static_priority,

            ProcessState::not_created,
            cpu->new_context()
        );
    }

    void run() {
        printf("tempo ");
        for (PCB& pcb : process_table) {
            printf(" P%d", pcb.pid);
        }
        printf("\n");

        // TODO: nao trocar de contexto se mesmo processo executando

        while (true) {
            {
                // update state before next_process
                for (PCB& pcb : process_table) {
                    if (pcb.creation_time == cpu->time) {
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

                int curr_pid = scheduler->next_process(process_table);
                if (curr_pid < 0) break;

                printf("%2d-%2d ", cpu->time, cpu->time+1);

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

    // TODO: usar new + delete em tudo?

    INE5412 cpu;
    DumbScheduler scheduler;
    System system(&cpu, &scheduler);

    for (ProcessParams* param : params) {
        system.add_process(
            param->get_creation_time(),
            param->get_duration(),
            param->get_priority()
        );
    }
    system.run();
}


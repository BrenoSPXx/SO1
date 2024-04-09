#include <vector>
#include <cstdint>
#include <stdio.h>

#include "read_file.cc"

class BaseContext {};

class INE5412Context : public BaseContext {
public:
    uint64_t registers[6];
    uint64_t sp;
    uint64_t pc;
    uint64_t st;
};

enum class ProcessState {
    not_created,
    created,
    ready,
    running,
    blocked,
    terminated,
};

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
        BaseContext* context_
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
    BaseContext* context;
};

class BaseScheduler {
public:
    std::vector<PCB> process_table;
    int next_pid = 1;

    void add_process(int creation_time, int duration, int static_priority, BaseContext* context) {
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

        int time = 0;
        BaseContext* context = 0;
        while (true) {
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

            int curr_pid = next_process();
            if (curr_pid < 0) return;

            printf("%2d-%2d ", time, time+1);

            for (PCB& pcb : process_table) {
                // update state after next_process
                if (pcb.pid == curr_pid) {
                    pcb.state = ProcessState::running;
                    context = pcb.context;
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
        }
    }

    virtual int next_process() = 0;
};

class DumbScheduler : public BaseScheduler {
public:
    virtual int next_process() override {
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

int main() {
    File f;
    f.read_file();
    vector<ProcessParams*> const& params = f.get_processes_params();

    DumbScheduler scheduler;
    for (ProcessParams* param : params) {
        scheduler.add_process(
            param->get_creation_time(),
            param->get_duration(),
            param->get_priority(),
            new INE5412Context
        );
    }
    scheduler.run();
}

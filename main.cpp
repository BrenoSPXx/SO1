#include <vector>
#include <cstdint>
#include <stdio.h>
#include <cstring>
#include <unistd.h>

#undef NDEBUG
#include <cassert>

#include "read_file.cc"

enum class ProcessState {
    created,
    ready,
    running,
    terminated,
};

class CPUContext {
public:
    CPUContext() {}

    CPUContext(CPUContext const&) = delete;
    CPUContext(CPUContext&&) = delete;
    CPUContext& operator=(CPUContext const&) = delete;
    CPUContext& operator=(CPUContext&&) = delete;

    virtual void copy(CPUContext const*) = 0;

    virtual ~CPUContext() {}
};

struct PCB {
    int pid;

    int next_creation_time;
    int duration;
    int period;
    int deadline;
    int static_priority;

    bool first_instance;
    int executed_cycles;
    int executed_instances;

    ProcessState state;
    CPUContext* context;
};

class BaseScheduler {
public:
    virtual int next_process(std::vector<PCB>& process_table) = 0;
};

class DumbScheduler : public BaseScheduler {
public:
    virtual int next_process(std::vector<PCB>& process_table) override {
        for (PCB& pcb : process_table) {
            if (pcb.state == ProcessState::ready || pcb.state == ProcessState::running) {
                return pcb.pid;
            }
        }
        return -1;
    }
};

class CPU {
private:
    int time;

public:
    CPU() : time(0) {}

    CPU(CPU const&) = delete;
    CPU(CPU&&) = delete;
    CPU& operator=(CPU const&) = delete;
    CPU& operator=(CPU&&) = delete;

    virtual void run() {
        sleep(1);
        time++;
    }

    int get_time() { return time; }

    virtual CPUContext* new_context() = 0;
    virtual void set_context(CPUContext* context) = 0;
    virtual CPUContext const* get_context() = 0;
};

class INE5412Context : public CPUContext {
public:
    virtual void copy(CPUContext const* other_) override {
        assert(dynamic_cast<INE5412Context const*>(other_));

        INE5412Context const& other = (INE5412Context const&)*other_;

        memcpy(registers, other.registers, sizeof(registers));
        sp = other.sp;
        pc = other.pc;
        st = other.st;
    }

    virtual ~INE5412Context() override {}

    uint64_t registers[6];
    uint64_t sp;
    uint64_t pc;
    uint64_t st;
};

class INE5412 : public CPU {
private:
    INE5412Context context;

public:
    INE5412() {}

    virtual CPUContext* new_context() override {
        return new INE5412Context;
    }

    virtual void set_context(CPUContext* other_context) override {
        context.copy(other_context);
    }

    virtual CPUContext const* get_context() override {
        return &context;
    }
};

class System {
public:
    System(CPU* cpu_, BaseScheduler* scheduler_) : cpu(cpu_), scheduler(scheduler_) {}

    CPU* cpu;
    BaseScheduler* scheduler;
    std::vector<PCB> process_table;
    int next_pid = 1;

    void add_process(int creation_time, int duration, int period, int deadline, int static_priority) {
        process_table.push_back({
            next_pid++,

            creation_time, duration, period, deadline, static_priority,

            true, 0, 0,

            ProcessState::created,
            cpu->new_context()
        });
    }

    void run() {
        printf("tempo ");
        for (PCB& pcb : process_table) {
            printf(" P%d", pcb.pid);
        }
        printf("\n");

        constexpr int total_executed_instances_per_process = 2;

        PCB* process_running = 0;
        while (true) {
            ////////////////////
            // update processes
            ////////////////////

            for (PCB& pcb : process_table) {
                // It could happen the following state change in a single time:
                //   running->terminated->created->ready
                // That's 3 state changes. For safety, the loop is until 5
                for (int i = 0; i < 5; i++) {
                    int deadline_timepoint = 
                        pcb.next_creation_time
                        - pcb.period
                        + pcb.deadline;

                    if (pcb.state == ProcessState::created) {
                        if (cpu->get_time() >= pcb.next_creation_time) {
                            pcb.state = ProcessState::ready;
                            pcb.next_creation_time += pcb.period;
                        }
                    } else if (
                           pcb.state == ProcessState::ready
                        || pcb.state == ProcessState::running
                    ) {
                        if (pcb.executed_cycles >= pcb.duration) {
                            pcb.executed_instances++;
                            pcb.executed_cycles = 0;
                            pcb.state = ProcessState::terminated;
                        } else if (cpu->get_time() >= deadline_timepoint) {
                            pcb.executed_instances++;
                            pcb.executed_cycles = 0;
                            pcb.state = ProcessState::created;
                        }
                    } else if (pcb.state == ProcessState::terminated) {
                        if (cpu->get_time() >= deadline_timepoint) {
                            pcb.state = ProcessState::created;
                        }
                    }
                }
            }

            ///////////////////////////////////
            // check if all instances were run
            ///////////////////////////////////

            bool ran_all_instances = true;
            for (PCB& pcb : process_table) {
                if (pcb.executed_instances < total_executed_instances_per_process) {
                    ran_all_instances = false;
                    break;
                }
            }

            if (ran_all_instances) {
                for (PCB& pcb : process_table) {
                    pcb.state = ProcessState::terminated;
                }
                return;
            }


            ////////////////////
            // get next process
            ////////////////////

            int curr_pid = scheduler->next_process(process_table);

            ////////////////////
            // change context
            ////////////////////

            if (!process_running || process_running->pid != curr_pid) {
                if (process_running) {
                    if (process_running->state == ProcessState::running) {
                        process_running->state = ProcessState::ready;
                    }
                    process_running->context->copy(cpu->get_context());
                }

                for (PCB& pcb : process_table) {
                    if (pcb.pid == curr_pid) {
                        cpu->set_context(pcb.context);
                        process_running = &pcb;
                        break;
                    }
                }
            }
            if (process_running) process_running->state = ProcessState::running;

            ////////////////////
            // print
            ////////////////////

            printf("%2d-%2d ", cpu->get_time(), cpu->get_time()+1);
            for (PCB& pcb : process_table) {
                char const* text = 0;
                switch (pcb.state) {
                case ProcessState::created:
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
            }
            printf("\n");
            fflush(stdout);

            ////////////////////
            // run cpu
            ////////////////////

            cpu->run();

            // update running process
            if (process_running) process_running->executed_cycles++;
        }
    }

    ~System() {
        for (PCB& pcb : process_table) {
            delete pcb.context;
        }
    }
};

int main() {
    File f;
    vector<ProcessParams> params = f.read_file();

    INE5412 cpu;
    DumbScheduler scheduler;
    System system(&cpu, &scheduler);

    for (ProcessParams& param : params) {
        system.add_process(
            param.creation_time,
            param.duration,
            param.deadline,
            param.period,
            param.static_priority
        );
    }
    system.run();
}


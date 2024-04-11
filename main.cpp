#include <vector>
#include <cstdint>
#include <stdio.h>
#include <cstring>

#undef NDEBUG
#include <cassert>

#include "read_file.cc"

enum class ProcessState {
    not_created,
    created,
    ready,
    running,
    blocked,
    terminated,
};

class CPUContext {
public:
    CPUContext() {}

    CPUContext(CPUContext const&) = delete;
    CPUContext(CPUContext&&) = delete;
    virtual CPUContext& operator=(CPUContext const&) = 0;
    virtual CPUContext& operator=(CPUContext&&) = 0;

    virtual ~CPUContext() {}
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

    PCB(PCB const&) = delete;
    PCB& operator=(PCB const&) = delete;

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
private:
    int time;

public:
    CPU() : time(0) {}

    CPU(CPU const&) = delete;
    CPU(CPU&&) = delete;
    CPU& operator=(CPU const&) = delete;
    CPU& operator=(CPU&&) = delete;

    virtual void run() {
        time++;
    }

    int get_time() { return time; }

    virtual CPUContext* new_context() = 0;
    virtual void set_context(CPUContext* context) = 0;
    virtual CPUContext* get_context() = 0;
};

class INE5412Context : public CPUContext {
private:
    void _copy(CPUContext const& other_) {
        assert(dynamic_cast<INE5412Context const*>(&other_));

        INE5412Context const& other = (INE5412Context const&)other_;

        memcpy(registers, other.registers, sizeof(registers));
        sp = other.sp;
        pc = other.pc;
        st = other.st;
    }

public:
    virtual CPUContext& operator=(CPUContext const& other) override {
        _copy(other);
        return *this;
    }

    virtual CPUContext& operator=(CPUContext&& other) override {
        _copy(other);
        return *this;
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
        context = *other_context;
    }

    virtual CPUContext* get_context() override {
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

        int last_pid = -1;
        while (true) {
            ////////////////////
            // update processes
            ////////////////////

            for (PCB& pcb : process_table) {
                switch (pcb.state) {
                case ProcessState::not_created: {
                    if (pcb.creation_time == cpu->get_time()) {
                        pcb.state = ProcessState::ready;
                    }
                } break;
                case ProcessState::running: {
                    pcb.executed_time++;
                    if (pcb.executed_time >= pcb.duration) {
                        pcb.state = ProcessState::terminated;
                    }
                } break;
                default: {
                } break;
                }
            }

            ////////////////////
            // get next process
            ////////////////////

            int curr_pid = scheduler->next_process(process_table);
            if (curr_pid < 0) break;

            ////////////////////
            // change context
            ////////////////////

            CPUContext* last_context = cpu->get_context();

            if (last_pid != curr_pid) {
                for (PCB& pcb : process_table) {
                    if (pcb.pid == curr_pid) {
                        pcb.state = ProcessState::running;
                        last_pid = curr_pid;
                        cpu->set_context(pcb.context);
                    } else if (pcb.state == ProcessState::running) {
                        pcb.state = ProcessState::ready;
                        *pcb.context = *last_context;
                    }
                }
            }

            ////////////////////
            // print
            ////////////////////

            printf("%2d-%2d ", cpu->get_time(), cpu->get_time()+1);
            for (PCB& pcb : process_table) {
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
            }
            printf("\n");

            ////////////////////
            // run cpu
            ////////////////////

            cpu->run();
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


#include <vector>
#include <cstdint>
#include <stdio.h>
#include <cstring>
#include <unistd.h>

// TODO: delete
#undef NDEBUG
#include <cassert>
#include <algorithm>
#include <climits>

#include "read_file.cc"

enum class ProcessState {
    ready,
    running,
};

class CPUContext {
public:
    CPUContext() {}

    CPUContext(CPUContext const&) = delete;
    CPUContext(CPUContext&&) = delete;
    CPUContext& operator=(CPUContext const&) = delete;
    CPUContext& operator=(CPUContext&&) = delete;

    virtual void set_register(int value) = 0;
    virtual void copy(CPUContext const*) = 0;

    virtual ~CPUContext() {}
};

struct PCB {
    int pid;

    int creation_time;
    int duration;
    int period;
    int deadline;
    int static_priority;

    int executed_cycles;

    ProcessState state;
    CPUContext* context;
};

class Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) = 0;
};

class DumbScheduler : public Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) override {
        int min_pid = -1;
        for (PCB const& pcb : process_table) {
            if (min_pid == -1 || pcb.pid < min_pid) min_pid = pcb.pid;
        }
        return min_pid;
    }
};

class RateMonotonicScheduler : public Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) override {
        int min_period = INT_MAX;
        int min_pid = -1;

        for (PCB const& pcb: process_table) {
            if (pcb.period < min_period) {
                min_period = pcb.period;
                min_pid = pcb.pid;
            }
        }
        return min_pid;
    }
};

class EarliestDeadlineFirstScheduler : public Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) override {
        int min_abs_deadline = INT_MAX;
        int min_pid = -1;

        for (PCB const& pcb : process_table) {
            if (pcb.deadline + pcb.creation_time < min_abs_deadline) {
                min_abs_deadline = pcb.deadline + pcb.creation_time;
                min_pid = pcb.pid;
            }
        }
        return min_pid;
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
    friend class INE5412;

private:
    uint64_t registers[6]{};
    uint64_t sp{};
    uint64_t pc{};
    uint64_t st{};

public:
    INE5412Context() : registers{}, sp{}, pc{}, st{} {}

    virtual void copy(CPUContext const* other_) override {
        assert(dynamic_cast<INE5412Context const*>(other_));

        INE5412Context const& other = (INE5412Context const&)*other_;

        memcpy(registers, other.registers, sizeof(registers));
        sp = other.sp;
        pc = other.pc;
        st = other.st;
    }

    virtual void set_register(int value) override {
        registers[0] = value;
    }

    virtual ~INE5412Context() override {}
};

class INE5412 : public CPU {
private:
    INE5412Context context;

public:
    INE5412() {}

    virtual void run() override {
        context.registers[1]++;
        CPU::run();
    }

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
private:
    struct PeriodicProcess {
        int pid;

        int next_creation_time;
        int duration;
        int period;
        int deadline;
        int static_priority;

        int executed_instances;
        int wait_time;
        int lost_deadlines;
        int turnaround_time;
    };

    CPU* cpu;
    Scheduler* scheduler;
    std::vector<PeriodicProcess> periodic_processes;
    std::vector<PCB> process_table;
    int next_pid = 1;
    int context_changes = 0;

public:
    System(CPU* cpu_, Scheduler* scheduler_) : cpu(cpu_), scheduler(scheduler_) {}

    void add_periodic_process(int creation_time, int duration, int period, int deadline, int static_priority) {
        periodic_processes.push_back({next_pid, creation_time, duration, period, deadline, static_priority, 0, 0, 0, 0});
        next_pid++;
    }

    void run() {
        constexpr int total_executed_instances_per_process = 5;
        printf("Numero de iteracoes: %d\n\n", total_executed_instances_per_process);

        printf("tempo ");
        for (PeriodicProcess& process : periodic_processes) {
            printf(" P%d", process.pid);
        }
        printf("\n");

        while (true) {
            /////////////////////////////
            // stop processes
            /////////////////////////////

            {
                std::vector<PCB> new_process_table;
                for (PCB& pcb : process_table) {
                    bool finished = pcb.executed_cycles >= pcb.duration;
                    bool reached_deadline = cpu->get_time() >= (pcb.creation_time + pcb.deadline);

                    if (!finished && reached_deadline) { 
                        for (PeriodicProcess& process : periodic_processes) {
                            if (process.pid == pcb.pid) {
                                process.lost_deadlines++;
                            }
                        }
                    }

                    if (finished || reached_deadline) {
                        for (PeriodicProcess& process : periodic_processes) {
                            if (process.pid == pcb.pid) {
                                process.turnaround_time = process.turnaround_time + (cpu->get_time() - pcb.creation_time);
                                process.executed_instances++;
                                break;
                            }
                        }
                        delete pcb.context;
                    } else {
                        new_process_table.push_back(pcb);
                    }
                }
                process_table = move(new_process_table);
            }

            /////////////////////////////
            // create periodic processes
            /////////////////////////////

            for (PeriodicProcess& process : periodic_processes) {
                if (cpu->get_time() >= process.next_creation_time) {
                    PCB new_pcb = {
                        process.pid,

                        process.next_creation_time, process.duration, process.period, process.deadline, process.static_priority,

                        0, 

                        ProcessState::ready,
                        cpu->new_context()
                    };

                    new_pcb.context->set_register(new_pcb.pid);

                    process_table.push_back(new_pcb);
                    process.next_creation_time += process.period;
                }
            }

            ///////////////////////////////////
            // check if all instances were run
            ///////////////////////////////////

            bool ran_all_instances = true;
            for (PeriodicProcess& process : periodic_processes) {
                if (process.executed_instances < total_executed_instances_per_process) {
                    ran_all_instances = false;
                    break;
                }
            }

            if (ran_all_instances) {
                break;
            }

            /////////////////////////////
            // get last running process
            /////////////////////////////

            PCB* last_running_process = 0;
            for (PCB& pcb : process_table) {
                if (pcb.state == ProcessState::running) last_running_process = &pcb;
            }

            ////////////////////
            // get next process
            ////////////////////

            int curr_pid = scheduler->next_process(process_table);

            ////////////////////
            // change context
            ////////////////////

            PCB* curr_running_process = last_running_process;
            if (!last_running_process || last_running_process->pid != curr_pid) {
                if (last_running_process) {
                    last_running_process->context->copy(cpu->get_context());

                    last_running_process->state = ProcessState::ready;
                }

                for (PCB& pcb : process_table) {
                    if (pcb.pid == curr_pid) {
                        cpu->set_context(pcb.context);
                        context_changes++;

                        pcb.state = ProcessState::running;
                        curr_running_process = &pcb;

                        break;
                    }
                }
            }

            ////////////////////
            // print
            ////////////////////

            printf("%2d-%2d ", cpu->get_time(), cpu->get_time()+1);
            for (PeriodicProcess& process : periodic_processes) {
                bool found = false;
                ProcessState state;
                for (PCB& pcb : process_table) {
                    if (pcb.pid == process.pid) {
                        state = pcb.state;
                        found = true;
                        break;
                    }
                }

                char const* text = 0;
                if (found) {
                    switch (state) {
                    case ProcessState::running: {
                        text = "##";
                    } break;
                    case ProcessState::ready: {
                        process.wait_time++;
                        text = "--";
                    } break;
                    }
                } else {
                    text = "  ";
                }

                printf(" %s", text);
            }
            printf("\n");
            fflush(stdout);

            ////////////////////
            // run cpu
            ////////////////////

            cpu->run();

            ///////////////////////////
            // update running process
            ///////////////////////////
            if (curr_running_process) curr_running_process->executed_cycles++;
        }

        printf("\n");

        float total_turnaround_time = 0;
        float total_wait_time = 0;
        int total_lost_deadlines = 0;

        for (PeriodicProcess& process : periodic_processes) {
            float process_turnaround_time = float(process.turnaround_time)/float(process.executed_instances);
            float process_wait_time = float(process.wait_time)/float(process.executed_instances);
            total_turnaround_time += process_turnaround_time;
            total_wait_time += process_wait_time;
            total_lost_deadlines += process.lost_deadlines;

            printf("Turnaround time do processo %d: %.2f\n", process.pid, process_turnaround_time);
            printf("Tempo médio de espera do processo %d: %.2f\n", process.pid, process_wait_time);
            printf("Número de deadlines perdidos do processo %d: %d\n\n", process.pid, process.lost_deadlines);
        }
        
        printf("Turnaround time médio: %.2f\n", (total_turnaround_time/periodic_processes.size()));
        printf("Tempo de espera médio entre os processos: %.2f\n", (total_wait_time/periodic_processes.size()));
        printf("Número total de deadlines perdidos: %d\n", total_lost_deadlines);
        printf("Número total de trocas de contexto: %d\n\n", context_changes);
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

    INE5412 rm_cpu;
    RateMonotonicScheduler rm_scheduler;

    INE5412 edf_cpu;
    EarliestDeadlineFirstScheduler edf_scheduler;

    System rm_system(&rm_cpu, &rm_scheduler);
    System edf_system(&edf_cpu, &edf_scheduler);

    for (ProcessParams& param : params) {
        rm_system.add_periodic_process(
            param.creation_time,
            param.duration,
            param.period,
            param.deadline,
            param.static_priority
        );
        edf_system.add_periodic_process(
            param.creation_time,
            param.duration,
            param.period,
            param.deadline,
            param.static_priority
        );
    }
    printf("###########\n");
    printf("RM System\n");
    printf("###########\n");
    rm_system.run();

    printf("\n");

    printf("###########\n");
    printf("EDF System\n");
    printf("###########\n");
    edf_system.run();
}


#define NDEBUG

#include <cstdio>
#include <vector>

#include "File.cpp"
#include "CPU.cpp"
#include "INE5412.cpp"
#include "RateMonotonicScheduler.cpp"
#include "EarliestDeadlineFirstScheduler.cpp"
#include "System.cpp"

int main() {
    File f;
    std::vector<ProcessParams> params = f.read_file();

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


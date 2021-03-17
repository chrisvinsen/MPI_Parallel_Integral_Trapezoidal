#include <stdio.h> 
#include <Windows.h>
#include <mpi.h>

// Function to get low block for specified process
int Low(int process_id, int number_of_processes, long number_of_grids); 
// Function to get high block for specified process
int High(int process_id, int number_of_processes, long number_of_grids); 
// Function to get size of block for specified process
int Size(int process_id, int number_of_processes, long number_of_grids); 

// Function to calculate integral value
double F(double x); 
// Function Integral Trapezoidal
double Trapezoidal(double lower_limit, double upper_limit, long number_of_grids, long first_block, long last_block); 
// Function to get system times
double GetTime(); 

// Main Function Using min max block before edit
int main(int argc, char* argv[]) {
    // Variable Declaration and Initialization

    // Process ID and Total Process
    int process_id, number_of_processes; 
    // Lower and Upper Limit in equation
    double lower_limit = 10; 
    double upper_limit = 50;
    // Total iteraions or grids
    long number_of_grids = 40000000;
    // First and Last Block in each Process
    long first_block, last_block; 
    // Total time used
    double elapsed_time, max_elapsed_time; 
    // Variable to store the result of equation
    double local_result, global_result; 
    
    // Initialize the MPI Execution Environment
    MPI_Init(&argc, &argv); 
    // Blocks until all processes in the communicator have reached this routine
    MPI_Barrier(MPI_COMM_WORLD); 
    // Get the Initial Time
    elapsed_time = -GetTime(); 

    // Determines the rank of the calling process in the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id); 
    // Determines the size of the group associated with a communicator
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes); 

    // Get first and last block of this process
    first_block = Low(process_id, number_of_processes, number_of_grids - 1) + 1; 
    last_block = High(process_id, number_of_processes, number_of_grids - 1) + 1;

    // Get the local result of process from Trapezoidal function
    local_result = Trapezoidal(lower_limit, upper_limit, number_of_grids, first_block, last_block); 

    // Get total values of result on all processes to a single value
    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
    // Blocks until all processes in the communicator have reached this routine
    MPI_Barrier(MPI_COMM_WORLD);
    // Get the Final Time
    elapsed_time += GetTime(); 
    // Get maximum values of elapsed_time on all processes to a single value
    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); 

    //printf("%d) local count = %lf with time = %lf second\n", process_id, local_result, elapsed_time);
    //fflush(stdout);

    if (process_id == 0) {
        // Print the results
        printf("Nilai Integral = %lf \nWaktu (Tp)     = %lf second\n", global_result, max_elapsed_time);
        fflush(stdout);
    }

    // Terminates MPI execution environment
    MPI_Finalize(); 
    return 0;
}

// Function to get low block for specified process
int Low(int process_id, int number_of_processes, long number_of_grids) { 
    return((process_id * number_of_grids) / number_of_processes); 
}
// Function to get high block for specified process
int High(int process_id, int number_of_processes, long number_of_grids) { 
    return((process_id + 1) * number_of_grids / number_of_processes - 1); 
}
// Function to get size of block for specified process
int Size(int process_id, int number_of_processes, long number_of_grids) { 
    return(Low(process_id + 1, number_of_processes, number_of_grids) - Low(process_id, number_of_processes, number_of_grids));
}

// Function to calculate integral value
double F(double x) { 
    double h = 3 * x * x + 4 * x + 12;
    return (h);
}
// Function Integral Trapezoidal
double Trapezoidal(double lower_limit, double upper_limit, long number_of_grids, long first_block, long last_block) {
    double xi, yi;
    double h = (upper_limit - lower_limit) / number_of_grids; // Grid spacing
    double y = 0.5 * h * (F(upper_limit) + F(lower_limit));

    for (int i = first_block; i <= last_block; i++) {
        xi = lower_limit + i * h;
        yi = F(xi);
        y += h * yi;
    }

    return y;
}
// Function to get system times
double GetTime() {
    SYSTEMTIME time;
    GetSystemTime(&time);
    LONG time_ms = (time.wSecond * 1000) + time.wMilliseconds; // msec     
    return(time_ms / 1000.0); // second 
}

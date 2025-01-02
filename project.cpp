#include <iostream>
#include <string>
using namespace std;

const int MAX_INSTRUCTIONS = 100;

int num_instructions;                    // Number of instructions
string operations[MAX_INSTRUCTIONS];     // Operation 
string dest[MAX_INSTRUCTIONS];           // Destination register 
string src1[MAX_INSTRUCTIONS];           // Source register 1 
string src2[MAX_INSTRUCTIONS];           // Source register 2 
int pipeline_stage[MAX_INSTRUCTIONS];    // To represent the current pipeline stage for every instruction
int instruction_counter = 0;             // To track the clock cycle for which instruction should enter fetch stage

int clock_cycle = 0;                     // To store total number of clock cycles
int wasted_cycle = 0;                    // To store the number of wasted clock cycles due to hazards

enum Pipeline_Stage                      // Enum to store stages of pipeline
{
    FETCH,
    DECODE,
    EXECUTE,
    MEMORY,
    WRITEBACK,
    DONE
};

void get_input() 
{
    cout << "Enter the number of instructions: ";
    cin >> num_instructions;

    for (int i = 0; i < num_instructions; i++) 
    {
        cout << "Enter instruction " << i + 1 << " (e.g., ADD R1 R2 R3): ";
        cin >> operations[i] >> dest[i] >> src1[i] >> src2[i];

        pipeline_stage[i] = DONE;  // Set all instructions to DONE initially
    }
    
}

bool check_hazard(int current, int previous)
{
    bool hazard_detected = false;

    // For Read after Write Hazard
    if(src1[current] == dest[previous] || src2[current] == dest[previous] )
    {
        cout << "Read After Write Hazard detected between instruction " <<previous + 1 << " and instruction " << current + 1 << ". Adding a stall.\n";
        hazard_detected = true;
    }

    // For Write After Read Hazard
    if( src1[previous] == dest[current] || src2[previous] == dest[current] )
    {
        cout << "Write After Read Hazard detected between Instruction " << previous + 1 << " and Instruction " << current + 1 << ". Adding a stall.\n";
        hazard_detected = true;
    }

    // For Write After Write Hazard
    if( dest[previous] == dest[current] )
    {
        cout << "WAW Hazard detected between Instruction " << previous + 1 << " and Instruction " << current + 1 << ". Adding a stall.\n";
        hazard_detected = true;
    }

    return hazard_detected;
}

void pipline()
{
    int completed_instructions = 0;

    cout << "Starting to push instructions into the Pipeline: \n";

    while(completed_instructions < num_instructions)
    {
        cout << "\nClock Cycle: " << clock_cycle << "\n";  // To print the state of pipeline for this clock cycle

        // Process each instruction (one instruction moves forward per clock cycle)
        for(int i = 0; i < num_instructions; i++)
        {
            // Skip instructions that are DONE
            if(pipeline_stage[i] == DONE)
                continue;

            // Print the instruction state
            if(pipeline_stage[i] != DONE) {
                cout << "Instruction " << i + 1 << " (" << operations[i] << "): ";
                switch (pipeline_stage[i])
                {
                    case FETCH:
                        cout << "FETCH";
                        break;
                    case DECODE:
                        cout << "DECODE";
                        break;
                    case EXECUTE:
                        cout << "EXECUTE";
                        break;
                    case MEMORY:
                        cout << "MEMORY";
                        break;
                    case WRITEBACK:
                        cout << "WRITEBACK";
                        break;
                }
                cout << "\n";
            }

            // Check for hazards with the previous instruction
            if (i > 0) 
            {
                while (pipeline_stage[i - 1] != DONE && check_hazard(i, i - 1))
                {
                    wasted_cycle++; // Increase wasted cycle count due to hazard
                    cout << "Stalling Instruction " << i + 1 << " due to dependency on Instruction " << i << ".\n";
                    i++;
                }
            }


            // Move instruction to the next stage if not DONE
            if(pipeline_stage[i] == FETCH) 
            {
                pipeline_stage[i] = DECODE;  // First instruction starts at FETCH and moves to DECODE
            }
            else if(pipeline_stage[i] == DECODE) 
            {
                pipeline_stage[i] = EXECUTE;
            }
            else if(pipeline_stage[i] == EXECUTE) 
            {
                pipeline_stage[i] = MEMORY;
            }
            else if(pipeline_stage[i] == MEMORY) 
            {
                pipeline_stage[i] = WRITEBACK;
            }
            else if(pipeline_stage[i] == WRITEBACK) 
            {
                pipeline_stage[i] = DONE;  // After Writeback, instruction is completed
                completed_instructions++;  // Increment completed instruction count
            }
        }

        // Manage which instruction should enter the FETCH stage
        if(instruction_counter < num_instructions) 
        {
            pipeline_stage[instruction_counter] = FETCH;
            instruction_counter++;  // Move the counter to the next instruction
        }

        clock_cycle++;  // Increment clock cycle after processing all instructions
    }
}

void final_performance_analysis()
{
    cout << "\nFinal Performance Analysis:\n";
    cout << "Total Clock Cycles: " << clock_cycle - 1 << "\n";
    cout << "Total Wasted Clock Cycles: " << wasted_cycle << "\n";
    cout << "Total Instructions: " << num_instructions << "\n";
    cout << "Cycles Per Instruction (CPI): " << static_cast<float>(clock_cycle - 1) / num_instructions << "\n";
    cout << "Average CPI (including stalls): " << static_cast<float>((clock_cycle - 1) + wasted_cycle) / num_instructions << "\n";
}

int main()
{
    get_input();

    pipline();

    final_performance_analysis();

    return 0;
}

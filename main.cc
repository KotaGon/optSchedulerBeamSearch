
#include "utils.h"
#include "text_reader.h"
#include "input.h"
#include "beam_search.h"

int main()
{
    input input_data;
    input_data.read();
    solverClass solver(input_data);
    for(int i = 0; i < 1;  ++i)
    {
        solver.optimize();
        solver.output();
    }
    return 0;
}

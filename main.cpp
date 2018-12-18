#include "limitgraph.h"
#include "limitsequencegraph.h"
#include "simplegraph.h"
#include "simplesequencegraph.h"

int main(int argc, char *argv[])
{
//    SimpleGraph simple;
//    simple.exec();

//    SimpleSequenceGraph simpleSequence;
//    simpleSequence.exec();

//    LimitGraph limitGraph;
//    limitGraph.exec();

    const size_t maxParallelJobs = 12;
    LimitSequenceGraph limitSequenceGraph(maxParallelJobs);
    limitSequenceGraph.exec();

    return 0;
}

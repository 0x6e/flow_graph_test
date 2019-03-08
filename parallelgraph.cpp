#include "parallelgraph.h"

#include <iostream>

ParallelGraph::ParallelGraph(size_t maxParallel /*= 8*/)
    : m_maxParallel(maxParallel)
    , m_graph(new graph)
{
    // Just waste some cpu cycles and memory - simulate decompressing columns
    auto f = [] (Message msg) -> Message {
        std::cout << "Process node " << msg.id << std::endl;

        const size_t size = 20000000;
        msg.data = new float[size];
        for (auto i = 0U; i < size; ++i) {
            msg.data[i] = static_cast<float>(msg.id);
            msg.data[i]++;
            msg.data[i]--;
        }

        // Imagine some work was done here with decompressed data

        // Cleanup
        delete[] msg.data;
        msg.data = nullptr;

        return msg;
    };

    // Do some work on maxParallel threads at once
    m_computeNode.reset(new function_node<Message>(*m_graph, m_maxParallel, f));

    // Limit the number of parallel computations
    m_limit.reset(new limiter_node<Message>(*m_graph, m_maxParallel));

    // Enforce the correct order, based on the message id
    m_ordering.reset(new sequencer_node<Message>(*m_graph, [] (const Message& msg) -> unsigned int {
        return msg.id;
    }));

    // Set up the graph topology:
    //
    // orderingNode -> limitNode -> decompressionAndSamplingNode (parallel)
    //                      ^                   |
    //                      |___________________|
    //
    // Run the decompressionAndSampling node in the correct order, but do not wait for the most
    // up-to-date data.
    make_edge(*m_ordering, *m_limit);
    make_edge(*m_limit, *m_computeNode);

    // Feedback that we can now decompress another column
    make_edge(*m_computeNode, m_limit->decrement);
}

void ParallelGraph::exec()
{
    // Push some messages into the top of the graph to be processed - representing the column indices
    for (unsigned int i = 0; i < 1000; ++i) {
        Message msg = { i, nullptr };
        m_ordering->try_put(msg);
    }

    // Wait for the graph to complete
    m_graph->wait_for_all();
}

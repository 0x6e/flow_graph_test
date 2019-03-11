#include "parallelgraph.h"

#include <iostream>
#include <mutex>

ParallelGraph::ParallelGraph(size_t maxParallel /*= 8*/)
    : m_maxParallel(maxParallel)
    , m_graph(new graph)
{
    auto f = [this] (Message msg) -> Message {
        // Just waste some cpu cycles and memory - simulate decompressing columns
        const size_t size = 20000000;
        msg.data.reset(new float[size]);
        for (auto i = 0U; i < size; ++i) {
            msg.data[i] = static_cast<float>(msg.id);
            msg.data[i]++;
            msg.data[i]--;
        }

        // Imagine some work was done here with decompressed data
        msg.beta = processAsync(msg.id, msg.data);

        return msg;
    };

    // Do some work on maxParallel threads at once
    m_computeNode.reset(new function_node<Message, Message>(*m_graph, m_maxParallel, f));

    // Decide whether to continue calculations or discard
    auto g = [] (decision_node::input_type input,
                 decision_node::output_ports_type &outputPorts) {

        std::get<0>(outputPorts).try_put(continue_msg());

        if (input.beta < -2) {
            // Do global computation
            std::get<1>(outputPorts).try_put(std::move(input));
        } else {
            // Discard
            std::get<0>(outputPorts).try_put(continue_msg());
        }
    };

    m_decisionNode.reset(new decision_node(*m_graph, m_maxParallel, g));

    // Do global computation
    auto h = [] (Message msg) -> continue_msg {

        std::cout << "Global computation " << msg.id
                  << "; beta: " << msg.beta << std::endl;

        // Just waste some cpu cycles and memory - simulate calculations
        const size_t size = 20000000;
        for (auto i = 0U; i < size; ++i) {
            msg.data[i] = static_cast<float>(msg.id);
            msg.data[i]++;
            msg.data[i]--;
        }

        return continue_msg();
    };
    // Use the serial policy
    m_globalComputeNode.reset(new function_node<Message>(*m_graph, serial, h));

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
    //                      |___discard____decisionNode (parallel)
    //                      ^                   |
    //                      |                   | keep
    //                      |                   |
    //                      |______________globalCompute (serial)
    //
    // Run the decompressionAndSampling node in the correct order, but do not
    // wait for the most up-to-date data.
    make_edge(*m_ordering, *m_limit);
    make_edge(*m_limit, *m_computeNode);

    // Feedback that we can now decompress another column, OR
    make_edge(*m_computeNode, *m_decisionNode);
    make_edge(output_port<0>(*m_decisionNode), m_limit->decrement);
    // Do the global computation
    make_edge(output_port<1>(*m_decisionNode), *m_globalComputeNode);

    // Feedback that we can now decompress another column
    make_edge(*m_globalComputeNode, m_limit->decrement);
}

void ParallelGraph::exec()
{
    // Push some messages into the top of the graph to be processed - representing the column indices
    for (unsigned int i = 0; i < 1000; ++i) {
        Message msg { i };
        m_ordering->try_put(msg);
    }

    // Wait for the graph to complete
    m_graph->wait_for_all();
}

double ParallelGraph::processAsync(const size_t id, const DataPtr &data)
{
    double beta;
    {
        // Simulate getting shared data
        std::shared_lock lock(m_mutex);
        beta = 0.0;
    }

    // Just waste some cpu cycles and memory - simulate calculations
    const size_t size = 20000000;
    for (auto i = 0U; i < size; ++i) {
        data[i] = static_cast<float>(id);
        data[i]++;
        data[i]--;
    }

    {
        // Simulate writing shared data
        std::unique_lock lock(m_mutex);
        beta = m_distribution(m_generator);
    }

    return beta;
}

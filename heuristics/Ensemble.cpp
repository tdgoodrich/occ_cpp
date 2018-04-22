#include "Ensemble.hpp"


/**
 * Run heuristics until reaching timeout. Then return the best result found
 * for each heuristic. Best result is reported as the largest bipartite
 * subgraph found by any solver. Also returns the corresponding OCT set.
 *
 * @param  timeout  Timeout in milliseconds.
 * @return          Tuple consisting of (best, oct, seconds).
 */
tuple<vector<int>, vector<int>, long> EnsembleSolver::heuristic_solve(
    Graph &graph, volatile sig_atomic_t &interrupt, long timeout, int seed)
{

    // List of solvers
    vector<vector<int> (*)(Graph &, std::default_random_engine &)> solvers = {
        greedy_bipartite,
        greedy_stochastic,
        greedy_dfs_bipartite,
        greedy_bfs_bipartite
    };

    // Starting index
    int idx = 0;

    // Initialize results
    vector<int> best;
    long totalTime;

    // Create a mercenne twster rand generator
    std::default_random_engine gen(seed);

    // Time
    const auto start = Clock::now();

    // Run heuristics while duration is less than timeout and not interrupted.
    // Ensure that at least one iteration has been run.
    do {

        // Compute greedy bipartite
        auto result = solvers[idx](graph, gen);

        // Record results, and get the current time
        if (result.size() > best.size()) {
            best = result;
            totalTime = chrono::duration_cast<chrono::milliseconds>(Clock::now() - start).count();
        }

        // Increment solver index
        idx = (idx + 1) % solvers.size();

    } while (!interrupt &&
             chrono::duration_cast<chrono::milliseconds>(Clock::now() - start).count() < timeout);

    // OCT results
    vector<int> oct, range;

    // Create vector of all vertices
    range.resize(graph.get_num_vertices());
    iota(range.begin(), range.end(), 0);

    // Sort best solutions
    sort(best.begin(), best.end());

    // Compute OCT
    set_difference(
        range.begin(), range.end(),
        best.begin(), best.end(),
        inserter(oct, oct.begin())
    );

    // Return dictionary of results
    return make_tuple(best, oct, totalTime);

}

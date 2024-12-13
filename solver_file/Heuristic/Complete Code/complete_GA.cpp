#include <bits/stdc++.h>
using namespace std;

// Structures
struct Item {
    int id;
    int w, l;
};

struct Truck {
    int id;
    int W, L, c;
};

struct Placement {
    int x;
    int y;
    bool o; // Orientation: 0 - original, 1 - rotated
};

struct Solution {
    vector<int> t; // Truck assignments for each item
    vector<Placement> placement; // Placement details for each item
    int totalCost;
    bool feasible;
};

// Function to get the dimensions based on orientation
pair<int, int> getDimensions(const Item &item, bool o) {
    if (!o) return {item.w, item.l};
    else return {item.l, item.w};
}

// Function to check overlap between two rectangles
bool isOverlap(int x1, int y1, int w1, int l1,
              int x2, int y2, int w2, int l2) {
    return !(x1 + w1 <= x2 || x2 + w2 <= x1 ||
             y1 + l1 <= y2 || y2 + l2 <= y1);
}

// Function to check feasibility of a solution
bool checkFeasibility(const Solution &sol, const vector<Item> &items, const vector<Truck> &trucks) {
    // Group items by trucks
    int K = trucks.size();
    vector<vector<int>> trucksItems(K, vector<int>());
    for(int i = 0; i < sol.t.size(); i++) {
        int truckIdx = sol.t[i];
        if(truckIdx >= 0 && truckIdx < K)
            trucksItems[truckIdx].push_back(i);
        else
            return false; // Invalid truck assignment
    }

    // Check for each truck
    for(int k = 0; k < K; k++) {
        // Get truck dimensions
        int truckW = trucks[k].W;
        int truckL = trucks[k].L;

        // Get items in this truck
        vector<int> &itemsInTruck = trucksItems[k];

        // Check each item is within truck boundaries
        for(auto &i : itemsInTruck) {
            auto dims = getDimensions(items[i], sol.placement[i].o);
            int w = dims.first;
            int l = dims.second;
            int x = sol.placement[i].x;
            int y = sol.placement[i].y;
            if(x < 0 || y < 0 || x + w > truckW || y + l > truckL)
                return false; // Item out of truck bounds
        }

        // Check for overlapping items
        // Sort items by x-coordinate
        sort(itemsInTruck.begin(), itemsInTruck.end(), [&](const int a, const int b) -> bool {
            return sol.placement[a].x < sol.placement[b].x;
        });

        for(int i = 0; i < itemsInTruck.size(); i++) {
            int idx1 = itemsInTruck[i];
            auto dims1 = getDimensions(items[idx1], sol.placement[idx1].o);
            int w1 = dims1.first;
            int l1 = dims1.second;
            int x1 = sol.placement[idx1].x;
            int y1 = sol.placement[idx1].y;

            for(int j = i + 1; j < itemsInTruck.size(); j++) {
                int idx2 = itemsInTruck[j];
                auto dims2 = getDimensions(items[idx2], sol.placement[idx2].o);
                int w2 = dims2.first;
                int l2 = dims2.second;
                int x2 = sol.placement[idx2].x;
                int y2 = sol.placement[idx2].y;

                // Since sorted by x, no need to check further if no overlap on x
                if(x2 >= x1 + w1)
                    break;

                if(isOverlap(x1, y1, w1, l1, x2, y2, w2, l2))
                    return false; // Overlapping items
            }
        }
    }

    return true;
}

// Function to calculate total cost
int calculateCost(const Solution &sol, const vector<Truck> &trucks, bool &feasible, const vector<Item> &items) {
    // Sum the cost of used trucks
    vector<bool> used(trucks.size(), false);
    for(auto t : sol.t) {
        if(t >= 0 && t < trucks.size())
            used[t] = true;
        else {
            feasible = false; // Invalid truck assignment
            return INT32_MAX;
        }
    }
    int cost = 0;
    for(int k = 0; k < trucks.size(); k++) if(used[k]) cost += trucks[k].c;
    
    // Check feasibility
    feasible = checkFeasibility(sol, items, trucks);
    if(!feasible)
        return INT32_MAX; // Assign a high penalty for infeasible solutions
    return cost;
}

int main(){
    srand(time(0));
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    // Input reading
    int N, K;
    cin >> N >> K;
    vector<Item> items(N);
    for(int i=0;i<N;i++){
        items[i].id = i+1;
        cin >> items[i].w >> items[i].l;
    }
    vector<Truck> trucks(K);
    for(int k=0;k<K;k++){
        cin >> trucks[k].W >> trucks[k].L >> trucks[k].c;
        trucks[k].id = k+1;
    }

    // GA parameters
    int populationSize = 50;
    int generations = 100;
    double crossoverRate = 0.8;
    double mutationRate = 0.1;

    // Initialize population
    vector<Solution> population(populationSize, Solution());
    for(auto &sol : population){
        sol.t.resize(N);
        sol.placement.resize(N);
        // Random assignment
        for(int i=0;i<N;i++){
            sol.t[i] = rand() % K;
            auto dims = getDimensions(items[i], false);
            sol.placement[i].x = rand() % max(1, trucks[sol.t[i]].W - dims.first + 1);
            sol.placement[i].y = rand() % max(1, trucks[sol.t[i]].L - dims.second + 1);
            sol.placement[i].o = rand() % 2;
        }
        // Calculate total cost and feasibility
        bool feasible;
        sol.totalCost = calculateCost(sol, trucks, feasible, items);
        sol.feasible = feasible;
    }

    // GA Main Loop
    for(int gen=0; gen < generations; gen++){
        // Sort population based on totalCost
        sort(population.begin(), population.end(), [&](const Solution &a, const Solution &b) -> bool{
            return a.totalCost < b.totalCost;
        });

        // Elitism: carry forward the top 10% solutions
        int elitismCount = populationSize * 0.1;
        vector<Solution> newPopulation;
        for(int i=0; i < elitismCount; i++) {
            newPopulation.push_back(population[i]);
        }

        // Generate rest of the new population
        while(newPopulation.size() < populationSize){
            // Tournament Selection
            auto tournament = [&](const vector<Solution> &pop) -> Solution {
                int tournamentSize = 3;
                Solution best = pop[rand() % pop.size()];
                for(int i=1; i < tournamentSize; i++) {
                    Solution contender = pop[rand() % pop.size()];
                    if(contender.totalCost < best.totalCost)
                        best = contender;
                }
                return best;
            };

            Solution parent1 = tournament(population);
            Solution parent2 = tournament(population);

            // Crossover
            if(((double)rand()/RAND_MAX) < crossoverRate){
                // Uniform crossover
                Solution child1, child2;
                child1.t.resize(N);
                child1.placement.resize(N);
                child2.t.resize(N);
                child2.placement.resize(N);
                for(int i=0;i<N;i++){
                    if(rand() % 2){
                        child1.t[i] = parent1.t[i];
                        child1.placement[i] = parent1.placement[i];
                        child2.t[i] = parent2.t[i];
                        child2.placement[i] = parent2.placement[i];
                    }
                    else{
                        child1.t[i] = parent2.t[i];
                        child1.placement[i] = parent2.placement[i];
                        child2.t[i] = parent1.t[i];
                        child2.placement[i] = parent1.placement[i];
                    }
                }
                newPopulation.push_back(child1);
                if(newPopulation.size() < populationSize)
                    newPopulation.push_back(child2);
            }
            else{
                newPopulation.push_back(parent1);
                if(newPopulation.size() < populationSize)
                    newPopulation.push_back(parent2);
            }
        }

        // Mutation
        for(auto &sol : newPopulation){
            if(((double)rand()/RAND_MAX) < mutationRate){
                // Mutate a random item's truck assignment and placement
                int mutateIdx = rand() % N;
                sol.t[mutateIdx] = rand() % K;
                auto dims = getDimensions(items[mutateIdx], false);
                sol.placement[mutateIdx].x = rand() % max(1, trucks[sol.t[mutateIdx]].W - dims.first + 1);
                sol.placement[mutateIdx].y = rand() % max(1, trucks[sol.t[mutateIdx]].L - dims.second + 1);
                sol.placement[mutateIdx].o = rand() % 2;
            }
        }

        // Evaluate total cost and feasibility
        for(auto &sol : newPopulation){
            bool feasible;
            sol.totalCost = calculateCost(sol, trucks, feasible, items);
            sol.feasible = feasible;
        }

        // Replace population
        population = newPopulation;

        // Optional: Print generation info
        // cout << "Generation " << gen+1 << ": Best Cost = " << population[0].totalCost << "\n";
    }

    // Select best feasible solution
    Solution best;
    best.totalCost = INT32_MAX;
    for(auto &sol : population){
        if(sol.feasible && sol.totalCost < best.totalCost){
            best = sol;
        }
    }

    // If no feasible solution found, handle accordingly
    if(best.totalCost == INT32_MAX){
        cout << "No feasible solution found.\n";
        return 0;
    }

    // Output the solution
    for(int i=0;i<N;i++){
        int t = best.t[i] + 1;
        int x = best.placement[i].x;
        int y = best.placement[i].y;
        bool o = best.placement[i].o;
        cout << (i+1) << " " << t << " " << x << " " << y << " " << o << "\n";
    }

    return 0;
}

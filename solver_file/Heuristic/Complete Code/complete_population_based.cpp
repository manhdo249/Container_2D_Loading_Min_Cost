#include <bits/stdc++.h>

using namespace std;

const int MAXN  = 1e3 + 7;
const int inf   = INT_MAX;


/*----------------- DECLARATION -----------------*/
int N_items, N_bins;
int bin_used, total_cost;
int check_algorithm = 0;
int N_sol = 5;

/*----------------- BUILD STRUCTURES FOR ITEMS -----------------*/
// Build structure for each item
struct Items
{
    int width, height, area,
        corner_x, corner_y, id;
    int pos_bin;
    bool rotated = 0;
} item[MAXN], item_guillotine[MAXN], item_maxrec[MAXN];

// Build compare function for list of items
bool compare_item_by_longer_side(Items a, Items b)
{
    if(a.height == b.height) return a.width > b.width;
    return a.height > b.height;
}

// Build compare function for list item to return to the order of the input list
bool compare_reset_item(Items a, Items b)
{
    return a.id < b.id;
}

// Rotating process
void rotate_item(Items &pack)
{
    pack.rotated    = 1 - pack.rotated;
    int temp        = pack.width;
    pack.width      = pack.height;
    pack.height     = temp;
}


/*----------------- BUILD STRUCTURES FOR BINS -----------------*/
//Build structure for list of free_rec in each bin
struct Free_Rectangles
{
    int corner_x, corner_y,
        width, height, area;
    bool operator == (const Free_Rectangles &x) const
    {
        return  (corner_x   == x.corner_x)    &&
                (corner_y   == x.corner_y)    &&
                (width      == x.width)       &&
                (height     == x.height);
    }
};

// Build structure for each bin
struct Bins
{
    int width, height,
        area, free_area,
        cost, id;
    vector <Free_Rectangles> list_of_free_rec;
    vector <Items*> list_of_items;
} bin[MAXN], bin_guillotine[MAXN], bin_maxrec[MAXN];

struct Solution
{
    Items item_sol[MAXN];
    Bins bin_sol[MAXN];
    double total_free_area = 0;
    int total_cost = 0;
};

Solution create_new_sol(){
    Solution sol;
    for (int i=1; i<=N_items; i++){
        sol.item_sol[i] = item[i];
    }
    for (int j=1; j<=N_bins; j++){
        sol.bin_sol[j] = bin[j];
    }
    return sol;
}

vector<Solution> all_sol;

bool compare_sol(const Solution& sol_a, const Solution& sol_b){
    return (double)sol_a.total_cost/(sol_a.total_free_area + 1e-1) < (double)sol_b.total_cost/(sol_b.total_free_area + 1e-1);
}

// Build compare function for list of bins
bool compare_bin_by_density(Bins a, Bins b)
{
    // If the density is equal then sort by larger largest side
    if(float(a.cost)/(a.width * a.height) == float(b.cost)/(b.width * b.height))
    {
        if(max(a.width, a.height) == max(b.width, b.height)) return min(a.width, a.height) > min(b.width, b.height);
        return max(a.width, a.height) > max(b.width, b.height);
    }
    // else sort by lower density
    return float(a.cost)/(a.width * a.height) < float(b.cost)/(b.width * b.height);
}


/*----------------- GENERAL PACKING ITEMS FUNCTIONS -----------------*/
// Check if the item fit a specific free_rec
bool check_fit_rec(Free_Rectangles rec, Items pack, bool rotated)
{
    // Not rotated case
    if((!rotated) && (pack.width <= rec.width) && (pack.height <= rec.height))
        return 1;
    // Rotated case
    if((rotated) && (pack.width <= rec.height) && (pack.height <= rec.width))
        return 1;

    return 0;
}

// Adding item into a bin at the bottom-left-corner of a free_rec
void add_item(Bins &car, Items &pack, bool rotated, int x, int y)
{
    if(rotated) rotate_item(pack);
    pack.corner_x = x;
    pack.corner_y = y;
    car.list_of_items.push_back(&pack);
    car.free_area -= pack.area;
}

// Build compare function for ranking
bool compare_ranking_rec_BSS(pair <int, int> a, pair <int, int> b)
{
    if(a.first == b.first) return a.second < b.second;
    return a.first < b.first;
}
// Calculate score for the item when insert to a free_rec
pair <int, int> score_rec(Free_Rectangles rec, Items pack, bool rotated)
{
    pair <int, int> score;

    // Best Short Side: shorter reaminder side after insertion is minimized, ties broken with best long
    // Rotate case
    if(rotated)
    {
        score.first     = min(rec.width - pack.height, rec.height - pack.width);
        score.second    = max(rec.width - pack.height, rec.height - pack.width);
    }
    // Not rotate case
    else
    {
        score.first     = min(rec.width - pack.width, rec.height - pack.height);
        score.second    = max(rec.width - pack.width, rec.height - pack.height);
    }

    return score;
}
// Find best free_rec for the item in the list free_rec of a bin
pair <pair <Free_Rectangles, unsigned int>, pair <bool, bool>> best_ranking(Bins car, Items pack)
{
    bool rotated = 0;
    Free_Rectangles best_rec;
    unsigned int best_pos = 0;
    int check_exist = 0;
    pair <int, int> best_score;
    pair <pair <Free_Rectangles, unsigned int>, pair <bool, bool>> ans_return;
    best_score.first = best_score.second = inf;

    // Loop to find the best score
    for(unsigned int i = 0; i < car.list_of_free_rec.size(); ++i)
    {
        Free_Rectangles rec = car.list_of_free_rec[i];
        // Not rotate case
        if(check_fit_rec(rec, pack, 0) && compare_ranking_rec_BSS(score_rec(rec, pack, 0), best_score))
        {
            best_score  = score_rec(rec, pack, 0);
            best_rec    = rec;
            best_pos    = i;
            rotated     = 0;
            check_exist = 1;
        }
        // Rotate case
        if(check_fit_rec(rec, pack, 1) && compare_ranking_rec_BSS(score_rec(rec, pack, 1), best_score))
        {
            best_score  = score_rec(rec, pack, 1);
            best_rec    = rec;
            best_pos    = i;
            rotated     = 1;
            check_exist = 1;
        }
    }

    //Prepare for returning value
    ans_return.first.first      = best_rec;
    ans_return.first.second     = best_pos;
    ans_return.second.first     = rotated;
    ans_return.second.second    = check_exist;

    return ans_return;
}


/*----------------- PREPARE SOLUTION -----------------*/
pair <int, int> calculate_solution()
{
    pair <int, int> return_solution;
    total_cost = 0; bin_used = 0;

    // Calculate total number of used bin and total cost
    for(int j = 1; j <= N_bins; ++j)
    {
        if(bin[j].list_of_items.size() > 0)
        {
            total_cost += bin[j].cost;
            ++bin_used;
        }
    }

    return_solution.first   = total_cost;
    return_solution.second  = bin_used;

    return return_solution;
}

pair <int, int> calculate_solution_local_search(vector<Bins>& list_bins){
    pair <int, int> return_solution;
    total_cost = 0; bin_used = 0;

    // Calculate total number of used bin and total cost
    for(const auto& b : list_bins)
    {
        if(b.list_of_items.size() > 0)
        {
            total_cost += b.cost;
            ++bin_used;
        }
    }

    return_solution.first   = total_cost;
    return_solution.second  = bin_used;

    return return_solution;
}


/*----------------- CHECKING ITEMS AND BINS STATUS ------------------*/
void checking_status(int algorithm)
{
//    cout << "/*----------------- These are the items -----------------*/" << '\n';
//    for(int i = 1; i <= N_items; ++i)
//    {
//        Items pack = item[i];
//        cout    << "ID: " << pack.id << ", Width: " << pack.width << ", Height: " << pack.height << ", "
//                << "Corner x: " << pack.corner_x << ", Corner y: " << pack.corner_y << ", Rotated: " << pack.rotated << '\n';
//    }
//    cout << "/*----------------- These are the bins -----------------*/" << '\n';

    // Debugging format: just one algorithm at a time
//    for(int j = 1; j <= N_bins; ++j)
//    {
//        if(bin[j].list_of_items.size() == 0)
//        {
//            continue;
//        }
//        cout << "/*----------------- Consider Bin: ------------------*/" << '\n';
//        cout << "Bin id: " << bin[j].id << '\n';
//        cout << "Bin width: " << bin[j].width << '\n';
//        cout << "Bin height: " << bin[j].height << '\n';
//        cout << "Bin cost: " << bin[j].cost << '\n';
//        cout << "Bin area: " << bin[j].area << '\n';
//        cout << "Bin remain area: " << bin[j].free_area << '\n';
//        cout << "/*----------------- List of items ------------------*/" << '\n';
//        for(auto pack : bin[j].list_of_items)
//        {
//            cout    << "ID: " << pack.id << ", Width: " << pack.width << ", Height: " << pack.height << ", "
//                    << "Corner x: " << pack.corner_x << ", Corner y: " << pack.corner_y << ", Rotated: " << pack.rotated << '\n';
//        }
//        cout << '\n' << '\n';
//    }

    // Output format for claiming data
    // if(algorithm == 0)
    // {
    //     for(int i = 1; i <= N_items; ++i)
    //     {
    //         if(item_guillotine[i].rotated)      cout << "Rotate pack "  << item_guillotine[i].id << " and put";
    //         else                                cout << "Put pack "     << item_guillotine[i].id;
    //         cout    << " in bin " << item_guillotine[i].pos_bin
    //                 << " that the top right corner coordinate (x, y) is ("
    //                 << item_guillotine[i].corner_x + item_guillotine[i].width << ", " << item_guillotine[i].corner_y + item_guillotine[i].height << ")" << '\n';
    //     }
    // }
    // else
    // {
    //     for(int i = 1; i <= N_items; ++i)
    //     {
    //         if(item[i].rotated)     cout << "Rotate pack "  << item[i].id << " and put";
    //         else                    cout << "Put pack "     << item[i].id;
    //         cout    << " in bin " << item[i].pos_bin
    //                 << " that the top right corner coordinate (x, y) is ("
    //                 << item[i].corner_x + item[i].width << ", " << item[i].corner_y + item[i].height << ")" << '\n';
    //     }
    // }

    // Output format for submitting
   if(algorithm == 0)
   {
       sort(item_guillotine + 1, item_guillotine + N_items + 1, compare_reset_item);
       for(int i = 1; i <= N_items; ++i)
       {
           cout    << item_guillotine[i].id        << ' ' << item_guillotine[i].pos_bin  << ' '
                   << item_guillotine[i].corner_x  << ' ' << item_guillotine[i].corner_y << ' '
                   << item_guillotine[i].rotated   << '\n';
       }
   }
   else
   {
       sort(item + 1, item + N_items + 1, compare_reset_item);
       for(int i = 1; i <= N_items; ++i)
       {
           cout    << item[i].id        << ' ' << item[i].pos_bin  << ' '
                   << item[i].corner_x  << ' ' << item[i].corner_y << ' '
                   << item[i].rotated   << '\n';
       }
   }
}

/*----------------- MAXIMAL RECTANGLES ALGORITHM -----------------*/

/*----------------- PACKING ITEMS -----------------*/
// Split initial free_rec after having an insertion
vector <Free_Rectangles> spliting_process_maxrec(Free_Rectangles rec, Items pack)
{
    vector <Free_Rectangles> list_of_free_rec;
    Free_Rectangles new_free_rec;

    // Vertical split to maximize the right corner free_rec
    if(pack.width < rec.width)
    {
        new_free_rec.width      = rec.width - pack.width;
        new_free_rec.height     = rec.height;
        new_free_rec.corner_x   = rec.corner_x + pack.width;
        new_free_rec.corner_y   = rec.corner_y;
        list_of_free_rec.push_back(new_free_rec);
    }

    // Horizontal split to maximize the top corner free_rec
    if(pack.height < rec.height)
    {
        new_free_rec.width      = rec.width;
        new_free_rec.height     = rec.height - pack.height;
        new_free_rec.corner_x   = rec.corner_x;
        new_free_rec.corner_y   = rec.corner_y + pack.height;
        list_of_free_rec.push_back(new_free_rec);
    }

    return list_of_free_rec;
}

// Check if the item intersects other free_rec
bool check_intersec_maxrec(Free_Rectangles rec, Items pack)
{
    if(pack.corner_x >= rec.corner_x + rec.width)   return 0;
    if(pack.corner_y >= rec.corner_y + rec.height)  return 0;
    if(pack.corner_x + pack.width <= rec.corner_x)  return 0;
    if(pack.corner_y + pack.height <= rec.corner_y) return 0;
    return 1;
}

// Find overlap part
Free_Rectangles find_overlap_maxrec(Free_Rectangles rec, Items pack)
{
    Free_Rectangles overlap_rec;
    overlap_rec.corner_x    = max(rec.corner_x, pack.corner_x);
    overlap_rec.corner_y    = max(rec.corner_y, pack.corner_y);
    overlap_rec.width       = min(rec.corner_x + rec.width, pack.corner_x + pack.width) - overlap_rec.corner_x;
    overlap_rec.height      = min(rec.corner_y + rec.height, pack.corner_y + pack.height) - overlap_rec.corner_y;
    return overlap_rec;
}

// Split the intersected rec into smaller free_recs
vector <Free_Rectangles> split_intersect_maxrec (Free_Rectangles initial_rec, Free_Rectangles overlap_rec)
{
    vector <Free_Rectangles> list_of_free_rec;
    Free_Rectangles new_free_rec;
    // Vertical split to maximize the left corner free_rec
    if(overlap_rec.corner_x > initial_rec.corner_x)
    {
        new_free_rec.corner_x   = initial_rec.corner_x;
        new_free_rec.corner_y   = initial_rec.corner_y;
        new_free_rec.width      = overlap_rec.corner_x - new_free_rec.corner_x;
        new_free_rec.height     = initial_rec.height;
        list_of_free_rec.push_back(new_free_rec);
    }

    // Vertical split to maximize the right corner free_rec
    if(overlap_rec.corner_x + overlap_rec.width < initial_rec.corner_x + initial_rec.width)
    {
        new_free_rec.corner_x   = overlap_rec.corner_x + overlap_rec.width;
        new_free_rec.corner_y   = initial_rec.corner_y;
        new_free_rec.width      = initial_rec.corner_x + initial_rec.width - new_free_rec.corner_x;
        new_free_rec.height     = initial_rec.height;
        list_of_free_rec.push_back(new_free_rec);
    }

    // Horizontal split to maximize the bottom corner free_rec
    if(overlap_rec.corner_y > initial_rec.corner_y)
    {
        new_free_rec.corner_x   = initial_rec.corner_x;
        new_free_rec.corner_y   = initial_rec.corner_y;
        new_free_rec.width      = initial_rec.width;
        new_free_rec.height     = overlap_rec.corner_y - new_free_rec.corner_y;
        list_of_free_rec.push_back(new_free_rec);
    }

    // Horizontal split to maximize the top corner free_rec
    if(overlap_rec.corner_y + overlap_rec.height < initial_rec.corner_y + initial_rec.height)
    {
        new_free_rec.corner_x   = initial_rec.corner_x;
        new_free_rec.corner_y   = overlap_rec.corner_y + overlap_rec.height;
        new_free_rec.width      = initial_rec.width;
        new_free_rec.height     = initial_rec.corner_y + initial_rec.height - new_free_rec.corner_y;
        list_of_free_rec.push_back(new_free_rec);
    }

    return list_of_free_rec;
}

// Check if a free_rec is fully covered by another free_rec
bool check_covered_maxrec(Free_Rectangles rec_covering, Free_Rectangles rec_covered)
{
    // Not intersect
    if(rec_covered.corner_x > rec_covering.corner_x + rec_covering.width)   return 0;
    if(rec_covered.corner_y > rec_covering.corner_y + rec_covering.height)  return 0;
    if(rec_covered.corner_x + rec_covered.width < rec_covering.corner_x)    return 0;
    if(rec_covered.corner_y + rec_covered.height < rec_covering.corner_y)   return 0;

    // Intersect but not fully covered
    if(rec_covered.corner_x < rec_covering.corner_x) return 0;
    if(rec_covered.corner_y < rec_covering.corner_y) return 0;
    if(rec_covered.corner_x + rec_covered.width > rec_covering.corner_x + rec_covering.width)   return 0;
    if(rec_covered.corner_y + rec_covered.height > rec_covering.corner_y + rec_covering.height) return 0;

    return 1;
}

// Remove the covered free_rec
void remove_covered_rec_maxrec(Bins &car)
{
    for(unsigned int i = 0; i < car.list_of_free_rec.size(); ++i)
    {
        Free_Rectangles first = car.list_of_free_rec[i];
        for(unsigned int j = i + 1; j < car.list_of_free_rec.size(); ++j)
        {
            Free_Rectangles second = car.list_of_free_rec[j];
            // If rec i cover rec j then delete rec j
            if(check_covered_maxrec(first, second))
            {
                car.list_of_free_rec.erase(car.list_of_free_rec.begin() + j);
                --j;
                continue;
            }
            // If rec j cover rec i then delete rec i
            if(check_covered_maxrec(second, first))
            {
                car.list_of_free_rec.erase(car.list_of_free_rec.begin() + i);
                --i;
                break;
            }
        }
    }
}

// Remove overlap part
void remove_overlap_maxrec(Bins &car, Items pack)
{
    for(unsigned int i = 0; i < car.list_of_free_rec.size(); ++i)
    {
        Free_Rectangles rec = car.list_of_free_rec[i];
        if(check_intersec_maxrec(rec, pack))
        {
            Free_Rectangles overlap_rec = find_overlap_maxrec(rec, pack);
            vector <Free_Rectangles> new_rec = split_intersect_maxrec(rec, overlap_rec);
            car.list_of_free_rec.erase(car.list_of_free_rec.begin() + i);
            for(auto free_rec : new_rec)
            {
                car.list_of_free_rec.push_back(free_rec);
            }
            --i;
        }
    }
    remove_covered_rec_maxrec(car);
}

// Check if item find a possible free_rec in the bin for inserting process
bool insert_item_maxrec(Bins &car, Items &pack)
{
    pair <pair <Free_Rectangles, unsigned int>, pair <bool, bool>> best_ranking_return;
    best_ranking_return         = best_ranking(car, pack);

    // If the free_rec which fits the item does not exist
    if(!best_ranking_return.second.second) return 0;

    // If the free_rec exists
    pack.pos_bin                = car.id;
    Free_Rectangles best_rec    = best_ranking_return.first.first;
    unsigned int best_pos       = best_ranking_return.first.second;
    int rotated                 = best_ranking_return.second.first;
    // Add the item into the choosen free_rec
    add_item(car, pack, rotated, best_rec.corner_x, best_rec.corner_y);
    // Replace the used free_rec by the new splited rec(s)
    car.list_of_free_rec.erase(car.list_of_free_rec.begin() + best_pos);
    vector <Free_Rectangles> new_rec = spliting_process_maxrec(best_rec, pack);
    for(auto rec : new_rec)
    {
        car.list_of_free_rec.push_back(rec);
    }

    // Remove overlap part
    remove_overlap_maxrec(car, pack);

    return 1;
}


/*----------------- BIN CHOOSING -----------------*/
void Solve_maxrec()
{
    for(int i = 1; i <= N_items; ++i)
    {
        // Bin First Fit: choose bin that first fit
        for(int j = 1; j <= N_bins; ++j)
            if(insert_item_maxrec(bin[j],item[i])) break;
    }
}

bool Solve_maxrec_partial(vector<Items*>& list_items, vector<Bins*>& list_bins)
{
    for(auto& pack : list_items)
    {
        bool found_place = false;
        // Bin First Fit: choose bin that first fit
        for(auto& b : list_bins){
            if(insert_item_maxrec(*b, *pack)) {
                found_place = true;
                break;
            }
        }
        if (!found_place) return false;
    }
    return true;
}

void Next_maxrec(int pack_id, const Solution &sol, priority_queue<Solution, vector<Solution>, decltype(&compare_sol)> &pq){
    int prev_j = 0;
    for (int k=0; k < N_sol; k++){
        Solution new_sol = sol;
        bool found_place = false;
        for (int j=prev_j+1; j <= N_bins; j++){ // Next solution will insert in the remaining bins
            int prev_free_area = new_sol.bin_sol[j].free_area;
            if (insert_item_maxrec(new_sol.bin_sol[j], new_sol.item_sol[pack_id])){
                prev_j = j;
                found_place = true;
                // Update metadata of the new_sol: to efficiently calculate the solution's score
                new_sol.total_free_area += (new_sol.bin_sol[j].free_area - prev_free_area);
                if (prev_free_area == new_sol.bin_sol[j].area){
                    // Add the cost of new bin
                    new_sol.total_cost += new_sol.bin_sol[j].cost;
                    new_sol.total_free_area += new_sol.bin_sol[j].area;
                } 
                break;
            }
        }
        if (found_place) {
            pq.push(new_sol);
            if (pq.size() > N_sol){
                pq.pop();
            }
        }
        else return;
    }
}

void Population_maxrec(){
    // Solve
    Solution first_sol = create_new_sol();
    all_sol.push_back(first_sol);
    for (int i=1; i<=N_items; i++){
        priority_queue<Solution, vector<Solution>, decltype(&compare_sol)> pq(compare_sol);
        for (auto &sol : all_sol){
            Next_maxrec(i, sol, pq);
        }
        all_sol.clear();
        while (!pq.empty()) {
            all_sol.push_back(pq.top());
            pq.pop();
        }
    }
    // Find best solution
    int best_cost = INT_MAX;
    Solution best_sol;
    for (auto& sol : all_sol){
        int sol_cost = 0;
        for(int j = 1; j <= N_bins; ++j){
            if(!sol.bin_sol[j].list_of_items.empty()){
                sol_cost += sol.bin_sol[j].cost; // Calculate total cost
            }
        }
        if (best_cost > sol_cost){
            best_cost = sol_cost;
            best_sol = sol;
        }
    }
    // Write best solution to bin_maxrec
    for (int j=1; j<=N_bins; j++){
        bin[j] = best_sol.bin_sol[j];
    }
    for (int i=1; i<=N_items; i++){
        item[i] = best_sol.item_sol[i];
    }
}


/*----------------- GUILLOTINE ALGORITHM -----------------*/

/*----------------- PACKING ITEMS -----------------*/
// Split initial free_rec after having an insertion
vector <Free_Rectangles> spliting_process_guillotine(bool horizontal, Free_Rectangles rec, Items pack)
{
    vector <Free_Rectangles> list_of_free_rec;
    Free_Rectangles new_free_rec;

    int right_x     = rec.corner_x + pack.width,
        right_y     = rec.corner_y,
        right_width = rec.width - pack.width,
        top_x       = rec.corner_x,
        top_y       = rec.corner_y + pack.height,
        top_height  = rec.height - pack.height;

    // horizontal split if split == 1, otherwise vertical split
    int right_height    = (horizontal)? pack.height : rec.height;
    int top_width       = (horizontal)? rec.width   : pack.width;
    if((right_width > 0) & (right_height > 0))
    {
        new_free_rec.corner_x   = right_x;
        new_free_rec.corner_y   = right_y;
        new_free_rec.width      = right_width;
        new_free_rec.height     = right_height;
        list_of_free_rec.push_back(new_free_rec);
    }
    if((top_width > 0) & (top_height > 0))
    {
        new_free_rec.corner_x   = top_x;
        new_free_rec.corner_y   = top_y;
        new_free_rec.width      = top_width;
        new_free_rec.height     = top_height;
        list_of_free_rec.push_back(new_free_rec);
    }
    return list_of_free_rec;
}
// Define spliting method
vector <Free_Rectangles> spliting_guillotine(Free_Rectangles rec, Items pack)
{
    // Split by Shorter Axis: split on the horizontal axis if rec.width <= rec.height
    return spliting_process_guillotine(rec.width <= rec.height, rec, pack);
}

// Merge two initial free_recs into a larger free_rec if possible
void merge_rec_guillotine(Bins &car)
{
    vector <Free_Rectangles> match_width, match_height;

    for(unsigned int i = 0; i < car.list_of_free_rec.size(); ++i)
    {
        Free_Rectangles first   = car.list_of_free_rec[i];
        bool check_exist_width  = 0;
        bool check_exist_height = 0;
        unsigned int pos_check_width     = 0;
        unsigned int pos_check_height    = 0;
        // Loop to find a mergable free_rec for the rec i
        for(unsigned int j = 0; j < car.list_of_free_rec.size(); ++j)
        {
            Free_Rectangles second  = car.list_of_free_rec[j];
            if(j == i) continue;
            // Find the mergable free_rec with the same width
            if((first.width == second.width) && (first.corner_x == second.corner_x) && (second.corner_y == first.corner_y + first.height))
            {
                check_exist_width   = 1;
                pos_check_width     = j;
                break;
            }
            // Find the mergable free_rec with the same height
            if((first.height == second.height) && (first.corner_y == second.corner_y) && (second.corner_x == first.corner_x + first.width))
            {
                check_exist_height  = 1;
                pos_check_height    = j;
                break;
            }
        }
        // Merge two free_rec with the same width
        if(check_exist_width)
        {
            Free_Rectangles merged_rec;
            merged_rec.width    = first.width;
            merged_rec.height   = first.height + car.list_of_free_rec[pos_check_width].height;
            merged_rec.area     = merged_rec.width * merged_rec.height;
            merged_rec.corner_x = first.corner_x;
            merged_rec.corner_y = first.corner_y;
            // Remove the two initial free_recs
            car.list_of_free_rec.erase(car.list_of_free_rec.begin() + pos_check_width);
            if(pos_check_width < i) --i;
            car.list_of_free_rec.erase(car.list_of_free_rec.begin() + i), --i;
            // Add the merged free_rec
            car.list_of_free_rec.push_back(merged_rec);
        }

        // Merge two free_rec with the same height
        if(check_exist_height)
        {
            Free_Rectangles merged_rec;
            merged_rec.width    = first.width + car.list_of_free_rec[pos_check_height].width;
            merged_rec.height   = first.height;
            merged_rec.area     = merged_rec.width * merged_rec.height;
            merged_rec.corner_x = first.corner_x;
            merged_rec.corner_y = first.corner_y;
            // Remove the two initial free_recs
            car.list_of_free_rec.erase(car.list_of_free_rec.begin() + pos_check_height);
            if(pos_check_height < i) --i;
            car.list_of_free_rec.erase(car.list_of_free_rec.begin() + i), --i;
            // Add the merged free_rec
            car.list_of_free_rec.push_back(merged_rec);
        }
    }
}

// Check if item find a possible free_rec in the bin for inserting process
bool insert_item_guillotine(Bins &car, Items &pack)
{
    pair <pair <Free_Rectangles, unsigned int>, pair <bool, bool>> best_ranking_return;
    best_ranking_return         = best_ranking(car, pack);

    // If the free_rec which fits the item does not exist
    if(!best_ranking_return.second.second) return 0;

    // If the free_rec exists
    pack.pos_bin                = car.id;
    Free_Rectangles best_rec    = best_ranking_return.first.first;
    unsigned int best_pos       = best_ranking_return.first.second;
    int rotated                 = best_ranking_return.second.first;
    // Add the item into the choosen free_rec
    add_item(car, pack, rotated, best_rec.corner_x, best_rec.corner_y);
    // Replace the used free_rec by the new splited rec(s)
    car.list_of_free_rec.erase(car.list_of_free_rec.begin() + best_pos);
    vector <Free_Rectangles> new_rec = spliting_guillotine(best_rec, pack);
    for(auto rec : new_rec)
    {
        car.list_of_free_rec.push_back(rec);
    }
    // Merge the free_rec if possible
    merge_rec_guillotine(car);

    return 1;
}


/*----------------- BIN CHOOSING -----------------*/
void Solve_guillotine()
{
    for(int i = 1; i <= N_items; ++i)
    {
        // Bin First Fit: choose bin that first fit
        for(int j = 1; j <= N_bins; ++j)
            if(insert_item_guillotine(bin[j],item[i])) break;
    }
}

bool Solve_guillotine_partial(vector<Items*>& list_items, vector<Bins*>& list_bins)
{
    for(auto& pack : list_items)
    {
        bool found_place = false;
        // Bin First Fit: choose bin that first fit
        for(auto& b : list_bins){
            if(insert_item_guillotine(*b, *pack)) {
                found_place = true;
                break;
            }
        }
        if (!found_place) return false;
    }
    return true;
}

// void Next_guillotine(int pack_id, const Solution &sol, priority_queue<Solution, vector<Solution>, decltype(&compare_sol)> &pq){
//     int prev_j = 0;
//     for (int k=0; k < N_sol; k++){
//         Solution new_sol = move(sol);
//         bool found_place = false;
//         for (int j=prev_j+1; j <= N_bins; j++){ // Next solution will insert in the remaining bins
//             int prev_free_area = new_sol.bin_sol[j].free_area;
//             if (insert_item_maxrec(new_sol.bin_sol[j], new_sol.item_sol[pack_id])){
//                 prev_j = j;
//                 found_place = true;
//                 // Update metadata of the new_sol: to efficiently calculate the solution's score
//                 new_sol.total_free_area += (new_sol.bin_sol[j].free_area - prev_free_area);
//                 if (prev_free_area == new_sol.bin_sol[j].area){
//                     // Add the cost of new bin
//                     new_sol.total_cost += new_sol.bin_sol[j].cost;
//                     new_sol.total_free_area += new_sol.bin_sol[j].area;
//                 } 
//                 break;
//             }
//         }
//         if (found_place) {
//             pq.push(new_sol);
//             if (pq.size() > N_sol){
//                 pq.pop();
//             }
//         }
//         else return;
//     }
// }

// void Population_guillotine(){
//     // Solve
//     Solution first_sol = create_new_sol();
//     all_sol.push_back(first_sol);
//     for (int i=1; i<=N_items; i++){
//         priority_queue<Solution, vector<Solution>, decltype(&compare_sol)> pq(compare_sol);
//         for (auto &sol : all_sol){
//             Next_guillotine(i, sol, pq);
//         }
//         all_sol.clear();
//         while (!pq.empty()) {
//             all_sol.push_back(pq.top());
//             pq.pop();
//         }
//     }
//     // Find best solution
//     int best_cost = INT_MAX;
//     Solution best_sol;
//     for (auto& sol : all_sol){
//         int sol_cost = 0;
//         for(int j = 1; j <= N_bins; ++j){
//             if(!sol.bin_sol[j].list_of_items.empty()){
//                 sol_cost += sol.bin_sol[j].cost; // Calculate total cost
//             }
//         }
//         if (best_cost > sol_cost){
//             best_cost = sol_cost;
//             best_sol = move(sol);
//         }
//     }
//     // Write best solution to bin_maxrec
//     for (int j=1; j<=N_bins; j++){
//         bin[j] = best_sol.bin_sol[j];
//     }
//     for (int i=1; i<=N_items; i++){
//         item[i] = best_sol.item_sol[i];
//     }
// }

/*---------------- LOCAL SEARCH ------------------*/

#include <random>

// Return largest free area across the bins: the larger -> the more compact the packing
int calculate_score(vector<Bins*> list_bins){
    int top_empty_space = INT_MIN;
    for (const auto & b : list_bins){
        if (b->free_area > top_empty_space) top_empty_space = b->free_area;
    }
    return top_empty_space;
}

// Function to calculate the top 1 bin and select K random bins
vector<Bins*> calculate_top_and_random_bins(vector<Bins>& bins, int K) {
    vector<Bins*> selected_bins;

    if (bins.empty() || K < 0) {
        // Return early if no bins or invalid K
        return selected_bins;
    }

    int top1_empty_space = INT_MIN;
    Bins* top1_bin = nullptr;

    // Find top 1 bin with the most free space
    for (auto& b : bins) {
        if (!b.list_of_items.empty()) {  // Only consider bins with items
            if (b.free_area > top1_empty_space) {
                top1_bin = &b;
                top1_empty_space = b.free_area;
            }
        }
    }

    // If top1_bin is found, proceed to select K random bins from the remaining bins
    if (top1_bin != nullptr) {
        selected_bins.push_back(top1_bin);  // Add top1_bin to the selected bins list

        vector<Bins*> other_bins;

        // Collect bins that are not the top1_bin
        for (auto& b : bins) {
            if (&b != top1_bin && !b.list_of_items.empty()) {
                other_bins.push_back(&b);
            }
        }
        // auto seed = std::chrono::duration_cast<std::chrono::microseconds>(
        //             std::chrono::high_resolution_clock::now().time_since_epoch()
        //         ).count();
        // default_random_engine generator(seed);

        random_device rd;
        mt19937 generator(rd());

        // Select K random bins from the remaining bins
        while (selected_bins.size() < K && !other_bins.empty()) {
            uniform_int_distribution<int> distribution(0, (int)(other_bins.size())-1);
            int index = distribution(generator);  // Generate random index
            selected_bins.push_back(other_bins[index]);
            other_bins.erase(other_bins.begin() + index);  // Remove selected bin
        }
    }
    sort(selected_bins.begin(), selected_bins.end(), [](Bins* b1, Bins* b2) {
        return compare_bin_by_density(*b1, *b2);  // Sort by density of each bin
    });
    return selected_bins;
}

// Function to reset a bin to its initial state
void reset_bin(Bins* b) {

    b->free_area = b->area;
    b->list_of_items.clear();
    b->list_of_free_rec.clear();

    Free_Rectangles first_rec;
    first_rec.width     = b->width;
    first_rec.height    = b->height;
    first_rec.corner_x  = 0;
    first_rec.corner_y  = 0;
    first_rec.area      = first_rec.width * first_rec.height;

    b->list_of_free_rec.push_back(first_rec);
}

// Function to rotate item so that long side is height and reset pos_x and pos_y
void reset_item(Items * pack){
    if (pack->width > pack->height) rotate_item(*pack);
    pack->corner_x = 0;
    pack->corner_y = 0;
}

// Retrieve the item in the bin and Reset the bin 
vector<Items*> reset_bin_and_retrieve_item(vector<Bins*>& list_bins){
    vector<Items*> list_items;

    // Add and reset the item of the top bin
    for (auto & b : list_bins){
        for (const auto& pack : b->list_of_items){
            list_items.push_back(pack);
        }
        reset_bin(b);
    }

    sort(list_items.begin(), list_items.end(), [](Items* pack1, Items* pack2) {
        return compare_item_by_longer_side(*pack1, *pack2); 
    });

    return list_items;
} 

// Local Search Implementation
void local_search(vector<Bins>& current_bins,int max_iter) {
    while(max_iter--) {

        // Select the bins
        vector<Bins*> list_bins = calculate_top_and_random_bins(current_bins, min(5, N_bins));

        int best_score = calculate_score(list_bins);

        // Store the temporal bins
        vector<Bins> save_bin;
        for (auto b: list_bins) save_bin.push_back(*b);

        // New solution for the top bin
        vector<Items*> list_items = reset_bin_and_retrieve_item(list_bins);

        // Store the temporal items
        vector<Items> save_item;
        for (auto pack: list_items) {
            save_item.push_back(*pack);
            reset_item(pack);
        }

        bool found_sol = Solve_maxrec_partial(list_items, list_bins);

        int new_score = calculate_score(list_bins);
        
        // No improvement or No solution
        if (!found_sol || best_score > new_score){
            for (int j=0; j < (int)list_bins.size(); j++) swap(*list_bins[j], save_bin[j]);
            for (int i=0; i < (int)list_items.size(); i++) swap(*list_items[i], save_item[i]);
        }
        else{
            cout << "Current score: " << new_score << endl;
        }
    }
}

// 
// Simulated annealing
// 

// Local Search Implementation using Simulated Annealing
void simulated_annealing(vector<Bins>& current_bins, int max_iter, double initial_temperature, double alpha) {
    
    // Random number generator for probability calculation
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    // Set the initial temperature
    double temperature = initial_temperature;

    while (max_iter--) {

        // Select the bins (could be top bins, top + random, etc.)
        vector<Bins*> list_bins = calculate_top_and_random_bins(current_bins, min(5, N_bins));

        int best_score = calculate_score(list_bins);

        // Store the temporal bins
        vector<Bins> save_bin;
        for (auto b: list_bins) save_bin.push_back(*b);

        // New solution for the top bin
        vector<Items*> list_items = reset_bin_and_retrieve_item(list_bins);

        // Store the temporal items
        vector<Items> save_item;
        for (auto pack: list_items) {
            save_item.push_back(*pack);
            reset_item(pack);
        }

        bool found_sol = Solve_maxrec_partial(list_items, list_bins);

        int new_score = calculate_score(list_bins);

        // If no solution or no improvement, we can still accept a worse solution with some probability
        if (!found_sol || best_score > new_score) {
            // Calculate the probability of accepting the worse solution
            double probability = exp((new_score - best_score) / temperature);
            
            // Accept the worse solution with the calculated probability
            if (dis(gen) < probability) {
                // Accept the new configuration
                cout << "Current score: " << new_score << endl;
            } else {
                // Revert to the previous solution
                for (int j = 0; j < (int)list_bins.size(); j++) swap(*list_bins[j], save_bin[j]);
                for (int i = 0; i < (int)list_items.size(); i++) swap(*list_items[i], save_item[i]);
            }
        } else {
            // If we found a better solution, accept it
            cout << "Current score: " << new_score << endl;
        }

        // Gradually decrease the temperature
        temperature *= alpha;  // Cooling schedule (0 < alpha < 1, e.g., alpha = 0.99)

        // Print or log the temperature or score at certain intervals
        // cout << "Iteration " << max_iter << ", Temperature: " << temperature << ", Score: " << new_score << endl;
    }
}

/*----------------- MAIN PROGRAM ------------------*/

/*----------------- READ INPUT -----------------*/
void Enter()
{
    cin >> N_items >> N_bins;
    // Read and prepare for the list of items
    for(int i = 1; i <= N_items; ++i)
    {
        cin >> item[i].width >> item[i].height;
        if(item[i].width > item[i].height) rotate_item(item[i]);
        item[i].area    = item[i].width * item[i].height;
        item[i].id      = i;
    }

    // Read and prepare for the list of bins
    for(int j = 1; j <= N_bins; ++j)
    {
        cin >> bin[j].width >> bin[j].height >> bin[j].cost;
        bin[j].area = bin[j].width * bin[j].height; bin[j].id = j;
        bin[j].free_area    = bin[j].area;
        Free_Rectangles first_rec;
        first_rec.width     = bin[j].width;
        first_rec.height    = bin[j].height;
        first_rec.corner_x  = first_rec.corner_y = 0;
        first_rec.area      = first_rec.width * first_rec.height;
        bin[j].list_of_free_rec.push_back(first_rec);
    }

    sort(item + 1, item + N_items + 1, compare_item_by_longer_side);
    sort(bin + 1, bin + N_bins + 1, compare_bin_by_density);
}


/*----------------- RESET -----------------*/
void reset(int check_algorithm)
{
    total_cost = 0;

    // Save packing solution for guillotine algorithm and reset items for next algorithm
    for(int i = 1; i <= N_items; ++i)
    {
        // Save guillotine solution
        if (check_algorithm == 0) item_guillotine[i] = item[i];
        else if (check_algorithm == 1) item_maxrec[i] = item[i];
        // Reset items
        item[i].corner_x = 0;
        item[i].corner_y = 0;
    }

    // Reset bins for next algorithm
    for(int j = 1; j <= N_bins; ++j)
    {
        // Save solution
        if (check_algorithm == 0) bin_guillotine[j] = bin[j];
        else if (check_algorithm == 1) bin_maxrec[j] = bin[j];
        // Reset bins
        bin[j].free_area = bin[j].area;
        bin[j].list_of_items.clear();
        bin[j].list_of_free_rec.clear();
        Free_Rectangles first_rec;
        first_rec.width     = bin[j].width;
        first_rec.height    = bin[j].height;
        first_rec.corner_x  = first_rec.corner_y = 0;
        first_rec.area      = first_rec.width * first_rec.height;
        bin[j].list_of_free_rec.push_back(first_rec);
    }
}

vector<Bins> restore_for_local_search(int check_algorithm){
    vector<Bins> current_bins;
    if (check_algorithm == 0){
        for(int i = 1; i <= N_items; ++i) item[i] = item_guillotine[i];
        for(int j = 1; j <= N_bins; ++j) current_bins.push_back(bin_guillotine[j]);
    }
    else if (check_algorithm == 1){
        for(int i = 1; i <= N_items; ++i) item[i] = item_maxrec[i];
        for(int j = 1; j <= N_bins; ++j) current_bins.push_back(bin_maxrec[j]);
    }
    return current_bins;
}


/*----------------- SOLVE -----------------*/
void Solve()
{
    Enter();
    // Run Gullotine algorithm
    Solve_guillotine();
    // Population_guillotine();
    pair <int, int> guillotine_result   = calculate_solution();
    reset(0);
    // Run Maxrec algorithm
    // Solve_maxrec();
    Population_maxrec();
    pair <int, int> maxrec_result       = calculate_solution();
    reset(1);
    if(guillotine_result.first < maxrec_result.first)
    {
        total_cost  = guillotine_result.first;
        bin_used    = guillotine_result.second;
        check_algorithm = 0;
    }
    else
    {
        total_cost  = maxrec_result.first;
        bin_used    = maxrec_result.second;
        check_algorithm = 1;
    }
    vector<Bins> current_bin = restore_for_local_search(check_algorithm);
    local_search(current_bin, 0);
    // simulated_annealing(current_bin, 10000, 10000, 0.99);
    check_algorithm = 1; // local search    
}


/*----------------- PRINT OUTPUT -----------------*/
void Print()
{
    // Print the input sizes
    // cout << "Number of item given: "    << N_items      << '\n';
    // cout << "Number of bin given: "     << N_bins       << '\n';

    // Uncomment for more detailed information of packed items in each used bin
//    cout << "Algorithm used: ";
//    if(!check_algorithm)    cout << "Guillotine"            << '\n';
//    else                    cout << "Maximal Rectangles"    << '\n';
    checking_status(check_algorithm);

    // Print the solution
    // cout << "Number of bin used: "      << bin_used     << '\n';
    // cout << "Total cost: "              << total_cost   << '\n';
}


/*----------------- MAIN -----------------*/
signed main(/*int argv, char* argc[]*/)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    // if(argv < 2)
    // {
    //     cout << "Error: No file name provided." << '\n';
    //     return 0;
    // }
    // freopen(argc[1], "r", stdin);
//    freopen("testing.data", "r", stdin);
//    freopen("testing2.out", "w", stdout);

    // Start timing process
    // clock_t start_timing    = clock();

    // Run solver
    Solve();

    // Stop timing process
    // clock_t end_timing      = clock();

    // Print output
    Print();
    // Print additional informations
    // cout << "Status: "                  << "None"       << '\n';
    // cout << "Time limit: "              << "None"       << '\n';
    // cout << "Running time: ";
    // cout.precision(20);
    // cout << float(end_timing - start_timing)/CLOCKS_PER_SEC << '\n';
}

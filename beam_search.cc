#include "beam_search.h"

void solverClass::initilize(stateClass &root)
{
    //cout << "init..." << endl;
    states.clear();
    states.reserve(1 << 20);

    for(auto &item : input_.items_)
    {
        if(item.machine_ != "" && item.machine_ != "None")
        {
            if(input_.machineMap_.find(item.machine_) != input_.machineMap_.end())
            {
                auto *machine = input_.machineMap_[item.machine_];
                int mac_id = machine->id_;
                root.last_times_[mac_id] = item.first_start_ + ceil((double) item.qty_ / machine->capacity_);
                root.last_items_[mac_id] = item.id_;
            }
            
            root.completed_[item.id_] = 1;
        }
    }

    //cout << "complete!" << endl;
}

void solverClass::optimize()
{
    w0 = 100 * (double) xor128() / ULONG_MAX;
    w1 = 100 * (double) xor128() / ULONG_MAX;
    w2 = 100 * (double) xor128() / ULONG_MAX;

    timerClass timer;
    timer.start();

    const int nitem = input_.items_.size();
    const int nmach = input_.machines_.size();
    const int end_time = input_.end_time;

    //init
    stateClass root = stateClass(nmach);
    initilize(root);
    states.push_back(root);
    
    vector<int> node_ids, next_node_ids;
    node_ids.push_back(0);

    //cout << "start optimize..." << endl;
    //cout << "nitem = " << nitem << " nmac = " << nmach << endl;
    
    vector<candidateClass> candidates;
    candidates.reserve(1 << 20);
    for(int step = 0; step < nitem; ++step)
    {
        candidates.clear();
        for(auto node_id : node_ids)
        {
            auto &state = states[node_id];
            for(auto &item : input_.items_)
            {
                int item_id = item.id_;
                if(state.completed_[item_id])
                    continue;
                for (auto &machine : input_.machines_)
                {
                    int mac_id = machine.id_;
                    if(input_.availables_[mac_id][item_id] && input_.setups_[state.last_items_[mac_id]][item_id] < end_time)
                    {
                        //if(input_.sol_[mac_id][state.n_items_[mac_id]] != item_id)
                        //        continue;
                        //auto candidate = getCandidate(item, machine, state, 0);
                        for(int do_over_time = 1; do_over_time <= 1; ++do_over_time)
                        {
                            auto cand = getCandidate(item, machine, state, do_over_time);
                            if(cand.dead_time_ == 0)
                            {
                                if(input_.sol_[mac_id][state.n_items_[mac_id]] != item_id)
                                    cand.good = 0;
                                else 
                                    cand.good = 1;
                                candidates.push_back(cand);

                            }
                                
                        }   

                    }
                }
            }
        }
        if(candidates.size() == 0) 
            break;

        const int W = min(10000, (int) candidates.size());
        partial_sort(candidates.begin(), candidates.begin() + W, candidates.end(), 
            [&](const candidateClass &lhs, const candidateClass &rhs)
            {
                auto &lstate = states[lhs.node_id_];
                auto &rstate = states[rhs.node_id_];
                return lstate.score_ + lhs.score_ < rstate.score_ + rhs.score_;
            } );
        candidates.resize(W);

        node_ids.clear();
        
        int ic = 0;
        for(auto &candidate : candidates)
        {
            stateClass &before = states[candidate.node_id_];
            double after_score = before.score_ + candidate.score_;
            
            if(step == 0 && ic < -200)
            { 
                auto &item = input_.items_[candidate.item_id_];
                cout << step << " " << ic++ << " " << candidate.good << " " << after_score << " " << candidate.item_id_ << " " << candidate.mac_id_ << " ";
                cout << candidate.start_time_ << " " << candidate.end_time_ <<  " " << item.complete_ << endl;;
            }
            //if(after_score != states.back().score_)
            {
                stateClass after = states[candidate.node_id_];
                
                after.accept(candidate, candidate.score_);

                if(states.back().completed_ == after.completed_ && states.back().score_ == after.score_)
                    continue;

                after.id_ = states.size();
                states.push_back(after);

                node_ids.push_back(after.id_);
            }
            
        } 

        if(step == -5)
        {
            int no = 0;
            for(auto &node_id : node_ids)
            {
                auto &state = states[node_id];
                bool ok = true;
                for(int i = 0; i < 4; ++i)
                {
                    int n = state.n_items_[i];
                    if(n > 1)
                    {
                        if(input_.sol_[i][n - 1] != state.last_items_[i])
                            ok = false;
                    }
                }
                if(ok)
                cout << no << " " << node_id << " " << ok << state.score_ << endl;
                //cout << state.score_ << endl;
                ++no;
            }
        }
        
        const int W2 = min(100000, (int) node_ids.size());
        partial_sort(node_ids.begin(), node_ids.begin() + W2, node_ids.end(),
        [&](const int &lhs, const int &rhs){
            return states[lhs].score_ < states[rhs].score_;
        });
        //states.resize(W2);
    }
    cout << "elapsed time " << timer.elapsed() << "[sec]" << endl;

    return;

}

candidateClass solverClass::getCandidate(itemClass &item, machineClass&machine, stateClass &state, int over_time)
{
    //const int comp_time = item.complete_;
    const int item_id = item.id_, mac_id = machine.id_;
    auto &last_fin_time = state.last_times_;
    auto &last_item = state.last_items_;
    int setup_time = input_.setups_[last_item[mac_id]][item_id];
    int test_start_time = max(max(0, last_fin_time[mac_id]), item.start_);
    int test_end_time = test_start_time + input_.setups_[last_item[mac_id]][item_id] + ceil((double) item.qty_ / machine.capacity_);
    
    int tot_lap_time = 0;
    int start_day = test_start_time / 600;
    int lap_time = 0;
    do
    {
            
        int start_over_time = 600 * start_day + 480;
        int end_over_time   = 600 * (start_day + 1);

        int overlap_start = max(test_start_time, start_over_time);
        int overlap_end   = min(test_end_time,   end_over_time);
        
        lap_time = max(overlap_end - overlap_start, 0);
        
        if(!over_time)
        {
            if(test_start_time >= start_over_time || lap_time == 0) 
            {
                test_start_time += lap_time;
                test_end_time   += lap_time;
            }
            else 
            {
                //test_start_time += (end_over_time - start_over_time);
                test_end_time   += (end_over_time - start_over_time);
            }
        }
        else 
            tot_lap_time += lap_time;
        ++start_day;
    } while(lap_time != 0);

    double process_time =  ceil((double) item.qty_ / machine.capacity_);
    double process_day  = ceil(process_time / 600.0);
    double score = 0 * max(0, -item.complete_ + test_end_time)
                    + 0 * tot_lap_time 
                    + w0 * max(0, item.complete_ - test_end_time) 
                    + w1 * max(0, test_start_time - last_item[mac_id])
                    //+ w2 * (test_end_time - test_start_time)
                    //- 3 * process_time * (item.complete_ - test_end_time < 100)
                    //+ 1 * test_end_time
                    //- w1 * 100000000 * ( (item.complete_ - test_end_time) < ((double) item.qty_ / machine.capacity_) )
                    //- 100 * (  (item.complete_ - test_end_time) / process_time < 1.0 ? ((item.complete_ - test_end_time) / process_time) : 0)
                    //+ w1 * item.complete_
                    //+ w2 * (test_start_time - last_item[machine.id_]) 
                    //+ 100 * setup_time
                    ;
    //score /= process_day;
    candidateClass candidate = candidateClass(item_id, mac_id, test_start_time, test_end_time, 
        tot_lap_time, max(0, -item.complete_ + test_end_time), state.id_, score);

    return candidate;
}

solutionClass solverClass::solution_of_(stateClass &state)
{
    solutionClass solution = solutionClass(input_.items_.size());
    
    for(int i = 0; i < solution.jobs.size(); ++i)
    {
        solution.jobs[i].item_id_ = i;
        solution.jobs[i].completed_ = false;
    }

    stateClass *node = &state;
    do
    {
        auto &item = input_.items_[node->cand_.item_id_];
        auto &job = solution.jobs[item.id_];
        job.mac_id_     = node->cand_.mac_id_;
        job.item_id_    = node->cand_.item_id_;
        job.start_time_ = node->cand_.start_time_;
        job.end_time_   = node->cand_.end_time_;
        job.completed_  = true;
        job.dead_time_  = max(job.end_time_ - item.complete_, 0);
        job.over_time_  = node->cand_.over_time_;
        node = &states[node->parent_id_];
    } while(node->parent_id_ != -1);
    return solution;
}

void solverClass::output()
{
    //pair<double, int> best_id = {0, 0};
    //tuple<int, double, int> best_id = {0,0,0};
    
    stateClass *best_state = &states[0];
    for(auto &state : states)
    {
        //cout << state.n_completed_ << endl;
        if(state > *best_state)
            best_state = &state;

    }
        
    //auto &best_state = states[best_id.second];
    auto solution   = solution_of_(*best_state);

    int tot_dead = 0;
    auto &jobs = solution.jobs;
    sort(jobs.begin(), jobs.end());
    for(auto &job : jobs)
    {
   
        auto &item = input_.items_[job.item_id_];
            
        if(job.completed_)
        {
            
            auto &machine = input_.machines_[job.mac_id_];
            cout 
                << " start: "    << std::setw(5) << std::setfill('0') << max(0, item.start_) 
                << " complted: " << std::setw(5) << std::setfill('0') << item.complete_ 
                << " qty:"       << std::setw(5) << std::setfill('0') << item.qty_ 
                << " pt:"        << std::setw(3) << std::setfill('0') << ceil((double) item.qty_ / machine.capacity_)
                << " / mac: "    << std::setw(2) << std::setfill('0') << machine.name_
                << " item : "    << std::setw(6) << std::setfill('0') << item.name_
                << " start: "    << std::setw(5) << std::setfill('0') << job.start_time_ 
                << " end: "      << std::setw(5) << std::setfill('0') << job.end_time_ 
                << " dead: "     << std::setw(5) << std::setfill('0') << job.dead_time_ 
                << " over: "     << std::setw(5) << std::setfill('0') << job.over_time_ 
            << endl;

            tot_dead += job.dead_time_;
        }
        else 
        {
              cout << "not comp " << item.id_ << " " << item.name_ << " " << item.complete_ << " " << item.qty_ <<  endl;
            item.backword_ += 100;
        }
    }
    //cout << "score = " << best_state.score_ << endl;
    cout << best_state->n_completed_ << endl;
    if(best_state->n_completed_ == 75)
        exit(0);
    //cout << "tot dead = " << tot_dead << endl;

    ofstream sol("./output/solution.dat");
    const int nmach = input_.machines_.size();
    vector<int> last_items(nmach, -1);
    for(auto &item : input_.items_)
    {
        if(item.machine_ != "" && item.machine_ != "None" && input_.machineMap_.find(item.machine_) != input_.machineMap_.end())
        {
            auto *machine = input_.machineMap_[item.machine_];
            last_items[machine->id_] = item.id_;
        }
    }

    sol << "機械名称,アイテム名称,開始時間,終了時間,型替時間" << endl;

    for(auto &job : jobs)
    {
        if(job.completed_)
        {
            auto &item = input_.items_[job.item_id_];
            auto &machine = input_.machines_[job.mac_id_];
            auto last_item = last_items[machine.id_];
            int setup = input_.setups_[last_item][item.id_];
            sol << machine.name_ << "," << item.name_ << "," << job.start_time_ << "," << job.end_time_ << "," << setup << endl;

            last_items[machine.id_] = item.id_;
        }
    }
    sol.close();

    return;
}
#ifndef _BEAM_SEARCH_H
#define _BEAM_SEARCH_H

#include "utils.h"
#include "input.h"

class jobClass
{
    private:
    public:
        jobClass() = default;
        jobClass(int start_time, int end_time, int mac_id, int item_id)
        : start_time_(start_time), end_time_(end_time), mac_id_(mac_id), item_id_(item_id), completed_(false) { }
        
        int start_time_, end_time_, mac_id_, item_id_, dead_time_, over_time_;
        bool completed_;

        void init()
        {
            completed_ = false;
        }
        
        bool operator<(const jobClass& other) const 
        {
            if (mac_id_ < other.mac_id_) {
                return true;
            } else if (mac_id_ == other.mac_id_) {
                return start_time_ < other.start_time_;
            } else {
                return false;
            }
        }
};

class actionClass
{
    private:
    public :
        actionClass() = default;
        actionClass(int item_id, int mac_id, int start_time, int end_time)
        : item_id_(item_id), mac_id_(mac_id), start_time_(start_time), end_time_(end_time) { }
        int item_id_, mac_id_, start_time_, end_time_;
};

class solutionClass
{
    private:
    public:
        solutionClass() = default;
        solutionClass(int item_size) 
        {
            jobs.resize(item_size);
        }
        vector<jobClass> jobs;
        
        void write()
        {
            auto sorted_jobs = jobs;
            sort(sorted_jobs.begin(), sorted_jobs.end());
            cerr << "machine_name,item_name,start_time_,end_time_" << endl;
            for(auto &job : sorted_jobs)
            {
                if(job.completed_)
                cerr << job.mac_id_ << " " << job.item_id_ << " " << job.start_time_ << " " << job.end_time_ << endl;
            }
        }
        
};

class candidateClass
{
    private:
    public:
        candidateClass() = default;
        candidateClass(int item_id, int mac_id, int start_time, int end_time, int over_time, int dead_time, int node_id, double score)
        : item_id_(item_id), mac_id_(mac_id), start_time_(start_time), end_time_(end_time)
        , over_time_(over_time), dead_time_(dead_time), node_id_(node_id), score_(score) { }

        int item_id_, mac_id_, start_time_, end_time_;
        int over_time_, dead_time_, node_id_;
        double score_;
        int good;
};

class stateClass
{
    private:
    public:
        stateClass() = default;
        stateClass(int mac_size)
        {
            id_ = 0;
            parent_id_ = -1;
            score_ = 0.0;
            completed_ = 0;
            n_completed_ = 0;
            last_times_.resize(mac_size, 0);
            last_items_.resize(mac_size, -1);

            n_items_.resize(mac_size, 1);
        }
        
        int id_, parent_id_, n_completed_;
        double score_;
        bitset<256> completed_;
        vector<int> last_times_, last_items_;
        candidateClass cand_;

        vector<int> n_items_;

        void accept(candidateClass &cand, double cand_score)
        {
            cand_ = cand;
            completed_[cand_.item_id_] = 1;
            last_times_[cand_.mac_id_] = cand_.end_time_;
            last_items_[cand_.mac_id_] = cand_.item_id_;
            parent_id_ = cand.node_id_;
            n_completed_ += 1;
            score_ += (cand_score);
            ++n_items_[cand_.mac_id_];
        }
        bool operator<(const stateClass& other) const 
        {
            if (n_completed_ < other.n_completed_) {
                return true;
            } else if (n_completed_ == other.n_completed_) {
                return score_ < other.score_;
            } else {
                return false;
            }
        }
        bool operator > (const stateClass& other) const 
        {
            if (n_completed_ > other.n_completed_) {
                return true;
            } else if (n_completed_ == other.n_completed_) {
                return score_ > other.score_;
            } else {
                return false;
            }
        }
};

class solverClass
{
    private:
        input input_;
        vector<stateClass> states;
        double w0, w1, w2;
    public:
        solverClass() = default;
        solverClass(input input_) : input_(input_)
        {
            states.reserve(1 << 20);
        }
        void initilize(stateClass &root);
        void optimize();
        void output();
        candidateClass getCandidate(itemClass &item, machineClass&machine, stateClass &state, int overtime);
        solutionClass solution_of_(stateClass &state);

} ;


#endif
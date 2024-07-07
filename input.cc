    
#include "input.h"

void input::read()
{
    input_texts_.push_back(textReaderClass("./input/item.dat"));
    input_texts_.push_back(textReaderClass("./input/machine.dat"));
    input_texts_.push_back(textReaderClass("./input/params.dat"));
    input_texts_.push_back(textReaderClass("./input/setup.dat"));
    input_texts_.push_back(textReaderClass("./input/available_machine.dat"));
    input_texts_.push_back(textReaderClass("./input/calendar.dat"));
    input_texts_.push_back(textReaderClass("./input/sol.dat"));

    read_items(input_texts_[0]);
    read_machine(input_texts_[1]);
    read_params(input_texts_[2]);
    read_setups(input_texts_[3]);
    read_available_machine(input_texts_[4]);
    read_calendar(input_texts_[5]);
    read_sol(input_texts_[6]);

}

void input::read_items(textReaderClass &text)
{
    cout << "伝票情報読み込み" << endl;
    items_.reserve(1 << 16);
    //**,開始予定時間,完了予定時間,機械名称,開始時刻
    for(int i = 0; i < text.getNumRow(); ++i)
    {
        string name = text.getCell(i, 0);
        int start = doubleParse(text.getCell(i, 1));
        int comp  = doubleParse(text.getCell(i, 2));
        string machine = text.getCell(i, 3);
        int end = doubleParse(text.getCell(i, 4));
        int q = doubleParse(text.getCell(i, 5));

        items_.push_back(itemClass(i, name, start, comp, machine, end, q));
        itemMap_[name] = &items_.back();            
    }
    //cout << items_.size() << endl;
}
void input::read_params(textReaderClass &text)
{
    cout << "パラメータ情報読み込み" << endl;
    for (int i = 0; i < text.getNumRow(); ++i)
    {
        string key = text.getCell(i, 0);
        double val = doubleParse(text.getCell(i, 1));
        if(key == "期間")
            end_time = val;
        else 
            timelimit = val;
    }
}
void input::read_setups(textReaderClass &text)
{
    cout << "型替情報読み込み" << endl;
    const int nitem = items_.size();
    setups_.resize(nitem, vector<int>(nitem, INT_MAX / 2));
    for(int i = 0; i < text.getNumRow(); ++i)
    {
        string from = text.getCell(i, 0);
        string to   = text.getCell(i, 1);
        int val = doubleParse(text.getCell(i, 2));
        
        if(itemMap_.find(from) == itemMap_.end())
            continue;
        else if(itemMap_.find(to) == itemMap_.end())
            continue;
        
        auto from_item = itemMap_[from];
        auto to_item = itemMap_[to];
        if(from_item && to_item)
            setups_[from_item->id_][to_item->id_] = val;
    }
}
void input::read_machine(textReaderClass &text)
{
    cout << "機械情報読み込み" << endl;
    machines_.reserve(1 << 16);
    for(int i = 0; i < text.getNumRow(); ++i)
    {
        string machine_name = text.getCell(i, 0);
        int capacity = doubleParse(text.getCell(i, 1));
        machines_.push_back({i, machine_name, capacity});
        machineMap_[machine_name] = &machines_.back();
    }
}
void input::read_available_machine(textReaderClass &text)
{
    cout << "使用可能機械情報読み込み" << endl;

    const int ni = items_.size(), nm = machines_.size();
    availables_.resize(nm, vector<int>(ni, 0));
    for(int i = 0; i < text.getNumRow(); ++i)
    {
        string item_name = text.getCell(i, 0);
        string machine_name = text.getCell(i, 1);
        string main_or_sub = text.getCell(i, 2);

        if(itemMap_.find(item_name) == itemMap_.end())
            continue;
        int item_id = itemMap_[item_name]->id_;
        int mac_id = machineMap_[machine_name]->id_;
        int main_or_sub_id = main_or_sub == "True" ? 2 : 1;

        availables_[mac_id][item_id] = main_or_sub_id;
    }
    
}
void input::read_calendar(textReaderClass &text)
{
    cout << "カレンダー情報読み込み" << endl;
    calendars_.resize(machines_.size(), vector<string>(100, ""));

    for(int i = 0; i < text.getNumRow(); ++i)
    {
        string machine_name = text.getCell(i, 0);
        int day = doubleParse(text.getCell(i, 1));
        string event = text.getCell(i, 2);
        int machine_id = machineMap_[machine_name]->id_;
        calendars_[machine_id][day] = event;
    }
}
void input::read_sol(textReaderClass &text)
{
    cout << "参考情報読み込み" << endl;

    sol_.resize(machines_.size());

    for(int i = 0; i < text.getNumRow(); ++i)
    {
        string machine_name = text.getCell(i, 0);
        string item_name = text.getCell(i, 1);

        auto &machine = machineMap_[machine_name];
        auto &item    = itemMap_[item_name];
        sol_[machine->id_].push_back(item->id_);
    }
}
#ifndef _INPUT_H
#define _INPUT_H

#include "utils.h"
#include "text_reader.h"

class itemClass
{
    private:
    public:
        itemClass() = default;
        itemClass(int id, string name, int start, int complete, string machine, int first_start, int qty) :
         id_(id), name_(name), start_(start), complete_(complete), machine_(machine), first_start_(first_start), qty_(qty)
         { }
    int id_;
    string name_;
    int start_;
    int complete_;
    string machine_;
    int first_start_;
    int qty_;
    int backword_;
};

class machineClass
{
    public:
        machineClass() = default;
        machineClass(int id, string name, int capacity) : id_(id), name_(name), capacity_(capacity) { }
        int id_;
        string name_;
        int capacity_;
};

class input
{
private:
    vector<textReaderClass> input_texts_;
public:
	input()
	{
	}
	
    double end_time, timelimit;
    map<string, itemClass*> itemMap_;
    vector<itemClass> items_;
    map<string, machineClass*> machineMap_;
    vector<machineClass> machines_;
    vector<vector<int>> setups_;
    vector<vector<int>> availables_;
    vector<vector<string>> calendars_;
    vector<vector<int>> sol_;

    void read();
    void read_items(textReaderClass &text);
    void read_params(textReaderClass &text);
    void read_setups(textReaderClass &text);
    void read_machine(textReaderClass &text);
    void read_available_machine(textReaderClass &text);
    void read_calendar(textReaderClass &text);
    void read_sol(textReaderClass &text);
};

#endif
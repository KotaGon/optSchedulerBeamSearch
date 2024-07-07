#ifndef _TEXT_READER_H
#define _TEXT_READER_H

#include <fstream>
#include <string>
#include "utils.h"
using namespace std;

class textReaderClass
{
    private: 
        string path_;
        
        void read(string path)
        {
            ifstream file(path);
            string line;
            getline(file, line);
            
            //char delim[] = {','};
            vector<string> elements = split(line);

            while(getline(file, line))
            {
                elements = split(line);
                cells_.push_back(elements);
            }

            return;
        }

    public:

        textReaderClass() = default;
        textReaderClass(string path) : path_(path)
        {
            read(path);
        }

        vector<vector<string>> cells_;
        int getNumRow(){ return cells_.size(); }
        int getNumCol(){ return cells_[0].size(); }
        string &getCell(int i, int j) { return cells_[i][j];}
};

#endif 
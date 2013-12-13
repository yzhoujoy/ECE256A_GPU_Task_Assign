//
//  main.cpp
//  GPU_Task_Assign
//
//  Created by YING ZHOU on 12/9/13.
//  Copyright (c) 2013 YING ZHOU. All rights reserved.
//

#include "Buffer.h"
#include <map>

using namespace std;

#include <vector>
#include <algorithm>

class Task {
public:
    int age;
    double weight;
    Task(int _age, double _weight) {
        age = _age;
        weight = _weight;
    }
};

class Buffer{
public:
    const bool DEBUG_LOG = false;
//    const bool DEBUG_LOG = true;
    const bool BUFFER_SORT_SMALL_FIRST = true;
//    const bool BUFFER_SORT_SMALL_FIRST = false;
    int sum=0;
    int out_num;                               //number of tasks should be sent
    vector<Task> tasks;
    int num_drop;
    double output_arr[20];
    
    static bool comparisonSmallFirst(const Task& a, const Task& b) {
        if ((a.age>=3 && b.age>=3) || (a.age<3 && b.age<3)) {
            return a.weight < b.weight;
        } else {
            if (a.age>=3) { // a stays in buffer for too long, but b doesn't
                return true; // a goes first
            } else {
                return false; // b goes first
            }
            
        }
    }
    static bool comparisonBigFirst(const Task& a, const Task& b) {
        if ((a.age>=3 && b.age>=3) || (a.age<3 && b.age<3)) {
            return a.weight > b.weight;
        } else {
            if (a.age>=3) { // a stays in buffer for too long, but b doesn't
                return true; // a goes first
            } else {
                return false; // b goes first
            }
            
        }
    }
    
    /*provide number of empty slot*/
    int buffer_empty_slot(){
        int num_task=0;
        num_task=20-(int)tasks.size();         //number of empty items in buffer
        if (num_task<10) num_drop=10-num_task; 
        else {
            num_task=10;
            num_drop=0;
        
        }
        return num_task;                       //number of task can be store into buffer
    }
    
    /*store tasks from CPU to buffer*/
    void buffer_input(double *task, int size){
        for (int n=0; n<size ; n++) {
            tasks.push_back(Task(0, task[n]));
        }
    }
    
    /*caculate size of output array*/
    double * buffer_output(int num_output){
        out_num=0;               
        if (DEBUG_LOG) cout<<"used buffer size = "<<(int)tasks.size()<<endl;
        if (DEBUG_LOG) cout<<"number of ready GPUs = "<< num_output<<endl;
        
        //assign size of output array
        if (tasks.size()<num_output) out_num=(int)tasks.size();
        else out_num=num_output;
        
        // sort
        
        if (BUFFER_SORT_SMALL_FIRST) {
            sort(tasks.begin(), tasks.end(), comparisonSmallFirst);
        } else {
            sort(tasks.begin(), tasks.end(), comparisonBigFirst);
        }
        double *output_pt=output_arr;
        vector<Task>::iterator iter;
        for (int n=0; n<out_num; n++) {
            iter=tasks.begin();
            *output_pt=iter->weight;
            output_pt++;
            tasks.erase(iter);
        }
        
        if (DEBUG_LOG)  cout<<"size of output array = "<< out_num<<endl;
        for (int i=0;i<tasks.size();i++) {
            tasks[i].age++;
        }
        return output_arr;
    }
    
    int drop_cost(int num_of_drop){
        sum = num_of_drop*5;
        return sum;
    }
    
    
};
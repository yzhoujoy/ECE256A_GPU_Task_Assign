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

class Buffer{
public:
    int task_ctr=0;        
    int out_num;                               //number of tasks should be sent
    map<int, double> tasks;
    int num_drop;
    double output_arr[20];
    
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
    void buffer_input(double task[], int size){
        for (int n=0; n<size ; n++) {
            tasks[task_ctr]=*task;
            task++;
            task_ctr++;
        }
    }
    
    /*caculate size of output array*/
    double * buffer_output(int aval_gpu_cnt){
        out_num=0;               
        cout<<"used buffer size = "<<(int)tasks.size()<<endl;
        cout<<"number of ready GPUs = "<< aval_gpu_cnt<<endl;
        
        //assign size of output array
        if (tasks.size()<aval_gpu_cnt) out_num=(int)tasks.size();
        else out_num=aval_gpu_cnt;
        
        double *output_pt=output_arr;
        map<int, double>::iterator iter;
        for (int n=0; n<out_num; n++) {
            iter=tasks.begin();
            *output_pt=iter->second;
            output_pt++;
            tasks.erase(iter);
        }
        
        sort(output_arr,output_arr+out_num);
        reverse(output_arr, output_arr+out_num);
        
        cout<<"size of output array = "<< out_num<<endl;
        return output_arr;
    }
    
    int drop_cost(int num_of_drop){
        int sum = num_of_drop*5;
        return sum;
    }
    
    
};
//
//  main.cpp
//  GPU_Task_Assign
//
//  Created by YING ZHOU on 12/9/13.
//  Copyright (c) 2013 YING ZHOU. All rights reserved.
//

#include "GPU_Controller.h"
#include "GPU.h"
#include "GPU.cpp"
#include <map>
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;


class GPU_Controller{
    
public:
    map<int,int> GPU_map;     //map<GPU_id,flag> table for GPU status
    map<int,int> GPU_assign;  //map<GPU_id,task> talbe for GPU assignment
    int ready_ctr,sum_buf;
    
    static bool comparison_pair( pair<int, double> & m1,  pair<int, double> & m2) {
        return m1.second > m2.second;
    }
    
    /*initialization*/
    GPU_Controller(){
        ready_ctr=0;         //# of GPUs, which are ready for accepting task
        sum_buf=0;           //# of clock toggling
    }
    
    /*scan all GPUs before task assignment to mantain map<int, int> for GPU status*/
    int parse(GPU *gpu_16[]){
        ready_ctr=0;
        for (int c1=0; c1<16; c1+=2) {
            int c2 = c1+1;
            GPU_map[c1]=1;      // 1 for busy
            GPU_map[c2]=1;      // 1 for busy
            if (gpu_16[c1]->f_ready==1 && gpu_16[c2]->f_ready==1) {
                ready_ctr+=2;
                GPU_map[c1]=0;
                GPU_map[c2]=0;
            }else if (gpu_16[c1]->f_idle==1 && gpu_16[c2]->f_ready==1){
                gpu_16[c1]->passive_idle();
                gpu_16[c2]->passive_idle_();
            }else if (gpu_16[c1]->f_ready==1 && gpu_16[c2]->f_idle==1){
                gpu_16[c1]->passive_idle_();
                gpu_16[c2]->passive_idle();
            }else if (gpu_16[c1]->f_idle==1 && gpu_16[c2]->f_idle==1){
                gpu_16[c1]->passive_idle();
                gpu_16[c2]->passive_idle();
            }
            
        }
        return ready_ctr;            //inform to buffer how many GPUs are free
    }
    
    vector<pair<int, double>> gpu_sort(GPU *gpu_16[]){
        vector<pair<int, double>> GPU_pair_heat;
        for (int i=0; i<8; i++) {
            GPU_pair_heat.push_back(make_pair(i, gpu_16[2*i]->heat + gpu_16[2*i+1]->heat));
            cout<<"pair number: "<< GPU_pair_heat[i].first<<" heat value: "<< GPU_pair_heat[i].second<<endl;
        }
        sort(GPU_pair_heat.begin(), GPU_pair_heat.end(), comparison_pair);
        return GPU_pair_heat;
    }
    
    
    void assign_task_op(GPU *gpu_16[],double task[], int num_task){
        vector<pair<int, double>> GPU_pair_heat=gpu_sort(gpu_16);
        int task_cnt=0;
        for (int i=0; i<8; i++) {
            int pair_id= GPU_pair_heat[i].first;
            int gpu1_id=pair_id*2;
            int gpu2_id=pair_id*2+1;
            GPU *gpu1=gpu_16[pair_id*2];
            GPU *gpu2=gpu_16[pair_id*2+1];
            
            if (task_cnt<num_task-1) {
                if (GPU_map[gpu1_id]==0 && GPU_map[gpu2_id]==0) {
                    gpu1->update(task[task_cnt]);
                    gpu2->update(task[task_cnt+1]);
                    GPU_assign[pair_id*2]=1;
                    GPU_assign[pair_id*2+1]=1;
                    task_cnt+=2;
                }else{
                    GPU_assign[pair_id*2]=0;
                    GPU_assign[pair_id*2+1]=0;
                }
            }else{
                if (GPU_map[gpu1_id]==0 && GPU_map[gpu2_id]==0) {
                    gpu1->update(0.0);
                    gpu2->update(0.0);
                }
                GPU_assign[pair_id*2]=0;
                GPU_assign[pair_id*2+1]=0;
            }
            
        }
        
    }
    
    void assign_task(GPU *gpu_16[],double task[], int num_task){
        assign_task_op(gpu_16,task, num_task);
    }
    
    /*assign task to GPUs*/
//    void assign_task(GPU *gpu_16[],double task[], int num_task){
//        int counter =0;
//        int ctr_temp=0;    //recode number of tasks assigned
//        map<int,int>::iterator iter_assign = GPU_assign.begin();
//        //clean all GPU assignments in last dutation
//        for (iter_assign; iter_assign !=GPU_assign.end(); iter_assign++) {
//            iter_assign->second=0;
//        }
//        
//        map<int, int>::iterator iter_map = GPU_map.begin();
//        
//        
//        for (int i=0; i<16; i++) {
//            if (ctr_temp<num_task) {
//                if(iter_map->second==0){
//                    gpu_16[iter_map->first]->update(task[counter]);//update GPUs status table
//                    GPU_assign[iter_map->first]=1;                 //assign task for GPU
//                    iter_map++;
//                    ctr_temp++;
//                }else{
//                    iter_map++;
//                    GPU_assign[iter_map->first]=0;
//                }
//            }else{
//                GPU_assign[iter_map->first]=0;
//                iter_map++;
//            }
//            counter++;
//        }
//        
//    }
    
    int sum_per_iter(){
        int num_tog_buf=11;
        int r=0;
        for (int ctr=0; ctr<15; ctr=ctr+2) {
            if ((GPU_assign[ctr]==1)||(GPU_assign[ctr+1]==1)) {
                num_tog_buf +=3;
                r++;
            }
        }
        cout<<"number of +3"<<r<<endl;
        sum_buf = num_tog_buf;
        return sum_buf;
    }
    
};























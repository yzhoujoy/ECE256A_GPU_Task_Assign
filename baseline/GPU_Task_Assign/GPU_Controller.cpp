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
using namespace std;

class GPU_Controller{

public:
    //const bool DEBUG_LOG = false;
    const bool DEBUG_LOG = true;
    map<int,int> GPU_map;     //map<GPU_id,flag> table for GPU status
    map<int,int> GPU_assign;  //map<GPU_id,task> talbe for GPU assignment
    int ready_ctr,sum_buf;
    
    /*initialization*/
    GPU_Controller(){
        ready_ctr=0;         //# of GPUs, which are ready for accepting task
        sum_buf=0;           //# of clock toggling
    }
    /*scan all GPUs to mantain map<int, int> for GPU status*/
    int parse(GPU *gpu_16[]){
        ready_ctr=0;
        for (int c1=0; c1<16; c1+=2) {
            int c2 = c1+1;
            GPU_map[c1]=1;
            GPU_map[c2]=1;
            if (gpu_16[c1]->f_ready==1 && gpu_16[c2]->f_ready==1) {
                if (DEBUG_LOG) cout<<c1<<" : ready"<< c2<<" :ready"<<endl;
                ready_ctr+=2;
                GPU_map[c1]=0;
                GPU_map[c2]=0;
            }else if (gpu_16[c1]->f_idle==1 && gpu_16[c2]->f_ready==1){
                if (DEBUG_LOG) cout<<c1<<": idle"<<c2<<" : force idle"<<endl;
                gpu_16[c1]->passive_idle();
                gpu_16[c2]->passive_idle_();
            }else if (gpu_16[c1]->f_ready==1 && gpu_16[c2]->f_idle==1){
                if (DEBUG_LOG) cout<<c1<<": force idle"<<c2<<" : idle"<<endl;
                gpu_16[c1]->passive_idle_();
                gpu_16[c2]->passive_idle();
            }else if (gpu_16[c1]->f_idle==1 && gpu_16[c2]->f_idle==1){
                if (DEBUG_LOG) cout<<c1<<": idle"<<c2<<" : idle"<<endl;
                gpu_16[c1]->passive_idle();
                gpu_16[c2]->passive_idle();
            }
            
        }
        return ready_ctr;            //inform to buffer how many tasks should be sent
    }
    
    /*scan all GPUs to mantain map<int, int> for GPU status*/
    int parse_old(GPU *gpu_16[]){
        ready_ctr=0;
        for (int counter=0; counter<16; counter++) {
            if(gpu_16[counter]->f_ready==1){
                GPU_map[counter]=0;   //ready
                ready_ctr++;
            }else{
                GPU_map[counter]=1;  //passive_idle
                if (counter%2==0) {
                    if (gpu_16[counter+1]->f_idle==0) {
                        gpu_16[counter+1]->passive_idle_();
                    }else{
                        gpu_16[counter]->passive_idle();
                    }
                }else{
                    if (gpu_16[counter-1]->f_idle==0) {
                        gpu_16[counter-1]->passive_idle_();
                    }else{
                        gpu_16[counter]->passive_idle();
                    }
                }
                //gpu_16[counter]->passive_idle();
                
            }
                
        }
        return ready_ctr;            //inform to buffer how many tasks should be sent
    }
    
    
    
    
    /*assign task to GPUs*/
    void assign_task(GPU *gpu_16[],double task[], int num_task){
        int counter =0;
        int ctr_temp=0;    //recode number of tasks assigned
        map<int,int>::iterator iter_assign = GPU_assign.begin();
        //clean all GPU assignments in last dutation
        for (iter_assign; iter_assign !=GPU_assign.end(); iter_assign++) {
            iter_assign->second=0;
        }
        
        map<int, int>::iterator iter_map = GPU_map.begin();
        

        for (int i=0; i<16; i++) {
            if (ctr_temp<num_task) {
                if(iter_map->second==0){
                    gpu_16[iter_map->first]->update(task[counter]);//update GPUs status table
                    GPU_assign[iter_map->first]=1;                 //assign task for GPU
                    iter_map++;
                    ctr_temp++;
                }else{
                    //gpu_16[iter_map->first]->update(0.0);
                    iter_map++;
                    GPU_assign[iter_map->first]=0;
                }
            }else{
                if (iter_map->second==0) {
                    gpu_16[iter_map->first]->update(0.0);
                }
                GPU_assign[iter_map->first]=0;
                iter_map++;
            }
            counter++;
        }
        
    }
    
     int sum_per_iter(){
        int num_tog_buf=11;
        int r=0;
        for (int ctr=0; ctr<15; ctr=ctr+2) {
            if ((GPU_assign[ctr]==1)||(GPU_assign[ctr+1]==1)) {
                num_tog_buf +=3;
                r++;
            }
        }
        if (DEBUG_LOG) cout<<"number of +3"<<r<<endl;
        sum_buf = num_tog_buf;
        return sum_buf;
    }
    
};























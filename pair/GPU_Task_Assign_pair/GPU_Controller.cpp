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
    const bool DEBUG_LOG = false;
//    const bool DEBUG_LOG = true;
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
            GPU_map[c1]=1;      //busy
            GPU_map[c2]=1;      //busy
            if (gpu_16[c1]->f_ready==1 && gpu_16[c2]->f_ready==1) {
                if (DEBUG_LOG) cout<<c1<<" : ready"<< c2<<" :ready"<<endl;
                ready_ctr+=2;
                GPU_map[c1]=0;  //ready
                GPU_map[c2]=0;  //ready
            } 
        }
        return ready_ctr;            //inform to buffer how many tasks should be sent
    }
    
    /*assign task to GPUs*/
    void assign_task(GPU *gpu_16[],double task[], int num_task){
        for(int i=0;i<16;i++) {
            if (DEBUG_LOG) {
                cout<<"gpu_id: "<<i<<" k: "<<gpu_16[i]->k<<endl;
            }
        }
        //int counter =0;
        int ctr_temp=0;    //recode number of tasks assigned
        map<int,int>::iterator iter_assign = GPU_assign.begin();
        //clean all GPU assignments in last dutation
        for (iter_assign; iter_assign !=GPU_assign.end(); iter_assign++) {
            iter_assign->second=0;
        }
        
        map<int, int>::iterator iter_map = GPU_map.begin();
        

        for (int i=0; i<16; i++) {
            if (ctr_temp<num_task) {
                if(iter_map->second==0){          //available
                    gpu_16[iter_map->first]->update(task[ctr_temp]);//update GPUs status
                    GPU_assign[iter_map->first]=1;                  //assign task for GPU
                    ctr_temp++;
                }else{
                    //gpu_16[iter_map->first]->update(0.0);
                    gpu_16[iter_map->first]->update(0.0);//update GPUs status
                    GPU_assign[iter_map->first]=0;                  //not assigned task
                }
            }else{
                gpu_16[iter_map->first]->update(0.0);
                GPU_assign[iter_map->first]=0;
            }
            iter_map++;
        }
        
        // pair
        for (int self_id=0; self_id<16; self_id++) {
            int brother_id = (self_id/2 * 2 * 2 +1 ) - self_id;
            GPU* self_gpu = gpu_16[self_id];
            GPU* brother_gpu = gpu_16[brother_id];
            if (self_gpu->f_idle && brother_gpu->f_ready) {
                brother_gpu->passive_idle();
                if (DEBUG_LOG) {
                    cout<<"MADE BRO IDLE --- self: "<<self_id<<" bro: "<<brother_id<<endl;
                    cout<<"self.k: "<<self_gpu->k<<" bro.k: "<<brother_gpu->k<<endl;
                    if (self_gpu->k != brother_gpu->k) {
                        cout<<"not equal for idle brothers"<<self_gpu->k<<"."<<brother_gpu->k<<endl;
                    }
                }
                
            }
            
        }
        for (int self_id =0; self_id<16; self_id+=2) {
            int brother_id = self_id + 1;
            GPU* self_gpu = gpu_16[self_id];
            GPU* brother_gpu = gpu_16[brother_id];
            if (self_gpu->f_idle && brother_gpu->f_idle) {
                if (self_gpu->k != brother_gpu->k) {
                    cout<<"ERROR: not equal for idle brothers:"<<endl<<"self_id: "<<self_id<<", self_k:"<<self_gpu->k<<
                    "  bro_id:"<<brother_id<<" bro_k:"<<brother_gpu->k<<endl;
                }
            }
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
        if (DEBUG_LOG) cout<<"number of +3: "<<r<<endl;
        sum_buf = num_tog_buf;
        return sum_buf;
    }
    
};























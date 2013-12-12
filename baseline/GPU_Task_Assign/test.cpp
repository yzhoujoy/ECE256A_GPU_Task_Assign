//
//  main.cpp
//  GPU_Task_Assign
//
//  Created by YING ZHOU, TONG SHEN on 12/9/13.
//  Copyright (c) 2013 YING ZHOU. All rights reserved.
//

#include "test.h"
#include "GPU_Controller.h"
#include "GPU_Controller.cpp"
#include "Buffer.h"
#include "Buffer.cpp"
#include <map>
using namespace std;

const int DURATION_SIZE=20000;
const int TEST_VECTOR_ID=1;
// 0: all 1
// 1: random
// 2: all 0.75
// 3: all 0.5

int main(){
    srand(999);
    int sum_cost=0;
    GPU *gpus[16];
    for (int i=0; i<16; i++) {
        gpus[i]=new GPU();
    }
    GPU_Controller *controller= new GPU_Controller();
    double task_temp[10];
    double task_arr [DURATION_SIZE*10];
    int task_counter=0;
    int dropped_cycle_cnt=0;
    int dropped_cycle_cost=0;
    
    switch (TEST_VECTOR_ID) {
        case 0:// all 1's
            for (int n=0; n<DURATION_SIZE*10; n++) {
                task_arr[n]=1;
            }
            break;
        case 1:// random
            double time_arr[4];
            int num;
            time_arr[0]=0.25;
            time_arr[1]=0.5;
            time_arr[2]=0.75;
            time_arr[3]=1;
            for(int i=0;i<DURATION_SIZE*10;i++)
            {
                num = rand()%4;
                task_arr[i] = time_arr[num];
            }
            break;
        case 2:// all 0.75's
            for (int n=0; n<DURATION_SIZE*10; n++) {
                task_arr[n]=0.75;
            }
            break;
            
        case 3:// all 0.5's
            for (int n=0; n<DURATION_SIZE*10; n++) {
                task_arr[n]=0.75;
            }
            break;
            
        default: cout<<"err: wrong TEST_VECTOR_ID"<<endl;
    }
   
    
    Buffer *obj = new Buffer();
    int num_task=0;
    double *input_arr=NULL;
    
    
    for (int i=0; i<DURATION_SIZE; i++) {                    //for duration
        cout<<"$$$$$$$$$$$$$$$$$ "<<i<<endl;
        
        //empty buffer size
        num_task = obj->buffer_empty_slot();
        
        //assign CPU output task array
        if (num_task>=10) {
            int ctr_temp1=0;
            for (int n=task_counter; n<task_counter+10; n++) {
                task_temp[ctr_temp1]=task_arr[n];
                ctr_temp1++;
            }
            input_arr=task_temp;
            num_task=10;
            task_counter+=10;
        }else{
            int ctr_temp2=0;
            for (int n=task_counter; n<task_counter+num_task; n++) {
                task_temp[ctr_temp2]=task_arr[n];
                ctr_temp2++;
            }
            task_counter+=10;
            input_arr=task_temp;
        }
        
        //CPU input to Bbuffer
        cout<<"number of task to input to buffer: "<<num_task<<endl;
        obj->buffer_input(input_arr, num_task);
        //int cost_dp = obj->drop_cost(obj->num_drop);
        //cout<<"cost from dropped jobs: "<<cost_dp<<endl;
        
        //check ready number of GPUs
        int ready_ctr=controller->parse(gpus);
        cout<<"number of ready GPUs: "<<ready_ctr<<endl;

        //buffer output array for GPUs
        double *output_arr = obj->buffer_output(ready_ctr);        
        
        //assign tasks to GPUs
        controller->assign_task(gpus, output_arr,obj->out_num);
        map<int,int>::iterator iter_contr = controller->GPU_assign.begin();

        for (int ctr=0; ctr<16; ctr++) {
            cout<<"gpu_id: "<< ctr <<" idle: "<< gpus[ctr]->f_idle<<" ready: "<<gpus[ctr]->f_ready<<" hot: "<<gpus[ctr]->hot<<" avg: "<<gpus[ctr]->avg<<endl;

        }
        
        //cost from dropped tasks
        int cost_dp = obj->drop_cost(obj->num_drop);
        if (cost_dp>0) {
            dropped_cycle_cnt++;
            dropped_cycle_cost += cost_dp;
        }
        cout<<"cost from dropped jobs: "<<cost_dp<<endl;
        //cost from buffer toggling
        int cost_tg = controller->sum_per_iter();
        cout<<"cost of toggling buffer ="<<cost_tg<<endl;
        //sum of cost
        sum_cost+=cost_dp+cost_tg;
        cout<<"sum of cost = "<<sum_cost<<endl;
        
    }
    cout<<"Dropped Cycle: "<< dropped_cycle_cnt<<endl;
    cout<<"Dropped Cost: "<< dropped_cycle_cost<<endl;

    
    return 0;
 
}
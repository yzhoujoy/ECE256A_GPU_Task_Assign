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
#include <string>
using namespace std;

//const int DURATION_SIZE=20;
//const int TEST_VECTOR_ID=1;

//const bool DEBUG_LOG = false;
const bool DEBUG_LOG = true;
// 0: all 1
// 1: random
// 2: all 0.75
// 3: all 0.5
string TEST_VECTOR_ID_NAMES[] = {"all 1", "random", "all 0.75", "all 0.5"};
//int main(){
int testCase(int TEST_VECTOR_ID, int DURATION_SIZE, int DATA_SEGMENT_SIZE) {
    
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
                task_arr[n]=0.5;
            }
            break;
            
        default: if (DEBUG_LOG) cout<<"err: wrong TEST_VECTOR_ID"<<endl;
    }
    
    
    Buffer *obj = new Buffer();
    int num_task=0;
    double *input_arr=NULL;
    
    
    for (int i=0; i<DURATION_SIZE; i++) {                    //for duration
        if (DEBUG_LOG) cout<<"$$$$$$$$$$$$$$$$$ "<<i<<endl;
        
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
        if (DEBUG_LOG) cout<<"number of task to input to buffer: "<<num_task<<endl;
        obj->buffer_input(input_arr, num_task);
        //int cost_dp = obj->drop_cost(obj->num_drop);
        //cout<<"cost from dropped jobs: "<<cost_dp<<endl;
        
        //check ready number of GPUs
        int ready_ctr=controller->parse(gpus);
        if (DEBUG_LOG) cout<<"number of ready GPUs: "<<ready_ctr<<endl;
        
        //buffer output array for GPUs
        double *output_arr = obj->buffer_output(ready_ctr);
        
        //assign tasks to GPUs
        controller->assign_task(gpus, output_arr,obj->out_num);
        
        for (int ctr=0; ctr<16; ctr++) {
            if (DEBUG_LOG) cout<<"gpu_id: "<< ctr <<" counter: "<<gpus[ctr]->counter<<" idle: "<< gpus[ctr]->f_idle<<" ready: "<<gpus[ctr]->f_ready<<" hot: "<<gpus[ctr]->hot<<" avg: "<<gpus[ctr]->avg<<" heat: "<<gpus[ctr]->getHeat()<<endl;
            
        }
        
        //cost from dropped tasks
        int cost_dp = obj->drop_cost(obj->num_drop);
        if (cost_dp>0) {
            dropped_cycle_cnt++;
            dropped_cycle_cost += cost_dp;
            if (DEBUG_LOG) {
                if (i>1000) cout<<"DROP HAPPENED AT I > 1000: "<<i<<endl;
                else cout<<"SMALL DROP HAPPENED AT I <=1000: "<<i<<endl;
            }
        }
        if (DEBUG_LOG) cout<<"cost from dropped jobs: "<<cost_dp<<endl;
        //cost from buffer toggling
        int cost_tg = controller->sum_per_iter();
        if (DEBUG_LOG) cout<<"cost of toggling buffer ="<<cost_tg<<endl;
        //sum of cost
        sum_cost+=cost_dp+cost_tg;
        if (DEBUG_LOG) cout<<"sum of cost = "<<sum_cost<<endl;
        if ((i+1)%DATA_SEGMENT_SIZE == 0) {
            cout << TEST_VECTOR_ID_NAMES[TEST_VECTOR_ID] << ","<<(i+1)<<","<<dropped_cycle_cnt << ","<< dropped_cycle_cost << ","<<sum_cost<<endl;
        }
    }
    if (DEBUG_LOG) cout<<"Dropped Cycle: "<< dropped_cycle_cnt<<endl;
    if (DEBUG_LOG) cout<<"Dropped Cost: "<< dropped_cycle_cost<<endl;
    
    //cout << TEST_VECTOR_ID_NAMES[TEST_VECTOR_ID] << ", "<<DURATION_SIZE<<", dropped_cycles: "<<dropped_cycle_cnt << ", dropped_cost: "<< dropped_cycle_cost << ", total cost: "<<sum_cost<<endl;
    
    
    return 0;
    
}

int main() {
    // debug
        testCase(3, 50, 100000);
        return 0;
    // debug end
    
    srand(999);
    cout<<"data_type,duration_cnt,dropped_cycles,dropped_cost,totoal_cost"<<endl;
    for (int data_id = 0; data_id<4; data_id++) {
        testCase(data_id, 20000, 1000);
    }
}
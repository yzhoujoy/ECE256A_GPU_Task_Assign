//
//  main.cpp
//  GPU_Task_Assign
//
//  Created by YING ZHOU on 12/9/13.
//  Copyright (c) 2013 YING ZHOU. All rights reserved.
//

#include "GPU.h"
using namespace std;

class GPU {
    
public:
    int hot,k,counter,f_idle,f_ready;
    double array[10];
    double avg =0;                    //test thermal constrain violation
    double heat=0;                    //used in sort GPUs
    
    /*initialization*/
    GPU(){
        hot = 0;
        k=3;
        counter=0;
        f_idle=0;
        f_ready=1;
        for(int i=0; i<10;i++)        //task array initialization
            array[i] = 0.0;
    }
    
    /*update task array and check thermal constrain*/
    void update(double time){
        cout<<"in update"<<endl;
        double sum=0;
        int ctr = counter%10;
        avg =0;
        heat=0;
        array[ctr] = time;       //store task into array
        
        //caculate for heat
        for (int n=0; n<10; n++){
            heat+=array[n];
        }
        heat=(double)heat/10;
        
        
        if(counter>=9)
        {
            for(int i=0; i<10; i++)
            {
                sum +=array[i];
                
            }
            
        }
        avg = sum/10;
        counter++;
        
        cout<<"array index in GPU = "<<counter<<endl;
        if(avg > 0.5) {
            passive_idle();
            counter=0;
        }
        else ready();
    }
    
    void passive_idle(){
        f_idle=1;      //force to idle
        f_ready=0;     //not ready to work
        hot=1;
        cout<<"k = "<<k<<endl;
        if(k>0)k--;
        else {
            k=3;
            f_idle=0;
            f_ready=1;
            cout<<"go to ready!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
            for (int i=0; i<10; i++) {
                array[i]=0;
            }
            ready();
        }
    }
    
    void passive_idle_(){
        k=3;
        f_idle=1;      //force to idle
        f_ready=0;     //not ready to work
        hot=1;
        k-=2;
        array[(counter++)%10]=0;
        array[(counter++)%10]=0;
        array[(counter++)%10]=0;
        cout<<"k_ = "<<k<<endl;
    }
    
    void ready(){
        f_ready=1;     //ready to accept task
        f_idle=0;      //not idle
        hot=0;
    }
    
};
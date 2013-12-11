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
    int hot,k,counter,f_idle,f_ready, f_option;//time;
    double array[10];
    
    /*initialization*/
    GPU(){
        f_option=0;
        hot = 0;
        k=3;
        counter=0;
        f_idle=0;
        f_ready=1;
        //time=0;
        for(int i=0; i<10;i++)
        {
            array[i] = 0.0;
        }
    }
   
    /*update task array and check thermal constrain*/
    void update(double time){
        cout<<"in update"<<endl;
        double sum=0;
        int ctr = counter%10;
        double avg =0;
        array[ctr] = time;

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
        }else if (avg > 0.4) {
            option_idle();
        }
        else ready();
    }

    /*idle caused by 0.5>=avg>0.4*/
    void option_idle(){
        f_option=1;
        f_idle=1;      //force to idle
        f_ready=0;     //not ready to work
        k=0;
        counter++;
        array[counter%10] = 0;
        
    }
    
    /*idle caused by termal constrain violation*/
    void passive_idle(){
        f_idle=1;      //force to idle
        f_ready=0;     //not ready to work
        hot=1;
        //cout<<"k = "<<k<<endl;
        if(k>0)k--;
        else {
            k=3;
            //f_option=0;
            f_idle=0;
            f_ready=1;
            //cout<<"go to ready!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
            if (f_option==0) {
                for (int i=0; i<10; i++) {
                    array[i]=0;
                }
            }else
                f_option=0;
            
            ready();
        }
    }
    
    /*force to be idle by pair GPU*/
    void passive_idle_(){
        k=3;
        f_idle=1;      //force to idle
        f_ready=0;     //not ready to work
        hot=1;
        k-=2;
        cout<<"k_p = "<<k<<endl;
    }
    
    /*force to be idle by pair GPU*/
    void passive_option_idle(){
        f_option=0;
        f_idle=0;
        f_ready=1;
        k=3;
        counter++;
        array[counter%10]=0;
        

    }
    
    void ready(){
        f_option=0;
        f_ready=1;     //ready to accept task
        f_idle=0;      //not idle
        k=3;
        hot=0;
    }

};
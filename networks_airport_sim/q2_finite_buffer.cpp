//single server, finite buffer

#include<bits/stdc++.h>
#include<algorithm>
#include<iostream>
#include<vector>
#include<queue>
#include<random>
#include<ctime>
#include<cmath>
#include<unistd.h>
#include <chrono>
#include <thread>
using namespace std;

struct Passenger{
    int id;
    double arrival_time;

    //constructor
    Passenger(int _id, double _arrivalTime) : id(_id), arrival_time(_arrivalTime) {}
};

double GeneratePoissonRandom(double mean)
{
    static default_random_engine generator(time(nullptr)); //random no generator with seed set to current time
    poisson_distribution<int> distribution(mean); //poisson distribution object with given mean
    return static_cast<double>(distribution(generator)); // generate random poisson object and cast to double
}

int main(int argc, char* argv[])
{ 
    if(argc<4) {cout<<"Usage: <arrival rate> <service rate> <buffer size>\n"; return 0;}
    double arrivalRate = (double) atoi(argv[1]);
    double serviceRate = (double) atoi(argv[2]);
    int k = atoi(argv[3]);
    double inter_arrival_mean = double(1)/arrivalRate;
    double service_time_mean = double(1)/serviceRate;

    queue<Passenger> initial_queue;
    double t1=0,t2=0; //t1= arrival timer, t2= service end timer
    int passenger_id=1;

    double wait_time_sum=0;
    double service_time_sum=0;
    double server_idle_time=0;
    vector<double> arrival_times; //into waiting queue
    vector<double> departure_times; //from waiting queue
    int no_dropped=0;
    
    while(true)
    {
        double interArrivalTime = GeneratePoissonRandom(inter_arrival_mean);
        t1 += interArrivalTime;
        if(t1>120) { break; }  // Check if we need to stop the simulation (e.g, after a certain duration)

        // check if finite buffer is full
        vector<double>::iterator upper= upper_bound(departure_times.begin(),departure_times.end(),t1);
        int no_in_queue = departure_times.end()-upper;
        if(no_in_queue>=k) 
        {
            Passenger passenger(passenger_id, t1);
            cout<<"Buffer full. Passenger ID "<<passenger_id<<" dropped.\nArrival time: "<<t1<<endl;
            passenger_id++;
            no_dropped++;
            continue;
        }
        
        arrival_times.push_back(t1); //records only those passengers who actually enter airport
        // Create a new passenger and add to the queue
        Passenger passenger(passenger_id, t1);
        initial_queue.push(passenger);
        passenger_id++;
        

        // If the queue is not empty and the serving station is available, serve the passenger
        if (!initial_queue.empty())
        {
            if(initial_queue.front().id==1) {t2 = t1;} //service begins at the first passenger
            if (initial_queue.front().arrival_time <= t2)
            {
                // Generate service time using Poisson distribution
                double serviceTime = GeneratePoissonRandom(service_time_mean);
                double waiting_time = (t2 - initial_queue.front().arrival_time);
                wait_time_sum += waiting_time;
                service_time_sum += serviceTime;
                departure_times.push_back(t2); //departure from waiting queue
                t2 += serviceTime;

                // Serve the passenger and remove them from the queue
                cout<<"Passenger "<<initial_queue.front().id<<" Arrived at: "<<initial_queue.front().arrival_time<<" Served by: "<<t2<<"\n";
                cout<<"Waiting time= "<<waiting_time<<" Service time= "<<serviceTime<<" Total queueing time= "<<waiting_time+serviceTime<<endl;
                initial_queue.pop();
                
            }
            else //if arrival time is later than service timer, server idle for (arrival time-t2)
            {
                server_idle_time += (initial_queue.front().arrival_time - t2);
                t2 = initial_queue.front().arrival_time;
            }
        }
        
        //this_thread::sleep_for(chrono::milliseconds(300));
    }
    
    double total_time = max(t1,t2); 
    
    cout<<"\n--------------------------------Report------------------------------------\n";
    cout<<"\nAverage waiting time= "<<wait_time_sum/double(passenger_id)<<" min"<<endl;
    cout<<"Average service time= "<<service_time_sum/double(passenger_id)<<" min"<<endl;
    cout<<"Average queueing time= "<<(wait_time_sum+service_time_sum)/double(passenger_id)<<" min"<<endl;

    double sys_util = (total_time-server_idle_time)/total_time;
    cout<<"System utilization= "<<sys_util*100<<"%"<<endl;

    cout<<"\nFinite buffer length= "<<k<<endl;
    cout<<"Number of passengers dropped= "<<no_dropped<<"/"<<passenger_id<<endl;
    double dropped = double(no_dropped)/double(passenger_id);
    cout<<"% of passengers dropped= "<<dropped*100<<"%"<<endl;

    return 0;
}
// multi server with no buffers and finite queue

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
    if(argc<5) {cout<<"Usage: <arrival rate> <service rate> <buffer size> <no of servers>\n"; return 0;}
    double arrivalRate = (double) atoi(argv[1]);
    double serviceRate = (double) atoi(argv[2]);
    int k = atoi(argv[3]);
    int m = atoi(argv[4]);
    double inter_arrival_mean = double(1)/arrivalRate;
    double service_time_mean = double(1)/serviceRate;

    queue<Passenger> initial_queue;
    double t1=0,t2=0; //t1= arrival timer, t2= service end timer
    int passenger_id=1;

    
    int no_of_servers = m;
    multiset<pair<double,double>> Servers;
    double wait_time_sum=0;
    double service_time_sum[no_of_servers];
    double server_idle_time[no_of_servers];
    int  no_of_users[no_of_servers];
    for (int i = 0; i < no_of_servers; i++)
    {
        Servers.insert(make_pair(0,i));
        service_time_sum[i] = 0;
        server_idle_time[i] = 0;
        no_of_users[i] = 0;
    }
    
    vector<double> arrival_times; //into waiting queue
    vector<double> departure_times; //from waiting queue
    int no_dropped=0;
    
    while(true)
    {
        double interArrivalTime = GeneratePoissonRandom(inter_arrival_mean);
        t1 += interArrivalTime;
        if(t1>120) // Check if we need to stop the simulation (e.g, after a certain duration)
        { break; }  

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
            auto it = Servers.begin();
            while(it != Servers.end())
            {
                if((*it).first <= initial_queue.front().arrival_time)break;
                it++;
            }
            if (it == Servers.end())
            {
                // Generate service time using Poisson distribution
                double serviceTime = GeneratePoissonRandom(service_time_mean);
                double waiting_time = max(0.00,((*Servers.begin()).first - initial_queue.front().arrival_time));
                wait_time_sum += waiting_time;
                service_time_sum[(int)(*Servers.begin()).second] += serviceTime;
                no_of_users[(int)(*Servers.begin()).second]++;
                departure_times.push_back((*Servers.begin()).first); //departure from waiting queue
                Servers.insert(make_pair((*Servers.begin()).first + serviceTime,(*Servers.begin()).second));
                Servers.erase(Servers.begin());

                // Serve the passenger and remove them from the queue
                cout<<"Passenger "<<initial_queue.front().id<<" Arrived at: "<<initial_queue.front().arrival_time;
                cout<<", Waiting time= "<<waiting_time<<" Service time= "<<serviceTime<<" Total queueing time= "<<waiting_time+serviceTime<<endl;
                initial_queue.pop();
                
            }
            else //if arrival time is later than service timer, server idle for (arrival time-t2)
            {
                server_idle_time[(int)(*it).second] += (initial_queue.front().arrival_time - (*it).first);
                double new_arrival = (*it).first;
                double new_server = (*it).second;
                double serviceTime = GeneratePoissonRandom(service_time_mean);
                service_time_sum[(int)(*it).second] += serviceTime;
                no_of_users[(int)(*it).second]++;
                Servers.erase(it);
                Servers.insert(make_pair(initial_queue.front().arrival_time+ serviceTime,new_server));
                t2 = initial_queue.front().arrival_time;
                // Serve the passenger and remove them from the queue
                cout<<"Passenger "<<initial_queue.front().id<<" Arrived at: "<<initial_queue.front().arrival_time;
                cout<<"Server idle time= "<<server_idle_time[(int)(*it).second]<<" Service time= "<<serviceTime<<" Total queueing time= "<<serviceTime<<endl;
                initial_queue.pop();
            }
        }
        auto it = Servers.begin();
        while (it != Servers.end())
        {
            cout<<"Server no. "<<(*it).second<<" "<<"Service end time: "<<(*it).first<<endl;
            it++;
        }
    } 
    
    cout<<"\n--------------------------------Report------------------------------------\n";
    cout<<"\nAverage waiting time= "<<wait_time_sum/double(passenger_id)<<" min"<<endl;
    for (int i = 0; i < no_of_servers; i++)
    {
        cout<<"\nServer "<<i<<endl;
        cout<<"Avg service time= "<<service_time_sum[i]/no_of_users[i]<<" min, ";        
        cout<<"Avg queueing time= "<<(server_idle_time[i]+service_time_sum[i])/no_of_users[i]<<" min, ";
        cout<<"System utilization= "<<(service_time_sum[i])/((server_idle_time[i]+service_time_sum[i]))*100<<"%"<<endl;
    }

    cout<<"\nFinite buffer length= "<<k<<endl;
    cout<<"Number of passengers dropped= "<<no_dropped<<"/"<<passenger_id<<endl;
    double dropped = double(no_dropped)/double(passenger_id);
    cout<<"% of passengers dropped= "<<dropped*100<<"%"<<endl;

    return 0;
}
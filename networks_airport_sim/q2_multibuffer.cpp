// multi server, multi buffer, finite queue

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
    
    int buffer_size = k;
    int no_of_servers = m;
    
    double wait_time_sum=0;
    double service_time_sum[no_of_servers];
    double server_idle_time[no_of_servers];
    double server_queue_waiting[no_of_servers];
    int  no_of_users[no_of_servers];
    vector<multiset<double>> server_deliver_times(no_of_servers);
    for (int i = 0; i < no_of_servers; i++)
    {
        for (int j = 0; j < buffer_size; j++)
        {
            server_deliver_times[i].insert(0);
        }
        
        service_time_sum[i] = 0;
        server_idle_time[i] = 0;
        server_queue_waiting[i] = 0;
        no_of_users[i] = 0;
    }
    
    vector<double> arrival_times; //into waiting queue
    vector<double> departure_times; //from waiting queue
    int no_dropped=0;
    
    while(true)
    {
        double interArrivalTime = GeneratePoissonRandom(inter_arrival_mean);
        t1 += interArrivalTime;
        if(t1>120) 
        {break;}  // Check if we need to stop the simulation (e.g, after a certain duration)

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
            double min_server = -1;
            double min_time = INFINITY;
            for (int i = 0; i < no_of_servers; i++)
            {
                if(*server_deliver_times[i].begin() < min_time)
                {
                    min_server = i;
                    min_time = *server_deliver_times[i].begin();
                }
            }
            
            if (initial_queue.front().arrival_time < min_time)
            {
                // Generate service time using Poisson distribution
                double serviceTime = GeneratePoissonRandom(service_time_mean);
                double waiting_time = max(0.00,(min_time - initial_queue.front().arrival_time));
                double current_processing = *(--(server_deliver_times[(int)min_server].end()));
                wait_time_sum += waiting_time;
                server_queue_waiting[(int)min_server] += current_processing - min_time;
                service_time_sum[(int)(min_server)] += serviceTime;
                no_of_users[(int)(min_server)]++;
                departure_times.push_back(min_time); //departure from waiting queue
                server_deliver_times[min_server].insert(current_processing + serviceTime);
                server_deliver_times[min_server].erase(server_deliver_times[min_server].begin());

                // Serve the passenger and remove them from the queue
                cout<<"Passenger "<<initial_queue.front().id<<" Arrived at: "<<initial_queue.front().arrival_time << " Served by: " << min_server << endl;
                cout<<"Waiting time= "<<waiting_time<<" Service time= "<<serviceTime<<" Total queueing time= "<<waiting_time+current_processing+serviceTime<<endl;
                initial_queue.pop();
                
            }
            else //if arrival time is later than service timer, server idle for (arrival time-t2)
            {
                double current_processing = *(--(server_deliver_times[(int)min_server].end())); 
                server_idle_time[(int)(min_server)] += max(0.00,(initial_queue.front().arrival_time - current_processing));
                double serviceTime = GeneratePoissonRandom(service_time_mean);
                service_time_sum[(int)(min_server)] += serviceTime;
                server_queue_waiting[(int)(min_server)] += max(0.00,(current_processing - initial_queue.front().arrival_time));
                no_of_users[(int)(min_server)]++;
                server_deliver_times[(int)min_server].erase(server_deliver_times[(int)min_server].begin());
                server_deliver_times[(int)min_server].insert(initial_queue.front().arrival_time + serviceTime);
                t2 = initial_queue.front().arrival_time;
                // Serve the passenger and remove them from the queue
                cout<<"Passenger "<<initial_queue.front().id<<" Arrived at: "<<initial_queue.front().arrival_time << " Served by: " << min_server << endl;
                cout<<"Server idle time= "<<server_idle_time[(int)(min_server)]<<" Service time= "<<serviceTime<<" Total queueing time= "<<serviceTime<<endl;
                initial_queue.pop();
            }
        }
        for (auto K : server_deliver_times)
        {
            cout << *(K.begin()) << " ";
        }
        cout << endl;
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
//single server, infinte buffer

#include<iostream>
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
    double waiting_time;
    double service_time;

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
    if(argc<3) {cout<<"Usage: <arrival rate> <service rate>\n"; return 0;}
    double arrivalRate = (double) atoi(argv[1]);
    double serviceRate = (double) atoi(argv[2]);
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
    
    while(true)
    {
        double interArrivalTime = GeneratePoissonRandom(inter_arrival_mean);
        t1 += interArrivalTime;
        arrival_times.push_back(t1);

        // Check if we need to stop the simulation (e.g, after a certain duration)
        if(t1>120) { break; }

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

    int q_len=0;
    int duration=0;
    int q_sum=0;
    int i=0,j=0;
    for(int t=0;t<=int(total_time);t++)
    {
        if(t==arrival_times[i] || t==departure_times[j])
        {
            q_sum += (q_len*duration);
            duration = 0;
        }
        if(t!=arrival_times[i] && t!=departure_times[j])
        {
            duration++;
        }

        while(t==arrival_times[i])
        {
            q_len++;
            i++;
        }
        while(t==departure_times[j])
        {
            q_len--;
            j++;
        }  
    }
    cout<<"Average queue length= "<<q_sum/int(total_time)<<endl;

    double sys_util = (total_time-server_idle_time)/total_time;
    cout<<"System utilization= "<<sys_util*100<<"%"<<endl;

    return 0;
}
#include <bits/stdc++.h>

using namespace std;

class Website
{
public:
    Website(int id, int owner, double bandwidth, double processing_power) : id(id), owner(owner), bandwidth(bandwidth), processing_power(processing_power) {}

    int getID() const
    {
        return id;
    }

    double getBandwidth() const
    {
        return bandwidth;
    }

    double getProcessingPower() const
    {
        return processing_power;
    }

    list<int> getRequestQueue() const
    {
        return request_queue;
    }

    list<int> getWeightQueue() const
    {
        return weight_queue;
    }

    vector<int> getArrivalTime()
    {
        return arrival_times;
    }

    void updateWeightQueue(list<int> &q)
    {
        weight_queue.assign(q.begin(), q.end());
    }

    void enqueueRequest(int request_id, double processing_time, int arrival_time)
    {
        request_queue.push_back(request_id);
        weight_queue.push_back(processing_time);
        arrival_times.push_back(arrival_time);
    }

    void dequeueRequest()
    {
        request_queue.pop_front();
        weight_queue.pop_front();
    }

private:
    int id;
    int owner;
    int arrival_time;
    double bandwidth;
    double processing_power;
    list<int> request_queue;
    list<int> weight_queue;
    vector<int> arrival_times;
};

class HttpRequest
{
public:
    HttpRequest(int id, int website_id, double processing_time, int arrival_time) : id(id), website_id(website_id), processing_time(processing_time), arrival_time(arrival_time) {}

    int getID() const
    {
        return id;
    }

    int getWebsiteID() const
    {
        return website_id;
    }

    double getProcessingTime() const
    {
        return processing_time;
    }
    
    int getArrivalTime() const
    {
        return arrival_time;
    }

private:
    int id;
    int website_id;
    double processing_time;
    int arrival_time;
};

class LoadBalancer
{
public:
    void addWebsite(int website_id, int owner_id, double bandwidth, double processing_power)
    {
        websites.push_back(Website(website_id, owner_id, bandwidth, processing_power));
    }

    void enqueueRequest(const HttpRequest &http_request)
    {
        int website_id = http_request.getWebsiteID();
        for (auto &website : websites)
        {
            if (website.getID() == website_id)
            {
                website.enqueueRequest(http_request.getID(), http_request.getProcessingTime(), http_request.getArrivalTime());
                break;
            }
        }
    }

    void dequeueRequest(int total_power)
    {
        vector<pair<int, double>> weights;
        double total = 0;
        for (auto &website : websites)
        {
            double w = website.getBandwidth() + website.getProcessingPower();
            total += w;
            int id = website.getID();
            weights.push_back({w, id});
        }
        unordered_map<int, double> wt;
        for (auto &website : websites)
        {
            int id = website.getID();
            double w = website.getBandwidth() + website.getProcessingPower();
            wt[id] = (w / total) * total_power;
        }
        vector<pair<int, int>> virtual_time;
        unordered_map<int, double> processing_times_temp;
        unordered_map<int, double> real_processing_times;
        unordered_map<int, double> arrival_times;
        for (auto &website : websites)
        {
            list<int> reqs = website.getRequestQueue();
            list<int> times = website.getWeightQueue();
            vector<int> arrival_times_temp = website.getArrivalTime();
            int id = website.getID();
            double t = 0;
            auto it_time = times.begin();
            for (auto it_req = reqs.begin(); it_req != reqs.end(); ++it_req, ++it_time)
            {
               t = max(t, (double)arrival_times_temp[distance(reqs.begin(), it_req)]) + (*it_time / wt[id]);

                processing_times_temp[*it_req] = t;
                real_processing_times[*it_req] = t;
            }
        }
        vector<pair<double, int>> processing_times;
        for (auto i : processing_times_temp)
        {
            int f = i.first;
            double s = i.second;
            processing_times.push_back({s, f});
        }
        sort(processing_times.begin(), processing_times.end());
        for (auto i : processing_times)
        {
            int id = i.second;
            cout <<"\033[1;33m"<< "Request with request ID " << id << " is dequeued at time " << real_processing_times[id] << "\033[0m\n";
        }
    }

public:
    vector<Website> websites;
};

int main()
{
    LoadBalancer load_balancer;
    int choice;
    int websites;
    int requests;
    cout << "\033[1;37mEnter the number of websites : \033[0m";
    cin >> websites;
    for (int i = 0; i < websites; i++)
    {
        int website_id, owner_id;
        double bandwidth, processing_power;
        cout << "\033[1;37mEnter website ID, owner ID, bandwidth, and processing power: \033[0m";
        cin >> website_id >> owner_id >> bandwidth >> processing_power;
        load_balancer.addWebsite(website_id, owner_id, bandwidth, processing_power);
    }

    cout << "\033[1;37mEnter the number of requests : \033[0m";
    cin >> requests;
    for (int i = 0; i < requests; i++)
    {
        int request_id, website_id;
        double processing_time, arrival_time;
        cout<<"\033[1;34m" << "Enter request ID, website ID, processing time and arrival time: "<<"\033[0m";
        cin >> request_id >> website_id >> processing_time >> arrival_time;
        load_balancer.enqueueRequest(HttpRequest(request_id, website_id, processing_time, arrival_time));
    }
    int total_power;
    cout << "\033[1;37mEnter the total power of the server : \033[0m";
    cin >> total_power;
    cout << "Dequeuing the requests...\n";
    load_balancer.dequeueRequest(total_power);

    return 0;
}


class UndergroundSystem {
public:
    unordered_map<int,pair<string,int>> checkInMp;
    unordered_map<string,pair<int,int>> checkOutMp;
    UndergroundSystem() {}
    void checkIn(int id, string stationName, int t) {
        checkInMp[id] = {stationName, t};
    }
    void checkOut(int id, string stationName, int t) {
        auto elem = checkInMp[id];
        string route = elem.first + "_" + stationName;
        checkOutMp[route].first+=(t-elem.second);
        checkOutMp[route].second+=1;
        checkInMp.erase(id);
    }
    
    double getAverageTime(string startStation, string endStation) {
        string route = startStation + "_" + endStation;
        return 1.0 * checkOutMp[route].first / checkOutMp[route].second;
    }
};

/**
 * Your UndergroundSystem object will be instantiated and called as such:
 * UndergroundSystem* obj = new UndergroundSystem();
 * obj->checkIn(id,stationName,t);
 * obj->checkOut(id,stationName,t);
 * double param_3 = obj->getAverageTime(startStation,endStation);
 */
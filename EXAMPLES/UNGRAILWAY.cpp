// LeetCode 1396. Design Underground System
// Every operation is O(1) on average (hash map lookups).
class UndergroundSystem {
public:
    unordered_map<int,pair<string,int>> checkInMp;     // id -> {start station, check-in time}
    unordered_map<string,pair<int,int>> checkOutMp;    // "start_end" -> {total travel time, trip count}
    UndergroundSystem() {}
    void checkIn(int id, string stationName, int t) {
        checkInMp[id] = {stationName, t};
    }
    void checkOut(int id, string stationName, int t) {
        auto& elem = checkInMp[id];
        string route = elem.first + "_" + stationName;
        checkOutMp[route].first+=(t-elem.second);
        checkOutMp[route].second+=1;
        checkInMp.erase(id);   // the trip is finished, so the check-in is no longer needed
    }

    double getAverageTime(string startStation, string endStation) {
        string route = startStation + "_" + endStation;
        return 1.0 * checkOutMp[route].first / checkOutMp[route].second;   // 1.0 * avoids integer division
    }
};

/**
 * Your UndergroundSystem object will be instantiated and called as such:
 * UndergroundSystem* obj = new UndergroundSystem();
 * obj->checkIn(id,stationName,t);
 * obj->checkOut(id,stationName,t);
 * double param_3 = obj->getAverageTime(startStation,endStation);
 */

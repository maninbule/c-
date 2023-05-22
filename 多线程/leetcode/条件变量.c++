class H2O {
private:
    std::condition_variable cv;
    std::mutex mtx;
    int state = 0;
    int transitions[6][2] = {{1,2},{3,4},{5,-1},{-1,0},{0,-1},{0,-1}};
public:
    H2O() {
        
    }

    void hydrogen(function<void()> releaseHydrogen) {
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[this]{
            return transitions[state][0] != -1;
        });
        state = transitions[state][0];
        releaseHydrogen();
        if(state == 0) cv.notify_all();
    }

    void oxygen(function<void()> releaseOxygen) {
        // releaseOxygen() outputs "O". Do not change or remove this line.
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[this]{
            return transitions[state][1] != -1;
        });
        state = transitions[state][1];
        releaseOxygen();
        if(state == 0) cv.notify_all();
    }
};

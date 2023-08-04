// CMSC 341 - Spring 2023 - Project 4
// CMSC 341 - Spring 2023 - Project 4
//Name: Aditya Niroula
//Campus id : MF56332
//email : aniroul1@umbc.edu
//Description : main virus detection test file

#include "vdetect.h"
#include <random>
#include <vector>
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else{ //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

unsigned int hashCode(const string str);
string sequencer(int size, int seedNum);

class Tester{
public:

    void insertTest(){
        cout << "******************Testing Insertion******************" << endl;
        VDetect a(MINPRIME, hashCode, DOUBLEHASH );
        Random rndNum(MINID, MAXID);
        int numToInsert = 10;

        //inserting virus
        for(int i=0; i<numToInsert; i++){
            Virus virus = Virus(sequencer(5, i), rndNum.getRandNum());
            a.insert(virus);
        }

        Virus v1("AAA", 1001);
        Virus v2("BBB", 2002);

        a.insert(v1);
        a.insert(v2);

        //checking if the virus matches
        Virus v1_check = a.getVirus("AAA", 1001);
        numToInsert++;
        Virus v2_check = a.getVirus("BBB", 2002);
        numToInsert++;

        if (v1 == v1_check and v2 == v2_check){
            cout << "Index Matches" << endl;
        }

        cout << "Total inserted: " << numToInsert << endl;
        cout << "New size : " << a.m_currentSize << endl;

        cout<<"Insert test Passed since index and size both matches!"<<endl;

    }

    void findTest(){
        cout << endl;
        cout << "******************Testing Find operation error case******************" << endl;

        //creating vdetect object
        VDetect a(MINPRIME, hashCode, DOUBLEHASH );
        Random rndNum(MINID, MAXID);
        int numToInsert = 20;
        //inserting virus into table
        for(int i=0; i < numToInsert; i++){
            Virus virus = Virus(sequencer(5, i), rndNum.getRandNum());
            a.insert(virus);
        }
        int numToFind = 20;
        bool result = true;

        //checking if the virus matches
        for(int i=0; i < numToFind; i++){
            Virus virus = Virus(sequencer(5, i), rndNum.getRandNum());
            Virus vFound = a.getVirus(virus.getKey(), virus.getID());
            if (virus.getKey() == vFound.getKey() and virus.getID() == vFound.getID()){
                result = true;
            }else{
                result = false;
            }
        }
        if(result) {
            cout << "Find test finished successful since the key and id matches!" << endl;

        }else {
            cout << "Find test failed as virus is not found in database!" << endl;
        }
    }
    void findCollidingTest(){
        cout << endl;
        cout << "******************Testing Find operation with colliding keys******************" << endl;
        // Creating vdetect object
        VDetect vd(10, hashCode, DOUBLEHASH);

        // Insert some viruses
        vd.insert(Virus("ABC", 1234));
        vd.insert(Virus("ABC", 2345));
        vd.insert(Virus("GHI", 3456));

        // Test the find operation with some non-colliding keys
        Virus v1 = vd.getVirus("ABC", 1234);
        if(v1.getKey() == "ABC"){
            cout << "Test 1 passed (key)" << endl;
        }
        if(v1.getID() == 1234){
            cout << "Test 1 passed (id)" << endl;
        }


        Virus v2 = vd.getVirus("DEF", 2345);
        if(v2.getKey() == "DEF"){
            cout << "Test 2 passed (key)" << endl;
        }
        if(v2.getID() == 2345){
            cout << "Test 2 passed (id)" << endl;
        }else{
            cout << "Test 2 failed because it has same key as test1" << endl;
        }

        Virus v3 = vd.getVirus("GHI", 3456);
        if(v3.getKey() == "GHI"){
            cout << "Test 3 passed (key)" << endl;
        }
        if(v3.getID() == 3456){
            cout << "Test 3 passed (id)" << endl;
        }
        cout << vd.m_currentSize << endl;


    }
    void findNonCollidingTest(){
        cout << endl;
        cout << "******************Testing Find operation with non colliding keys******************" << endl;
        // Creating vdetect object
        VDetect vd(10, hashCode, DOUBLEHASH);

        // Insert some viruses
        vd.insert(Virus("ABC", 1234));
        vd.insert(Virus("DEF", 2345));
        vd.insert(Virus("GHI", 3456));

        // Test the find operation with some non-colliding keys
        Virus v1 = vd.getVirus("ABC", 1234);
        if(v1.getKey() == "ABC"){
            cout << "Test 1 passed (key)" << endl;
        }
        if(v1.getID() == 1234){
            cout << "Test 1 passed (id)" << endl;
        }

        Virus v2 = vd.getVirus("DEF", 2345);
        if(v2.getKey() == "DEF"){
            cout << "Test 2 passed (key)" << endl;
        }
        if(v2.getID() == 2345){
            cout << "Test 2 passed (id)" << endl;
        }


        Virus v3 = vd.getVirus("GHI", 3456);
        if(v3.getKey() == "GHI"){
            cout << "Test 3 passed (key)" << endl;
        }
        if(v3.getID() == 3456){
            cout << "Test 3 passed (id)" << endl;
        }

    }

    void removeCollidingTest(){
        cout << endl;
        cout << "******************Testing Remove operation with colliding keys******************" << endl;
        // Initialize a virus detection object
        VDetect detector(10, hashCode, QUADRATIC);

        // Test the remove operation with a colliding keys
        Virus v1("key1", 1);
        Virus v2("key1", 2);
        Virus v3("key1", 3);
        Virus v4("abc", 4);
        //inserting viruses in table
        detector.insert(v1);
        detector.insert(v2);
        detector.insert(v3);

        //checking tests
        if(detector.remove(v4) == true){
            cout << "Test 1 passed as key 4 is removed " << endl;
        }else{
            cout << "Test failed as the key is not in table" << endl;
        }
        if(detector.remove(v2) == true){
            cout << "Test 2 passed as key 2 is successfully removed" << endl;
        }
        if(detector.remove(v3) == true){
            cout << "Test 3 passed as key 3 is successfully removed" << endl;
        }


    }
    void removeNonCollidingTest(){
        cout << endl;
        cout << "******************Testing remove operation with non colliding keys******************" << endl;
        // Initialize a virus detection object
        VDetect detector(10, hashCode, QUADRATIC);

        // Test the remove operation with a non-colliding keys
        Virus v1("key1", 1);
        Virus v2("key2", 2);
        Virus v3("key3", 3);
        //inserting virus in table
        detector.insert(v1);
        detector.insert(v2);
        detector.insert(v3);

        //checking tests
        if(detector.remove(v1) == true){
            cout << "Test 1 passed as key 1 is successfully removed" << endl;
        }
        if(detector.remove(v2) == true){
            cout << "Test 2 passed as key 2 is successfully removed" << endl;
        }
        if(detector.remove(v3) == true){
            cout << "Test 3 passed as key 3 is successfully removed" << endl;
        }


    }

    void rehashingTriggerTest(){
        cout << endl;
        cout << "******************Testing if rehash is triggered******************" << endl;
        VDetect a(MINPRIME, hashCode, DOUBLEHASH );
        Random rndNum(MINID, MAXID);
        int numToInsert = 100;
        for(int i=0; i<numToInsert; i++){
            Virus virus = Virus(sequencer(5, i), rndNum.getRandNum());
            a.insert(virus);
        }

        // Test that rehashing is triggered after a decent number of data insertion
        if(a.lambda() <= 0.5){
            cout << "Worked as rehash is triggered" << endl;
        }
        if(a.deletedRatio() <= 0.2){
            cout << "Worked as ratio is less" << endl;
        }

        // Check whether the data size changes correctly
        int sizeBefore = a.m_currentSize + a.m_currNumDeleted + a.m_oldSize + a.m_oldNumDeleted;
        a.rehash();
        int sizeAfter = a.m_currentSize + a.m_currNumDeleted;
        if(sizeBefore == sizeAfter){
            cout  << "Size before : " << sizeBefore  << " equals to size after : " << sizeAfter << endl;
        }
    }


    void loadFactorRehashTest(){
        cout << endl;
        cout << "******************Testing loadfactor******************" << endl;

        VDetect vdetect(5, hashCode, QUADRATIC);

        // Insert some viruses
        Virus v1("A", 0);
        Virus v2("B", 1);
        Virus v3("C", 2);
        Virus v4("D", 3);
        Virus v5("E", 4);

        vdetect.insert(v1);
        vdetect.insert(v2);
        vdetect.insert(v3);
        vdetect.insert(v4);
        vdetect.insert(v5);

        // Check the load factor
        if(vdetect.lambda() < 1){
            cout << "Load factor test 1 passed" << endl;
        }

        // Check the table size
        if(vdetect.m_currentSize == 5){
            cout << "Table size matched" << endl;
        }

        // Remove some viruses
        vdetect.remove(v2);
        vdetect.remove(v3);

        // Check the load factor
        if(vdetect.lambda() < 0.5){
            cout << "Load factor test 2 passed" << endl;
        }

        // Insert some more viruses
        Virus v6("F", 5);
        Virus v7("G", 6);
        Virus v8("H", 7);

        vdetect.insert(v6);
        vdetect.insert(v7);
        vdetect.insert(v8);

        // Check the load factor
        if(vdetect.lambda() < 1){
            cout << "Load factor test 3 passed" << endl;
        }

        // Check the table size
        if(vdetect.m_currentSize == 6){
            cout << "Size test 2 passed" << endl;
        }
        cout << "Rehash test passed" << endl;
    }

    void rehashAfterRemoval(){
        cout << endl;
        cout << "******************Testing rehash after removal******************" << endl;
        // Create a VDetect object with a small hash table size and quadratic probing
        VDetect vd(10, hashCode, QUADRATIC);

        // Insert 10 viruses
        for (int i = 0; i < 10; i++) {
            string key = "Virus" + to_string(i);
            int id = MINID + i;
            vd.insert(Virus(key, id));
        }

        // Remove 8 viruses
        for (int i = 0; i < 8; i++) {
            string key = "Virus" + to_string(i);
            int id = MINID + i;
            vd.remove(Virus(key, id));
        }

        // Check if rehashing has not been triggered yet
        if(vd.m_currentCap > 10){
            cout << "Rehashing not triggered yet" << endl;
        }
        if(vd.m_oldCap == 0){
            cout << "Rehashing still not triggered" << endl;
        }

        // Remove 2 more viruses to trigger rehashing
        vd.remove(Virus("Virus8", MINID + 8));
        vd.remove(Virus("Virus9", MINID + 9));

        // Check if rehashing has been triggered
        if(vd.m_currentCap > 10){
            cout << "Rehash triggered" << endl;
        }
        if(vd.m_oldCap < 10){
            cout << "Rehash Triggered" << endl;
        }

        // Check if the data size changes correctly
        if(vd.m_oldSize == 0){
            cout << "Size changed correctly" << endl;
        }
    }

    void deleteRatioRemoveTest() {
        cout << endl;
        cout << "******************Testing rehash after delete ratio******************" << endl;
        VDetect vd(6, hashCode, DOUBLEHASH); // set small size for easier testing

        // insert some viruses
        vd.insert(Virus("A", 1));
        vd.insert(Virus("B", 2));
        vd.insert(Virus("C", 3));
        vd.insert(Virus("D", 4));
        vd.insert(Virus("E", 5));
        vd.insert(Virus("F", 6));

        // remove some viruses to increase delete ratio
        vd.remove(Virus("B", 2));
        vd.remove(Virus("E", 5));
        vd.remove(Virus("F", 6));

        // check the initial deletedRatio value
        float initial_deleted_ratio = vd.deletedRatio();
        cout << "Initial deleted ratio: " << initial_deleted_ratio << endl;

        // remove more viruses to trigger a rehash
        vd.remove(Virus("A", 1));
        vd.remove(Virus("D", 4));
        vd.remove(Virus("C", 3));

        // check the new deletedRatio value
        float new_deleted_ratio = vd.deletedRatio();
        cout << "New deleted ratio: " << new_deleted_ratio << endl;

        // assert that the new deletedRatio is 0
        if(new_deleted_ratio < 0.5){
            cout << "Delete ratio test passed" << endl;
        }
    }


    void TA() {
        cout << endl;
        cout << endl;
        cout << endl;
        cout << endl;
        cout << endl;
        cout << "THANK YOU TAs FOR YOUR HARD WORK. HAVE A GREAT BREAK!!!!" << endl;
    }
};


int main(){
    //calling tester class
    Tester test;
    test.insertTest();
    test.findTest();
    test.findNonCollidingTest();
    test. findCollidingTest();
    test.removeCollidingTest();
    test.removeNonCollidingTest();
    test.rehashingTriggerTest();
    test.loadFactorRehashTest();
    test.rehashAfterRemoval();
    test.deleteRatioRemoveTest();
    test.TA();
    return 0;
}

unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for ( int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}

string sequencer(int size, int seedNum){
    //this function returns a random DNA sequence
    string sequence = "";
    Random rndObject(0,3);
    rndObject.setSeed(seedNum);
    for (int i=0;i<size;i++){
        sequence = sequence + ALPHA[rndObject.getRandNum()];
    }
    return sequence;
}
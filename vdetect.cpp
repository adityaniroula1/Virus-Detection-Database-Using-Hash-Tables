// CMSC 341 - Spring 2023 - Project 4
//Name: Aditya Niroula
//Campus id : MF56332
//email : aniroul1@umbc.edu
//Description : main virus detection cpp file


#include "vdetect.h"

//constructor to initialize all the meber variables
VDetect::VDetect(int size, hash_fn hash, prob_t probing = DEFPOLCY){

    //allocating the size
    if (size < MINPRIME or size > MAXPRIME) {
        if (!isPrime(size)) {
            size = findNextPrime(size);
        }
    }
    //if size less than minprime
    if (size <= MINPRIME)
        size = MINPRIME;


    // Initialize member variables
    // Set the hash function and collision handling policy
    m_hash = hash;
    m_currProbing = probing;

    // Initialize the new table
    m_currentCap = size;
    m_currentTable = new Virus[size];
    m_currentSize = 0;
    m_currNumDeleted = 0;

    // Initialize the old table
    m_oldTable = nullptr;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = DEFPOLCY;


}

//destructor to deallocate the memory
VDetect::~VDetect(){

    //deallocating all the memory by deleting the table
    delete [] m_currentTable;

    if(m_oldTable != nullptr) {
        delete[] m_oldTable;
    }

}

//function to change the collision handling policy of the hash table at the runtime
void VDetect::changeProbPolicy(prob_t policy){
    // Update the new collision handling policy
    m_newPolicy = policy;

    // If a rehash is already in progress, set the collision handling policy for the new table
    if (m_oldTable != nullptr) {
        m_oldProbing = m_newPolicy;
    }
    else {
        // Otherwise, set the collision handling policy for the current table
        m_currProbing = m_newPolicy;
    }

}


//This function inserts an object into the current hash table.
// The insertion index is determined by applying the hash function m_hash that is set in the
// VDetect constructor and then reducing the output of the hash function modulo the table size.
bool VDetect::insert(Virus virus){

        // Calculate hash key
        int i = 0;
        unsigned index = 0;

        if (m_currentSize != 0) {
            // Using probing that is given to perform equation
            while (!(m_currentTable[index] == EMPTY) and !(m_currentTable[index] == DELETED)) {
                i++;
                if (m_currProbing == DOUBLEHASH) {
                    index = ((m_hash(virus.getKey()) % m_currentCap) + (i * i)) % m_currentCap;
                }else if(m_currProbing == NONE){
                    index = ((m_hash(virus.getKey()) % m_currentCap));
                }else{
                    index = ((m_hash(virus.getKey()) % m_currentCap) + (i * (11 - (m_hash(virus.getKey())) % 11))) % m_currentCap;
                }
            }
            //checking already existed key
            if (m_currentTable[index] == virus) {
                return false; //because same value already exists
            }
        }

        //checking id
        if (MINID < virus.getID() > MAXID) {
            return false;
        }

        //inserting in a table
        m_currentTable[index] = virus;
        m_currentSize++;

        // Load factor check
        if (lambda() > 0.5)  {
            rehash(); //new table creation with helper function
            //function to transfer 25 percent data
            trasferTwentyFive();  //calling helper function
        }

        return true;

}


//This function removes a data point from either the current hash table
// or the old hash table where the object is stored.
bool VDetect::remove(Virus virus) {
    // Finding the bucket containing the virus object
    int i = 0;
    unsigned index = 0;

    while (i < m_currentCap) {
        if (m_currentTable[index] == virus) {
            // Tag the bucket as deleted
            m_currentTable[index] = DELETED;
            m_currNumDeleted++;
            m_currentSize--;

            // Check deletion criteria
            if (m_currNumDeleted > 0.8 * (m_currentSize + m_currNumDeleted)) {
                // Rehash to a new table
                unsigned int newCap = findNextPrime(4 * (m_currentSize - m_currNumDeleted));
                Virus* newTable = new Virus[newCap];
                int newNumDeleted = 0;
                int newSize = 0;

                // Transfer data from the current table to the new table
                for (unsigned int j = 0; j < m_currentCap; j++) {
                    if (m_currentTable[j] != DELETED) {
                        unsigned int newIndex = m_hash(m_currentTable[j].getKey()) % newCap;
                        while (newTable[newIndex] != EMPTY) {
                            if (m_currProbing == DOUBLEHASH) {
                                newIndex = ((m_hash(virus.getKey()) % m_currentCap) + (i * i)) % m_currentCap;
                            }else if(m_currProbing == NONE){
                                newIndex = ((m_hash(virus.getKey()) % m_currentCap));
                            } else {
                                newIndex = ((m_hash(virus.getKey()) % m_currentCap) + (i * (11 - (m_hash(virus.getKey())) % 11))) % m_currentCap;
                            }
                        }
                        newTable[newIndex] = m_currentTable[j];
                        newSize++; //increasing size
                    } else {
                        newNumDeleted++;
                    }
                }

                // Deallocate the old table and update member variables
                delete[] m_currentTable;
                m_currentTable = newTable;
                m_currentCap = newCap;
                m_currNumDeleted = newNumDeleted;
                m_currentSize = newSize;
                m_currProbing = m_newPolicy;

                // Reset the old table variables
                m_oldTable = nullptr;
                m_oldCap = 0;
                m_oldSize = 0;
                m_oldNumDeleted = 0;
            }
            else if (lambda() < 0.1) {   //made change here
                // Incrementally transfer 25% of the nodes from the old table to the new table
                trasferTwentyFive();

                // If all data has been transferred, deallocate the old table
                if (m_oldNumDeleted + m_oldSize == 0) {
                    delete[] m_oldTable;
                    m_oldTable = nullptr;
                    m_oldCap = 0;
                    m_oldSize = 0;
                    m_oldNumDeleted = 0;
                }
            }

            return true; // Virus object has been removed
        }

        i++;
        //use probing according to what specified
        if (m_currProbing == DOUBLEHASH) {
            index = ((m_hash(virus.getKey()) % m_currentCap) + (i * i)) % m_currentCap;
        }else if(m_currProbing == NONE){
            index = ((m_hash(virus.getKey()) % m_currentCap));
        } else {
            index = ((m_hash(virus.getKey()) % m_currentCap) + (i * (11 - (m_hash(virus.getKey())) % 11))) % m_currentCap;
        }
    }

    return false; //if virus not found
}



//This function looks for the Virus object with key and id in the hash table,
// if the object is found the function returns it, otherwise the function returns empty object.
Virus VDetect::getVirus(string key, int id) const{
    // Look for the Virus object with key and id in the hash table
    Virus emptyVirus;

    // Check both hash tables for the virus object
    for (int j = 0; j < 2; j++) {
        const Virus* table = j == 0 ? m_currentTable : m_oldTable;
        const unsigned int cap = j == 0 ? m_currentCap : m_oldCap;
        //check if the table is empty
        if (table != nullptr) {
            //loop through the table and find keys and id
            for (unsigned int i = 0; i < cap; i++) {
                if (table[i] != EMPTY and table[i] != DELETED) {
                    if (table[i].getKey() == key and table[i].getID() == id) {
                        //return virus if found
                        return table[i];
                    }
                }
            }
        }
    }
    //return empty object if not found
    return emptyVirus;


}

//This function returns the load factor of the current hash table.
// The load factor is the ratio of occupied buckets to the table capacity
float VDetect::lambda() const {
    //total avaialble buckets / total capacity
    if (m_currentTable != nullptr) {
        //changing to float
        float num = (float)m_currNumDeleted + (float)m_currentSize;
        return num / (float)m_currentCap;
    }

}

//This function returns the ratio of the deleted buckets to the total number of occupied buckets .
float VDetect::deletedRatio() const {
    //ration of deleted buckets in float
    return (float)m_currNumDeleted / (float)m_currentSize;

}

void VDetect::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}


//helper function to check if the number is prime or not
bool VDetect::isPrime(int number) {
    if (number < 2) {
        return false;
    }
    for (int i = 2; i * i <= number; ++i) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}


//helper function to find the next prime number
int VDetect::findNextPrime(int current) {
    if (current < MINPRIME) {
        current = MINPRIME - 1;
    }
    for (int i = current + 1; i <= MAXPRIME; ++i) {
        if (isPrime(i)) {
            return i;
        }
    }
    //if it goes over MAXPRIME
    return MAXPRIME;
}


//assignment operator
ostream& operator<<(ostream& sout, const Virus &virus ) {
    if (!virus.m_key.empty())
        sout << virus.m_key << " (ID " << virus.m_id << ")";
    else
        sout << "";
    return sout;
}

//assignment operator
bool operator==(const Virus& lhs, const Virus& rhs){
    return ((lhs.m_key == rhs.m_key) and (lhs.m_id == rhs.m_id));
}



//helper functions

//this helper funtion helps to rehash the table
void VDetect::rehash(){
    // delete old table if it exists
    if (m_oldTable != nullptr) {
        delete[] m_oldTable;
        m_oldTable = nullptr;
    }
    // find the new size of the table
    int newSize = findNextPrime(m_currentCap * 2);
    Virus* newTable = new Virus[newSize]();
    int oldSize = m_currentSize;

    // iterate through the old table and rehash each element into the new table
    for (int i = 0; i < m_currentCap; i++) {
        // skip empty and deleted elements
        if (m_currentTable[i] == EMPTY and m_currentTable[i] == DELETED) {
            int count = 0;
            unsigned int index = 0;
            // probe for an empty spot in the new table
            while (newTable[index] == EMPTY) {
                count++;
                // use probing to what specified
                if(m_oldProbing == DOUBLEHASH) {
                    index = ((m_hash(m_oldTable[i].getKey()) % m_currentCap) + ((count * count))) % m_currentCap;
                }else if(m_currProbing == NONE){
                    index = ((m_hash(m_oldTable[i].getKey()) % m_currentCap));
                }else{
                    index = ((m_hash(m_oldTable[i].getKey()) % m_currentCap) + (i * (11 - (m_hash(m_oldTable[i].getKey())) % 11))) % m_currentCap;
                }
            }
            // rehash the element into the new table
            newTable[index] = m_currentTable[i];
        }
    }
    // delete the old table and update the member variables
    delete[] m_currentTable;
    m_currentTable = newTable;
    m_currentCap = newSize;
    m_currentSize = oldSize;
    m_currNumDeleted = 0;

}


//this helper funtion transfers 25% of the nodes at a time
bool VDetect::trasferTwentyFive(){
    // return false if there is no old table
    if (!m_oldTable) {
        return false;
    }
    // calculate how many elements to transfer
    const int transferCount = m_oldSize / 4;
    int transferredCount = 0;
    bool transferAll = transferCount >= m_oldSize - m_oldNumDeleted;

    // iterate through the old table and transfer elements to the new table
    for (int i = 0; (i < m_oldCap) && (transferredCount < transferCount or transferAll); i++) {
        if (m_oldTable[i] == EMPTY or m_oldTable[i] == DELETED) {
            continue;
        }

        unsigned int index = 0;
        int count = 0;
        // use probing according to what specified
        if (m_oldProbing == DOUBLEHASH) {
            index = (m_hash(m_oldTable[i].getKey()) % m_currentCap) + (count * count) % m_currentCap;
            // probe for an empty spot in the new table
            while (m_currentTable[index] != EMPTY) {
                count++;
                index = (m_hash(m_oldTable[i].getKey()) % m_currentCap) + (count * count);
            }
        }else if(m_oldProbing == NONE){
            index = (m_hash(m_oldTable[i].getKey()) % m_currentCap);
            // prob for an empty spot in the new table
            while (m_currentTable[index] != EMPTY) {
                count++;
                index = (m_hash(m_oldTable[i].getKey()) % m_currentCap);
            }

        }else {
            index = ((m_hash(m_oldTable[i].getKey()) % m_currentCap) + (i * (11 - (m_hash(m_oldTable[i].getKey())) % 11))) % m_currentCap;
            // probe for an empty spot in the new table
            while (m_currentTable[index] != EMPTY) {
                count++;
                index = (index + 1) % m_currentCap;
            }
        }
        // transfer the element to the new table
        m_currentTable[index] = m_oldTable[i];
        transferredCount++;
        m_currentSize++;
        m_oldNumDeleted++;
    }


    //deallocating old table
    if (transferAll) {
        delete[] m_oldTable;
        m_oldTable = nullptr;
        m_oldCap = 0;
        m_oldSize = 0;
        m_oldNumDeleted = 0;
    }
    return true;   //return true
}
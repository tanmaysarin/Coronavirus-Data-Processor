#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <experimental/filesystem>
#include <locale>
#include <iomanip>
#include <vector>

#include <map>

using namespace std;

//template<typename CountryNameT, typename DataStruct>

class covid
{
private:
    //
    // A struct which stores the total
    // number of deaths, confirmed and
    // recover cases.
    // 
    struct DataStruct
    {
        string Death;      // Number Death cases      
        string Confirmed;  // Number of Confirmed cases
        string Recovered;  // Number of Recovered cases
    };
public:
    
    //
    // A map which uses the name of the
    // country as the primary key, and 
    // connects the DataStruct as the
    // information related to the specific
    // map/ country key/name.
    //  
    map<CountryName, DataStruct> covidMap;
    
    // constructors:
    covid()
    {
        // default empty empty map:
    }
    
    //
    // Function to read the file (daily_reports
    // and the worldfacts). The function takes
    // the file name as a pratameter and then
    // stores the data in maps and structs.
    // 
    // The function takes the filename as a parameter
    //    
    void fileReader(string filename){ //vector to store all the entries

        ifstream infile(filename);

        if (!infile.is_open()) {
            cout << "**Error, unable to open '" <<filename<<"'"<< endl;
            return addresses;
        }

        string line; // variable to read the string in the file
        
        getline(infile, line); // skip first line (header row)
        
        while(getline(infile, line)) // start reading the file line by line
        {
            if (line[0] == '"')// => province is "city, state"
            {
                //
                // we want to delete the " on either end of the value, and
                // delete the ',' embedded within => will get city state:
                //
                line.erase(0, 1); // delete the leading "
                size_t pos = line.find(','); // find embedded ','
                line.erase(pos, 1); // delete ','
                pos = line.find('"');// find closing "
                line.erase(pos, 1);// delete closing "
            }
            
            stringstream s(line);
            string province, country, last_update;
            string confirmed, deaths, recovered;
            getline(s, province, ',');
            getline(s, country, ',');
            getline(s, last_update, ',');
            getline(s, confirmed, ',');
            getline(s, deaths, ',');
            getline(s, recovered, ',');
            
            if (confirmed == "")
                confirmed = "0";
            if (deaths == "")
                deaths = "0";
            if (recovered == "")
                recovered = "0";
            if (country == "Mainland China") // map to name in the earlier reports:
                country = "China";
            
            //
            //Make a new Struct and store
            // the data in the struct.
            // 
            DataStruct newDataStruct = new DataStruct;
            newDataStruct->Death = deaths;
            newDataStruct->Confirmed = confirmed;
            newDataStruct->Recovered = recovered;
            
            covidMap.emplace(country, newDataStruct); // Add the struct data to the map, making country as the key
        }

        infile.close();
  
    }
    
    //
    // A function to return the total number
    // of contries stored in the map.
    // 
    // The function takes no parameters
    // The function returns an integer.
    // 
    int size()
    {
        return covidMap.size();
    }

    
         
    
    
    
    
    

};
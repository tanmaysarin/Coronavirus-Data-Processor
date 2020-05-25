/*main.cpp*/

//
// Name: Tanmay Sarin
// COVID-19 Data Analysis Program
// 
// The program provides the user with 6 menu
// options namely:
// -> Help: provides a list of all the options available
// -> <name>: Lookup and display of data by country
// -> counties: lists all countries in alphabetical order
//              with data from the most recent report
// -> top10: countries based on # of confirmed cases
// -> totals: Lists the worldwide totals (confirmed, deaths, recovered)
// -> MedicalSuccess: Reports the percentage of recovered
//                    cases in all countries, thus shouwing
//                    the medical success.
// 
// !Stay safe and Healthy!             
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <experimental/filesystem>
#include <locale>
#include <iomanip>
#include <vector>

#include <map>
#include <algorithm>

using namespace std;
namespace fs = std::experimental::filesystem;


//
// getFilesWithinFolder
//
// Given the path to a folder, e.g. "./daily_reports/", returns 
// a vector containing the full pathnames of all regular files
// in this folder.  If the folder is empty, the vector is empty.
//
vector<string> getFilesWithinFolder(string folderPath)
{
  vector<string> files;

  for (const auto& entry : fs::directory_iterator(folderPath))
  {
    files.push_back(entry.path().string());
  }
    
    std::sort(files.begin(), files.end());

  return files;
}

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

// 
// FileReader-->
// 
// Function to read the file (daily_reports)
// The function takes the file name as a
// pratameter and then stores the data in
// maps and structs.
// 
// The function is of type void and takes
// the filename and a map as its parameters.
//    
void fileReader(string filename, std::map<string, DataStruct> &covidMap){ //vector to store all the entries

    ifstream infile(filename);

    if (!infile.is_open()) {
        cout << "**Error, unable to open '" <<filename<<"'"<< endl;
        return;
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
        // Make a new Struct and store
        // the data in the struct.
        //         
        if(covidMap.count(country) == 0){ //country is not on the map
            
            DataStruct newDataStruct;
            newDataStruct.Death = deaths;
            newDataStruct.Confirmed = confirmed;
            newDataStruct.Recovered = recovered;

            covidMap.emplace(country, newDataStruct); // Add the struct data to the map, making country as the key
        }
        else if(covidMap.count(country) == 1){ // country is in the map
                        
            //need to access variable with value(struct) of an assigned key
            int currentDeath = stoi(covidMap.at(country).Death);
            int currentRecovered = stoi(covidMap.at(country).Recovered);
            int currentConfirmed = stoi(covidMap.at(country).Confirmed);
            
            int total_death = 0, total_confirmed = 0, total_recovered = 0;
            
            total_death = currentDeath + stoi(deaths);           // 
            total_recovered = currentRecovered + stoi(recovered);// add the values if there exists more line of data for any country
            total_confirmed = currentConfirmed + stoi(confirmed);//
            

            DataStruct aDataStruct; // Create a new struct
            aDataStruct.Death = to_string(total_death);              
            aDataStruct.Confirmed = to_string(total_confirmed);
            aDataStruct.Recovered = to_string(total_recovered); 
            covidMap[country] = aDataStruct; // Replace the old struct with the new struct
            
        }
    }
    infile.close();
}

// 
// WorldFactsReader-->
// 
// Function to read the file (worldfacts)
// The function takes the file name as a
// pratameter and then stores the data in
// maps and structs.
// 
// The function is of type void and takes
// the filename and a map as its parameters.
//    
void worldfactsReader(string filename, std::map<string, string> &factsMap)
{
    ifstream infile(filename);

    if (!infile.is_open()) {
        cout << "**Error, unable to open '" <<filename<<"'"<< endl;
        return;
    }

    string line; // variable to read the string in the file

    getline(infile, line); // skip first line (header row)

    while(getline(infile, line)) // start reading the file line by line
    {
        stringstream s(line);
        
        string position, name, value;
        
        getline(s, position, ',');
        getline(s, name, ',');
        getline(s, value, ',');

        factsMap.emplace(name, value); // Add the value to the map, making name as the key
    }
    infile.close();
}

// 
// Confirmed_Timeline_First()-->
// 
// Prints out the first seven lines
// of the confirmed timeline.
// 
// The function is of type void, and 
// takes the integers first and  counter,
// the strings, userInput and foodFile,
// a map and a vector of files as its 
// parameters.
// 
void confirmed_timeline_first(int first, string userInput, int counter, string foundFile, map<string, DataStruct> b, vector<string> files)
{
    for(unsigned int j=first; j<first + 7; j++){//Getting the first 7 files
        fileReader(files.at(j), b); // read the files and store them in a map
        if(b.find(userInput) != b.end()){
            if(b[userInput].Confirmed != "0"){                    
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // get the date
                cout<<foundFile<<" (day "<<counter++<<"): "<< b[userInput].Confirmed<<endl;
                b.clear();
            }else{
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10);
                cout<<foundFile<<" (day "<<counter++<<"): 0"<<endl;
                b.clear();
            }
        }
    }
}

// 
// Confirmed_Timeline_Last()-->
// 
// Prints out the last seven lines
// of the confirmed timeline.
// 
// The function is of type void, and 
// takes the integers last and  counter,
// the strings, userInput and foodFile,
// a map and a vector of files as its 
// parameters.
// 
void confirmed_timeline_last(int last, string userInput, string foundFile, map<string, DataStruct> b, vector<string> files)
{
    for(unsigned int j = (last - 6); j<(last+1); j++){ //Getting the last seven files
        fileReader(files.at(j), b); // read the files and store them in a map
        if(b.find(userInput) != b.end()){
            if(b[userInput].Confirmed != "0"){
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // get the date
                cout<<foundFile<<" (day "<<j+1<<"): "<< b[userInput].Confirmed<<endl;
                b.clear();
            }else{
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10);
                cout<<foundFile<<" (day "<<j+1<<"): 0"<<endl;
                b.clear();
            }
        }
    }
}

// 
// Confirmed_Timeline_All()-->
// 
// Prints out the all lines of the
// confirmed timeline if the number of lines
// is less than or equal to 14.
// 
// The function is of type void, and 
// takes the integers last and  counter,
// the strings, userInput and foodFile,
// a map and a vector of files as its 
// parameters.
//
void confirmed_timeline_all(int first, string userInput, int counter, string foundFile, map<string, DataStruct> b, vector<string> files)
{
    for(unsigned int j=first; j<files.size(); j++){//Getting the rest of the files
        fileReader(files.at(j), b); // read the files and store them in a map
        if(b.find(userInput) != b.end()){
            if(b[userInput].Confirmed != "0"){
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // get the date
                cout<<foundFile<<" (day "<<counter++<<"): "<< b[userInput].Confirmed<<endl;
                b.clear();
            }else{
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10);
                cout<<foundFile<<" (day "<<counter++<<"): 0"<<endl;
                b.clear();
            }
        }
    }
}

// 
// Confirmed_Helper()-->
// 
// A helper function for the "c" command of
// the Timeline() function. It displays
// a timeline of all confirmed cases if they
// are less than 14, otherwise displays the
// first 7 and the last 7 case timelines only.
// 
// The function is of type void and takes
// a user input and a vector of files as its 
// parameters.
// 
void confirmed_helper(string userInput, vector<string> files)
{
    map<string, DataStruct> b;
    string foundFile;
    
    cout<<"Confirmed:"<<endl;
    
    int first = 0, last = 0;
    
    for(unsigned int i = 0; i < files.size(); i++){ // get the first confirmed date
        fileReader(files.at(i), b);
        if(b.find(userInput) != b.end()){
            if(b[userInput].Confirmed != "0"){
                first = i;
                b.clear();
                break;
            }
        }
    }
        
    for(unsigned int i = files.size()-1; i > 0; i--){ //get the last confirmed date
        fileReader(files.at(i), b);
        if(b.find(userInput) != b.end()){
            if(b[userInput].Confirmed != "0"){
                last = i;
                b.clear();
                break;
            }
        }
    }
        
    if((last - first) > 14){
        int counter = first + 1;
        confirmed_timeline_first(first, userInput, counter, foundFile, b, files); //Call the function to get the first 7 lines
        cout<<" .\n .\n ."<<endl;
        confirmed_timeline_last(last, userInput, foundFile, b, files); //Call the function to get the last 7 lines
    }
    else{
        int counter = 1;
        confirmed_timeline_all(first, userInput, counter, foundFile, b, files); // Call the function to get all the lines
    }
}

// 
// Death_Timeline_First()-->
// 
// Prints out the first seven lines
// of the death timeline.
// 
// The function is of type void, and 
// takes the integers first and  counter,
// the strings, userInput and foodFile,
// a map and a vector of files as its 
// parameters.
// 
void death_timeline_first(int first, string userInput, int counter, string foundFile, map<string, DataStruct> b, vector<string> files)
{
    for(unsigned int j=first; j<first + 7; j++){//Getting the first 7 files
        fileReader(files.at(j), b); // read the files and store them in a map
        if(b.find(userInput) != b.end()){
            if(b[userInput].Death != "0"){
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // get the date
                cout<<foundFile<<" (day "<<counter++<<"): "<< b[userInput].Death<<endl;
                b.clear();
            }else{
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10);
                cout<<foundFile<<" (day "<<counter++<<"): 0"<<endl;
                b.clear();
            }
        }
    }
}

// 
// Death_Timeline_Last()-->
// 
// Prints out the last seven lines
// of the death timeline.
// 
// The function is of type void, and 
// takes the integers last and  counter,
// the strings, userInput and foodFile,
// a map and a vector of files as its 
// parameters.
//
void death_timeline_last(int last, string userInput, string foundFile, map<string, DataStruct> b, vector<string> files)
{
    for(unsigned int j = (last - 6); j<(last+1); j++){ //Getting the last seven files
        fileReader(files.at(j), b); // read the files and store them in a map
        if(b.find(userInput) != b.end()){
            if(b[userInput].Death != "0"){
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // get the date
                cout<<foundFile<<" (day "<<j+1<<"): "<< b[userInput].Death<<endl;
                b.clear();
            }else{
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10);
                cout<<foundFile<<" (day "<<j+1<<"): 0"<<endl;
                b.clear();
            }
        }
    }
}

// 
// Death_Helper()-->
// 
// A helper function for the "d" command of
// the Timeline() function. It displays
// a timeline of all death cases if they
// are less than 14, otherwise displays the
// first 7 and the last 7 case timelines only.
// 
// The function is of type void and takes
// a user input and a vector of files as its 
// parameters.
// 
void death_helper(string userInput, vector<string> files)
{
    map<string, DataStruct> b;
    string foundFile;
    
    cout<<"Deaths:"<<endl;
    
    int first = 0, last = 0;
    
    for(unsigned int i = 0; i < files.size(); i++){ // get the first confirmed date
        fileReader(files.at(i), b);
        if(b.find(userInput) != b.end()){
            if(b[userInput].Death != "0"){
                first = i;
                b.clear();
                break;
            }
        }
    }
    
    for(unsigned int i = files.size()-1; i > 0; i--){ //get the last confirmed date
        fileReader(files.at(i), b);
        if(b.find(userInput) != b.end()){
            if(b[userInput].Death != "0"){
                last = i;
                b.clear();
                break;
            }
        }
    }
    
    if((last - first) > 14){
        int counter = first + 1;
        death_timeline_first(first, userInput, counter, foundFile, b, files); //Call the function to get the first 7 lines
        cout<<" .\n .\n ."<<endl;
        death_timeline_last(last, userInput, foundFile, b, files); //Call the function to get the last 7 lines
    }
}

// 
// Recovered_Timeline_First()-->
// 
// Prints out the first seven lines
// of the recovered timeline.
// 
// The function is of type void, and 
// takes the integers first and  counter,
// the strings, userInput and foodFile,
// a map and a vector of files as its 
// parameters.
// 
void recovered_timeline_first(int first, string userInput, int counter, string foundFile, map<string, DataStruct> b, vector<string> files)
{
    for(unsigned int j=first; j<first + 7; j++){//Getting the first 7 files
        fileReader(files.at(j), b); // read the files and store them in a map
        if(b.find(userInput) != b.end()){
            if(b[userInput].Recovered != "0"){
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // get the date
                cout<<foundFile<<" (day "<<counter++<<"): "<< b[userInput].Recovered<<endl;
                b.clear();
            }else{
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10);
                cout<<foundFile<<" (day "<<counter++<<"): 0"<<endl;
                b.clear();
            }
        }
    }
}

// 
// Recovered_Timeline_Last()-->
// 
// Prints out the last seven lines
// of the recovered timeline.
// 
// The function is of type void, and 
// takes the integers last and firstc,
// the strings, userInput and foodFile,
// a map and a vector of files as its 
// parameters.
//
void recovered_timeline_last(int last, string userInput, int firstc, string foundFile, map<string, DataStruct> b, vector<string> files)
{
    for(unsigned int j = (last - 6); j<(last+1); j++){ //Getting the last seven files
        fileReader(files.at(j), b); // read the files and store them in a map
        if(b.find(userInput) != b.end()){
            if(b[userInput].Recovered != "0"){
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // get the date
                cout<<foundFile<<" (day "<<j - firstc +1<<"): "<< b[userInput].Recovered<<endl;
                b.clear();
            }else{
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10);
                cout<<foundFile<<" (day "<<j - firstc +1<<"): 0"<<endl;
                b.clear();
            }
        }
    }
}

// 
// Recovered_Helper()-->
// 
// A helper function for the "r" command of
// the Timeline() function. It displays
// a timeline of all recovered cases if they
// are less than 14, otherwise displays the
// first 7 and the last 7 case timelines only.
// 
// The function is of type void and takes
// a user input and a vector of files as its 
// parameters.
// 
void recovered_helper(string userInput, vector<string> files)
{
    map<string, DataStruct> b;
    string foundFile;
    
    cout<<"Recovered:"<<endl;
    
    int first = 0, last = 0, firstc =0;
    
    for(unsigned int i = 0; i < files.size(); i++){ // get the first confirmed date
        fileReader(files.at(i), b);
        if(b.find(userInput) != b.end()){
            if(b[userInput].Confirmed != "0"){
                firstc = i;
                b.clear();
                break;
            }
        }
    }
    
    for(unsigned int i = 0; i < files.size(); i++){ // get the first confirmed date
        fileReader(files.at(i), b);
        if(b.find(userInput) != b.end()){
            if(b[userInput].Recovered != "0"){
                first = i;
                b.clear();
                break;
            }
        }
    }
    
    for(unsigned int i = files.size()-1; i > 0; i--){ //get the last confirmed date
        fileReader(files.at(i), b);
        if(b.find(userInput) != b.end()){
            if(b[userInput].Recovered != "0"){
                last = i;
                b.clear();
                break;
            }
        }
    }
    
    if((last - first) > 14){
        int counter = first - firstc + 1;
        recovered_timeline_first(first, userInput, counter, foundFile, b, files); //Call the function to get the first 7 lines
        cout<<" .\n .\n ."<<endl;
        recovered_timeline_last(last, userInput, firstc, foundFile, b, files); //Call the function to get the last 7 lines
    }
}

// 
// Timeline()-->
// 
// A helper function called in the the 
// country_name function to help setup
// the timeline functionality of the
// <name> menu function option.
// 
// The function is of type void and takes
// a string userInput and a vector of files
// as its parameters.
// 
void timeline(string userInput, vector<string> files)
{
    string input;
    
    cout<<"Do you want to see a timeline? Enter c/d/r/n> ";
    getline(cin, input);
    
    if(input == "c"){
        confirmed_helper(userInput, files); // Call the function to display the confirmed cases timeline
    }else if(input == "d"){
        death_helper(userInput, files); // Call the function to display the death cases timeline
    }else if(input == "r"){
        recovered_helper(userInput, files); // Call the function to display the recovered cases timeline
    }else if(input == "n"){
        return;
    }
}

// 
// Totals()-->
// 
// The totals function returns the
// total number of deaths, confirmed
// and recovered cases from the virus.
// 
// The function also gives outputs a percentage
// of the deaths and recovered cases.
// 
// The function is of type void.
// It takes the files vector as its parameters.
// 
void totals(vector<string> files)
{
    map<string, DataStruct> newMap;
    string nameFile;
    
    nameFile = files.back(); // Get the file with the latest report
    
    fileReader(nameFile, newMap); // read the latest report file and store it into a new map
    
    nameFile = nameFile.substr(16, 10); // Select the name of the file to be displayed from the full file name
    
    cout<<"As of "<<nameFile<<", the world-wide totals are:\n";
    
    float total_death = 0, total_recovered = 0;
    int total_confirmed = 0;
    float deaths = 0, confirmed = 0, recovered = 0;
    
    for(auto [key, val] : newMap) {
        
        deaths = stof(val.Death);
        total_death = total_death + deaths; // Calculate the total number od deaths due to the virus
        
        confirmed = stof(val.Confirmed);
        total_confirmed = total_confirmed + confirmed; // Calculate the total number of virus confirmed cases
        
        recovered = stof(val.Recovered);
        total_recovered = total_recovered + recovered; // Calculate the total number of recovered cases
    }
    
    cout<<" confirmed: "<<total_confirmed<<endl;
    cout<<" deaths: "<<int(total_death)<<" ("<<((total_death/total_confirmed)*100)<<"%)"<<endl;
    cout<<" recovered: "<<int(total_recovered)<<" ("<<((total_recovered/total_confirmed)*100)<<"%)"<<endl;
}

// 
// Countries()-->
// 
// The countries function lists all the
// countries alphabetically, their number
// of confirmed, death and recovered cases
//  in that order.
//  
//  The function is of type void, and takes
//  the files vector as its parameters.
//  
void countries(vector<string> files)
{
    map<string, DataStruct> newMap;
    string nameFile;
    
    //Read all the data from all the files and add the countries to the map
    for(unsigned int i=0; i< files.size(); i++){
        fileReader(files.at(i), newMap);//get the string at the i'th position and read the file
    }
    
    for(auto& [key, val] : newMap){
        val.Confirmed = to_string(0);
        val.Death = to_string(0);
        val.Recovered = to_string(0);
    }
    
    nameFile = files.back(); // Get the file with the latest report
    
    fileReader(nameFile, newMap); // read the latest report file and store it into a new map
    
    for(auto [key, val] : newMap){
        cout<<key<<": "<<val.Confirmed<<", "<<val.Death<<", "<<val.Recovered<<endl;
    }
}

// 
// SortByVal()-->
// 
// A helper utility comparator function
// to pass to the sort() module.
// 
// The function is of type bool, and takes
// two pairs as its parameters and returns
// the sorted values in ascending order.
// 
bool sortByVal(const pair<string, int> &a, 
               const pair<string, int> &b) 
{ 
    return (a.second < b.second); 
}

// 
// Top10()-->
// 
// Prints of the top 10 countries with 
// highest number of confirmed cases.
// 
// The funtion is of type void.
// The function takes, the file vector
// and the map as its parameters.
// 
void top10(vector<string> files)
{
    int counter = 0;
    
    map<string, DataStruct> newMap;
    string nameFile;
    
    nameFile = files.back(); // Get the file with the latest report
    
    fileReader(nameFile, newMap); // read the latest report file and store it into a new map
    
    // create a empty vector of pairs
    vector<pair<string, int>> vec;
    
    for(auto [key, val] : newMap){
        vec.push_back(make_pair(key, stoi(val.Confirmed)));
    }

    // // sort the vector by increasing order of its pair's second value
    sort(vec.begin(), vec.end(), sortByVal); 

    // print the vector
    for (unsigned int i = vec.size()-1; i>vec.size()-11; i--)
    {
        counter++;
        cout<<counter<<". "<< vec[i].first << ": " << vec[i].second << endl;
    }
}


// 
// Country_Name()-->
// 
// The function returns the data of the country
// input given by the user. It displays the
// population, life expectancy and the latest data
// along with the first confirmed case and the first
// recorded death.
// 
// It also gives a the user a timeline option
// if the user wants to see the timeline of
// confirmed/deaths/recovered cases, and calls the
// timeline function to implement the functionality.
// 
// The function if of type void, and takes userInput
// and two vectors of files as its parameters.
// 
void country_name(string userInput, vector<string> files, vector<string> files2)
{
    map<string, DataStruct> newMap;
    map<string, DataStruct> a; // we need this because newMap is being read and already has all the values, so we need a new fresh map.
    map<string, DataStruct> b;
    
    map<string, string> factsMap; // Store the data of life_expectancies with the country name as the key
    map<string, string> popMap; // Store the data of population with country name as the key
    worldfactsReader(files2[0], factsMap);
    worldfactsReader(files2[1], popMap);
    
    
    if(popMap.find(userInput) != popMap.end()){ // Check if the name of the country entered exists in the data file
        cout<<"Population: "<<stoi(popMap[userInput])<<endl;
    }else{
        cout<<"Population: 0"<<endl;
    }
    
    if(factsMap.find(userInput) != factsMap.end()){ // Check if the name of the country entered exists in the data file
        cout<<"Life Expectancy: "<<stof(factsMap[userInput])<<" years"<<endl;
    }else{
        cout<<"Life Expectancy: 0.00"<<endl;
    }
    
    string nameFile, foundFile;
    
    nameFile = files.back(); // Get the file with the latest report
    
    fileReader(nameFile, newMap); // read the latest report file and store it into a new map
    
    string deaths = newMap[userInput].Death;
    string confirmed = newMap[userInput].Confirmed;
    string recovered = newMap[userInput].Recovered;
    
    if(deaths == ""){
        deaths = "0";
    }
    if(confirmed == ""){
        confirmed = "0";
    }
    if(recovered == ""){
        recovered = "0";
    }
    
    //
    // To display the first confirmed cases
    // 
    for(unsigned int i=0; i < files.size(); i++){
        fileReader(files.at(i), a);//get the string at the i'th position and read the file
        if(a.find(userInput) != a.end()){
            foundFile = files.at(i); // Get the file name where the first case was recorded
            foundFile = foundFile.substr(16, 10); // Get the file name
            cout<<"Latest data:"<<endl;
            cout<<" confirmed: "<<confirmed<<endl;
            cout<<" deaths: "<<deaths<<endl;
            cout<<" recovered: "<<recovered<<endl;

            cout<<"First confirmed case: "<<foundFile<<endl;
            a.clear();
            break;
        }else{
            a.clear();
        }
    }
    
    // 
    // To display the first death recorded
    // 
    for(unsigned int j=0; j<files.size(); j++){
        fileReader(files.at(j), b); // Read all the files one by one
         if(b.find(userInput) != b.end()){ // Search the map based on key
            if(b[userInput].Death != "0"){ // Search the first instance where the death recorded is not 0 
                foundFile = files.at(j);
                foundFile = foundFile.substr(16, 10); // Get the file name
                cout<<"First recorded death: "<<foundFile<<endl;

                b.clear(); // Clear the map
                break; // If the first instance is fount, the break from the loop
            }else if (b[userInput].Death == "0"){
                fileReader(files.at(files.size()-1), b); // Read the lastest file
                if(b[userInput].Death == "0"){ // If the death recorded there is still 0, then no death recorded
                    cout<<"First recorded death: none"<<endl;
                    b.clear();
                    break;
                }
                b.clear();
            }
         }
    }
}

//
// MedicalSuccess()-->
// 
// Reports the percentage of recovered 
// cases in all countries.
// 
// The function is of type void
// and takes the files vector as its
// parameter.
// 
void medicalSuccess(vector<string> files)
{
    map<string, DataStruct> successMap;
    string nameFile;
    float percentage=0;
    
    nameFile = files.back(); // Get the file with the latest report
    
    fileReader(nameFile, successMap); // read the latest report file and store it into a new map
    
    cout<<"The recovery percentage of each country is ->"<<endl;
    
    for(auto [key, val] : successMap){
        percentage = ((stof(val.Recovered) / stof(val.Confirmed)) * 100); // Calculate the percentage for each country
        cout<<key<<":"<<endl;
        cout<<" Confirmed: "<<val.Confirmed<<endl;
        cout<<" Recovered: "<<val.Recovered<<endl;
        cout<<" Medical Success: "<<percentage<<"%"<<endl;
        
        cout<<endl;
    }
}

//
// Help()-->
// 
// Displays a help menu, listing
// all the possible commands and
// their uses in the program
// 
// The function is of type void 
// and has no parameters
// 
void help()
{
    cout<<"Available commands:\n";
    cout<<" <name>: enter a country name such as US or China \n";
    cout<<" countries: list all countries and most recent report\n";
    cout<<" top10: list of top 10 countries based on most recent # of confirmed cases\n";
    cout<<" totals: world-wide totals of confirmed, deaths, recovered\n";
    cout<<" MedicalSuccess: Reports the percentage of recovered cases in all countries, thus shouwing the medical success"<<endl;
}

//
// main:
//
int main()
{
  cout << "** COVID-19 Data Analysis **" << endl;
  cout << endl;
  cout << "Based on data made available by John Hopkins University" << endl;
  cout << "https://github.com/CSSEGISandData/COVID-19" << endl;
  cout << endl;
  
  //
  // setup cout to use thousands separator, and 2 decimal places:
  //
  cout.imbue(std::locale(""));
  cout << std::fixed;
  cout << std::setprecision(2);

  //
  // get a vector of the daily reports, one filename for each:
  //
  vector<string> files1 = getFilesWithinFolder("./daily_reports/");
  vector<string> files2 = getFilesWithinFolder("./worldfacts/");
    
        
  map<string, DataStruct> covidMap; // Declare a map in main 
  map<string, DataStruct> factsMap;
    
  cout<<">> Processed "<< files1.size() <<" daily reports"<<endl;
  cout<<">> Processed "<< files2.size() <<" files of world facts"<<endl;
    
  //Read all the data from all the files and add the countries to the map
  for(unsigned int i=0; i< files1.size(); i++){
      fileReader(files1.at(i), covidMap);//get the string at the i'th position and read the file
  }
        
  cout<<">> Current data on "<< covidMap.size() <<" countries"<<endl;
      
  cout<<"Enter command (help for list, # to quit)> ";
 
  string userInput;
   
  getline(cin, userInput); // Take the input from the user
    
  while(userInput != "#")
  {
      if(covidMap.find(userInput) != covidMap.end()){ // gives the name of the country to get that country's data
          country_name(userInput, files1, files2);
          timeline(userInput, files1); // Call the timeline function to further implement the functionality of the menu option.
      }else if (userInput == "help"){ //selects the help option
          help();
      }else if(userInput == "top10"){ //selects the top 10 command
          top10(files1);
      }else if(userInput == "totals"){ // selects the totals command
          totals(files1);
      }else if(userInput == "countries"){ //displays all the countries
          countries(files1);
      }else if(userInput == "MedicalSuccess"){ //my command, my choice
          medicalSuccess(files1);
      }else{
          cout<<"country or command not found..."<<endl;
      }
      cout<<endl;
      cout<<"Enter command> ";
      getline(cin, userInput);
  }
    
  if(userInput == "#"){ // to quit
      return 0;
  }
  return 0;
}
#include <fstream>
#include <iostream>
#include <iomanip>

#include <boost/algorithm/string.hpp>

#include "Input.h"
#include "VersionInfo.h"

Input::Input()
    :
      commandLineOptions_("Available Options:")
{
    using namespace std;
    using namespace boost::program_options;

    //- Construct the available command line options

    commandLineOptions_.add_options()
            ("help", "| Displays this help message")
            ("version", "| Displays the current version")
            ("user-file", value<string>(),  "| Specify the user input file")
            ("airfoil-file", value<string>(), "| Specify the file containing the airfoil data")
            ("symmetric", value<string>(), "| Takes an arg <yes/no> to specify if the airfoil is symmetric or not");

    //- Construct the input map

    inputMap_["SourcePanels"] = "ON";
    inputMap_["VortexPanels"] = "OFF";
    inputMap_["FreestreamDensity"] = "1.205";
    inputMap_["FreestreamVelocityUnits"] = "m/s";
    inputMap_["FreestreamVelocity"] = "0";
    inputMap_["AngleOfAttackUnits"] = "degrees";
    inputMap_["AngleOfAttack"] = "0";

}

Input::Input(int argc, const char *argv[], Freestream &freestream, Airfoil& airfoil)
    :
      Input()
{

    readCommandLineArgs(argc, argv, freestream, airfoil);

}

void Input::processBuffer(std::string &buffer)
{
    using namespace boost::algorithm;

    erase_all(buffer, " ");

    buffer = buffer.substr(0, buffer.find("#"));

}

void Input::readCommandLineArgs(int argc, const char *argv[], Freestream &freestream, Airfoil &airfoil)
{
    using namespace std;
    using namespace boost::program_options;

    store(parse_command_line(argc, argv, commandLineOptions_), commandLineVarMap_);
    notify(commandLineVarMap_);

    if(commandLineVarMap_.count("help"))
    {

        cout << commandLineOptions_ << endl;
        exit(0);

    }
    else if (commandLineVarMap_.count("version"))
    {

        versionInfo();
        exit(0);

    }

    if (commandLineVarMap_.count("user-file"))
    {

        readUserInputFile(commandLineVarMap_["user-file"].as<string>(), freestream);

    }
    else
    {

        cout << commandLineOptions_ << endl;
        throw "No user input file specified.";

    }

    if (commandLineVarMap_.count("airfoil-file"))
    {

        readAirfoilCoordinatesFile(commandLineVarMap_["airfoil-file"].as<string>(), airfoil);

    }
    else
    {

        cout << commandLineOptions_ << endl;
        throw "No user airfoil file specified.";

    }
}

void Input::readUserInputFile(std::string filename, Freestream &freestream)
{
    using namespace std;

    string buffer, parameterName, parameterValue;
    ifstream inputFile;

    inputFile.open(filename.c_str());

    if(!inputFile.is_open())
        throw ("User input file \"" + filename + "\" was not found.").c_str();

    while(!inputFile.eof())
    {

        getline(inputFile, buffer);

        processBuffer(buffer);

        if(buffer.empty())
            continue;

        parameterName = buffer.substr(0, buffer.find("="));
        parameterValue = buffer.substr(buffer.find("=") + 1, buffer.back());

        if(inputMap_.count(parameterName) != 0)
        {

            inputMap_[parameterName] = parameterValue;

        }
        else
        {

            throw ("Unrecognized input parameter name \"" + parameterName + "\".").c_str();

        }

    }

    cout << "Finished reading file \"" + filename + "\".\n";

    freestream.acceptInput(inputMap_);

}

void Input::readAirfoilCoordinatesFile(std::string filename, Airfoil &airfoil)
{
    using namespace std;

    double x, y;
    string buffer;
    ifstream inputFile;
    vector<Point3D> nodes;

    inputFile.open(filename.c_str());

    if(!inputFile.is_open())
        throw ("User input file \"" + filename + "\" was not found.").c_str();

    while(!inputFile.eof())
    {

        getline(inputFile, buffer);

        processBuffer(buffer);

        if(buffer.empty())
            continue;

        airfoil.name = buffer;

        cout << "Airfoil name: " + airfoil.name + "\n";

        break;

    }

    while(!inputFile.eof())
    {

        if(!(inputFile >> x))
        {

            throw "A proglem occured in Input::readAirfoilCoordinatesFile.";

        }

        if(!(inputFile >> y))
        {

            throw "A proglem occured in Input::readAirfoilCoordinatesFile.";

        }

        nodes.push_back(Point3D(x, y));

    }

    cout << "Finished reading file \"" + filename + "\".\n"
         << setfill('-') << setw(64) << "\n";

    airfoil.init(nodes);

}

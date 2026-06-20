#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "Input.h"

Input::Input()
{
    // Default values — overridden by the user input file
    inputMap_["SourcePanels"]            = "ON";
    inputMap_["VortexPanels"]            = "ON";
    inputMap_["FreestreamDensity"]       = "1.225";
    inputMap_["FreestreamVelocityUnits"] = "m/s";
    inputMap_["FreestreamVelocity"]      = "0";
    inputMap_["AngleOfAttackUnits"]      = "degrees";
    inputMap_["AngleOfAttack"]           = "0";
}

// ─────────────────────────────────────────────────────────────────────────────

void Input::processBuffer(std::string& buffer)
{
    // Remove all spaces and tabs
    buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
                                [](unsigned char c){ return c == ' ' || c == '\t' || c == '\r'; }),
                 buffer.end());

    // Strip inline comments
    auto commentPos = buffer.find('#');
    if (commentPos != std::string::npos)
        buffer = buffer.substr(0, commentPos);
}

// ─────────────────────────────────────────────────────────────────────────────

void Input::readUserInputFile(const std::string& filename, Freestream& freestream)
{
    using namespace std;

    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("User input file \"" + filename + "\" was not found.");

    string buffer, paramName, paramValue;

    while (getline(file, buffer))
    {
        processBuffer(buffer);
        if (buffer.empty())
            continue;

        auto eqPos = buffer.find('=');
        if (eqPos == string::npos)
            throw runtime_error("Malformed line (no '=') in \"" + filename + "\": " + buffer);

        paramName  = buffer.substr(0, eqPos);
        paramValue = buffer.substr(eqPos + 1);

        if (inputMap_.count(paramName))
            inputMap_[paramName] = paramValue;
        else
            throw runtime_error("Unrecognised parameter \"" + paramName + "\" in \"" + filename + "\".");
    }

    cout << "Finished reading user input file \"" << filename << "\".\n";

    freestream.acceptInput(inputMap_);
}

// ─────────────────────────────────────────────────────────────────────────────

void Input::readAirfoilCoordinatesFile(const std::string& filename, Airfoil& airfoil)
{
    using namespace std;

    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Airfoil file \"" + filename + "\" was not found.");

    string buffer;
    vector<Point3D> nodes;

    // First non-empty line is the airfoil name
    while (getline(file, buffer))
    {
        processBuffer(buffer);
        if (!buffer.empty())
        {
            airfoil.name = buffer;
            cout << "Airfoil: " << airfoil.name << "\n";
            break;
        }
    }

    // Remaining lines: x y coordinate pairs
    double x, y;
    while (file >> x >> y)
        nodes.push_back(Point3D(x, y));

    if (nodes.size() < 3)
        throw runtime_error("Airfoil file \"" + filename + "\" has fewer than 3 coordinate points.");

    cout << "Read " << nodes.size() << " nodes ("
         << (nodes.size() - 1) << " panels).\n"
         << setfill('-') << setw(64) << "" << "\n";

    airfoil.init(nodes);
}

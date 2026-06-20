#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include "Input.h"
#include "Freestream.h"
#include "Airfoil.h"
#include "PanelSolver.h"
#include "SvgPlotter.h"

// ── CLI flags ─────────────────────────────────────────────────────────────────
ABSL_FLAG(std::string, user_file,    "", "Path to the user input file (required)");
ABSL_FLAG(std::string, airfoil_file, "", "Path to the airfoil coordinate file (required)");
ABSL_FLAG(std::string, output,       "results", "Base name for output files (.csv and .svg)");

// ─────────────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[])
{
    using namespace std;

    absl::SetProgramUsageMessage(
        "panelz2 --user_file <input.in> --airfoil_file <airfoil.dat> [--output <basename>]");
    absl::ParseCommandLine(argc, argv);

    // ── Banner ────────────────────────────────────────────────────────────────
    cout << setfill('-') << setw(64) << "" << "\n"
         << "\n"
         << "  Panelz2 — 2-D Hess & Smith Panel Method Solver\n"
         << "  Author: Adam O'Brien  <obrienadam89@gmail.com>\n"
         << "\n"
         << setfill('-') << setw(64) << "" << "\n";

    try
    {
        const string userFile    = absl::GetFlag(FLAGS_user_file);
        const string airfoilFile = absl::GetFlag(FLAGS_airfoil_file);
        const string outputBase  = absl::GetFlag(FLAGS_output);

        if (userFile.empty())
            throw runtime_error("--user_file is required. Use --help for usage.");
        if (airfoilFile.empty())
            throw runtime_error("--airfoil_file is required. Use --help for usage.");

        // ── Read inputs ───────────────────────────────────────────────────────
        Freestream freestream;
        Airfoil    airfoil;
        Input      input;

        input.readUserInputFile(airfoilFile.empty() ? "" : userFile, freestream);
        input.readAirfoilCoordinatesFile(airfoilFile, airfoil);

        // ── Solve ─────────────────────────────────────────────────────────────
        PanelSolver solver(airfoil, freestream);
        solver.solve();

        // ── Write outputs ─────────────────────────────────────────────────────
        solver.writeResults(outputBase + ".csv");

        SvgPlotter plotter(airfoil, freestream, solver);
        plotter.write(outputBase + ".svg");
    }
    catch (const std::exception& e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch (const char* e)
    {
        cerr << "Error: " << e << "\n";
        return 1;
    }

    return 0;
}

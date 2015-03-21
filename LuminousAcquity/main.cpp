// =============================================================================
//  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
// -----------------------------------------------------------------------------
//  Main entry point for the application
//  main creates the Application object and runs it.
// =============================================================================

#include <stdexcept>
#include <iostream>

#include "Application.h"

int main(int argc, char* argv[])
{
  try
  {
    lumacu::Application app = lumacu::Application::initialize(argc, argv);
    app.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl << "Exiting\n";
    return EXIT_FAILURE;
  }
  catch (int status)
  {
    return status;
  }
  catch (...)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

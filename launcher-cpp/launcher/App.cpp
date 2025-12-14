//
// Created by bimba on 11/15/25.
//

#include "Launcher.hpp"
#include "argparse/argparse.hpp"

int main(int argc, char** argv)
{
      argparse::ArgumentParser parser("multiroom");

      parser.add_argument("--snapserver.path")
            .help("Path to snapserver binary file")
            .default_value("./snapserver");
      parser.add_argument("--snapserver.config")
            .help("Path to snapserver config file")
            .default_value("");
      parser.add_argument("--snapclient.path")
            .help("Path to snapclient binary file")
            .default_value("./snapclient");
      parser.add_argument("--server.path")
            .help("Path to server binary file")
            .default_value("./backend");

      try
      {
            parser.parse_args(argc, argv);

            settings settings;

            settings.module.server.bin = parser.get<std::string>("--server.path");
            settings.module.snapclient.bin = parser.get<std::string>("--snapclient.path");
            settings.module.snapserver.bin = parser.get<std::string>("--snapserver.path");
            settings.module.snapserver.config = parser.get<std::string>("--snapserver.config");
            Launcher launcher;
            launcher.launch(settings);
      }
      catch (const std::exception& e)
      {
            std::cerr << e.what() << std::endl;
            std::cerr << parser;
            std::exit(EXIT_FAILURE);
      }
}

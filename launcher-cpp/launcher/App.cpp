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
            .required();
      parser.add_argument("--snapserver.config")
            .help("Path to snapserver config file")
            .default_value("");
      parser.add_argument("--snapclient.path")
            .help("Path to snapclient binary file")
            .required();
      parser.add_argument("--server.path")
            .help("Path to server binary file")
            .required();
      parser.add_argument("--server.port")
            .help("Port to bind for server")
            .required();
      parser.add_argument("--server.static_dir")
            .help("Path to static directory")
            .required();

      try
      {
            parser.parse_args(argc, argv);

            settings settings;

            settings.module.server.bin = parser.get<std::string>("--server.path");
            settings.module.server.port = std::atoi(parser.get<std::string>("--server.port").c_str());
            settings.module.server.static_dir = parser.get<std::string>("--server.static_dir");

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

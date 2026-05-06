/**
 * @file main.cpp
 * @author Sean Kannanaikal
 * @brief core file which houses main routine
 * @version 0.1
 * @date 2026-05-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <boost/program_options.hpp>
#include <iostream>
#include "./lib/returncodes.hpp"
#include "./lib/elf.hpp"

/**
 * @brief main routine of kami which handles argument parsing and calls proper routines based on cli args
 * 
 * @param argc number of arguments in cli
 * @param argv array of strings containing various cli args
 * @return int 
 */
int main(int argc, char *argv[]) 
{
	int retcode;
	std::string filepath;
	try 
	{
		//using boost library to parse cli arguments
		boost::program_options::options_description descriptions{"Options"};
		descriptions.add_options()
			("help", "Help Screen")
			("headers,h", "Headers")
			("file,f", boost::program_options::value<std::string>(&filepath), "File");
		
		boost::program_options::variables_map args;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, descriptions), args);
		boost::program_options::notify(args);

		if (args.count("help")) //output usage information
		{
			std::cout << descriptions << std::endl;
			retcode = returncodes::SUCCESS;
		}

		else if (args.count("headers") && args.count("file")) //if reqesting headers and file is passed in
		{
			//create a binary object of Elf class and print its headers
			Elf binary(filepath);
			std::string headers = binary.print_headers();
			std::cout << headers << std::endl;
			retcode = returncodes::SUCCESS;
		}

		else // assume improper usage and exit cleanly
		{
			std::cout  << "[-] Improper usage check the --help option" << std::endl;
			retcode = returncodes::IMPROPER_USAGE;
		}
	} 
	catch (const boost::program_options::error &ex)
	{
		std::cout << "[-] " << ex.what() << std::endl;
	}
	
	return retcode;
}

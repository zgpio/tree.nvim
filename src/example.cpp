#include "socket.hpp"

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 4)
    {
      std::cerr << "Usage: blocking_tcp <host> <port> <message>\n";
      return 1;
    }

    Socket c;
    c.connect(argv[1], argv[2], 10);
    
    for (int i=0; i < 3; ++i) {
      boost::posix_time::ptime time_sent =
        boost::posix_time::microsec_clock::universal_time();
      
      c.write(argv[3], 10);

      //for (;;)
      //{
        char test[300];
	c.read(test, 1000, 10);
	//std::string line = c.read_line(boost::posix_time::seconds(10));

        // Keep going until we get back the line that was sent.
	std::string line = test;
	std::cout << "received string = " << test << std::endl;
        
	/*
	if (line == argv[3]) {
          break;
	} else {
     
	}
	*/
      //}

      boost::posix_time::ptime time_received =
        boost::posix_time::microsec_clock::universal_time();

      std::cout << "Round trip time: ";
      std::cout << (time_received - time_sent).total_microseconds();
      std::cout << " microseconds\n";
    } 
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

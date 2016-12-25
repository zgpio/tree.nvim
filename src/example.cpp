//
// blocking_tcp_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "socket.hpp"

//----------------------------------------------------------------------

//
// This class manages socket timeouts by applying the concept of a deadline.
// Each asynchronous operation is given a deadline by which it must complete.
// Deadlines are enforced by an "actor" that persists for the lifetime of the
// client object:
//
//  +----------------+
//  |                |     
//  | check_deadline |<---+
//  |                |    |
//  +----------------+    | async_wait()
//              |         |
//              +---------+
//
// If the actor determines that the deadline has expired, the socket is closed
// and any outstanding operations are consequently cancelled. The socket
// operations themselves use boost::lambda function objects as completion
// handlers. For a given socket operation, the client object runs the
// io_service to block thread execution until the actor completes.
//

//----------------------------------------------------------------------

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

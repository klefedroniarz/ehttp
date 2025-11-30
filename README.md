# ehttp
c++ http library

# usage
 ```
 auto r = ehttp::Get("https://httpbin.org/get"); 
 std::cout << "Status: " << r.status << "\n"; 
 std::cout << r.text << "\n";  


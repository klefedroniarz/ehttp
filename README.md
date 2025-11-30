# ehttp
c++ http library

# usage
 ```
 auto r = ehttp::Get("https://httpbin.org/get"); 
 std::cout << "Status: " << r.status << "\n"; 
 std::cout << r.text << "\n";  

auto r = ehttp::Post("https://jsonplaceholder.typicode.com/posts")
             .Body(R"({"title":"foo","body":"bar","userId":1})")
             .Header("Content-Type","application/json")
             .Timeout(5)
             .Send();

std::cout << r.text << "\n";

auto r = ehttp::Put("https://jsonplaceholder.typicode.com/posts/1")
             .Body(R"({"title":"updated"})")
             .Header("Content-Type","application/json")
             .Send();

auto r = ehttp::Patch("https://jsonplaceholder.typicode.com/posts/1")
             .Body(R"({"title":"patched title"})")
             .Header("Content-Type","application/json")
             .Send();

auto r = ehttp::Delete("https://jsonplaceholder.typicode.com/posts/1")
             .Send();

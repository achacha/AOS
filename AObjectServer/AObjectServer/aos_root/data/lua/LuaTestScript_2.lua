print("======Script 2 executing======\r\nQuery string parameters found: ")
local names = aos.getRequestParameterNames();
if nil == names then
  print("No parameters found.\r\n");
else
  print(names .. "\r\n");	
end

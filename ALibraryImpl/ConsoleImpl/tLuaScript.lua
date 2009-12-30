print("\r\n---START---\r\n");
local status = web.HttpGet("ping", "http://localhost/ping");
print("Status: "..status);
if status == 200 then
  print("---HEADER-----\r\n");
  print(objects.emit("ping.response"));
  print("---DOC-----\r\n");
  print(objects.emit("ping.body"));
  print("\r\n---END-----\r\n");
end

print("\r\n---START---\r\n");
local http, doc = web.HttpGet("http://localhost/ping");
print(http);
print("---DOC-----\r\n");
print(doc);
print("\r\n---END-----\r\n");
